/*
 * @date Jan 18, 2018
 * @author Anton Bondarev
 */

#include <inttypes.h>
#include <linux/wait.h>
#include <stdio.h>
#include <unistd.h>

#include <etnaviv_xml/cmdstream.xml.h>
#include <etnaviv_xml/common.xml.h>
#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_hi.xml.h>
#include <kernel/panic.h>
#include <util/log.h>

#include "etnaviv_cmdbuf.h"
#include "etnaviv_compat.h"
#include "etnaviv_drm.h"
#include "etnaviv_drv.h"
#include "etnaviv_gem.h"
#include "etnaviv_gpu.h"

int etnaviv_gpu_get_param(struct etnaviv_gpu *gpu, uint32_t param,
    uint64_t *value) {
	switch (param) {
	case ETNAVIV_PARAM_GPU_MODEL:
		*value = gpu->identity.model;
		break;

	case ETNAVIV_PARAM_GPU_REVISION:
		*value = gpu->identity.revision;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_0:
		*value = gpu->identity.features;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_1:
		*value = gpu->identity.minor_features0;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_2:
		*value = gpu->identity.minor_features1;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_3:
		*value = gpu->identity.minor_features2;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_4:
		*value = gpu->identity.minor_features3;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_5:
		*value = gpu->identity.minor_features4;
		break;

	case ETNAVIV_PARAM_GPU_FEATURES_6:
		*value = gpu->identity.minor_features5;
		break;

	case ETNAVIV_PARAM_GPU_STREAM_COUNT:
		*value = gpu->identity.stream_count;
		break;

	case ETNAVIV_PARAM_GPU_REGISTER_MAX:
		*value = gpu->identity.register_max;
		break;

	case ETNAVIV_PARAM_GPU_THREAD_COUNT:
		*value = gpu->identity.thread_count;
		break;

	case ETNAVIV_PARAM_GPU_VERTEX_CACHE_SIZE:
		*value = gpu->identity.vertex_cache_size;
		break;

	case ETNAVIV_PARAM_GPU_SHADER_CORE_COUNT:
		*value = gpu->identity.shader_core_count;
		break;

	case ETNAVIV_PARAM_GPU_PIXEL_PIPES:
		*value = gpu->identity.pixel_pipes;
		break;

	case ETNAVIV_PARAM_GPU_VERTEX_OUTPUT_BUFFER_SIZE:
		*value = gpu->identity.vertex_output_buffer_size;
		break;

	case ETNAVIV_PARAM_GPU_BUFFER_SIZE:
		*value = gpu->identity.buffer_size;
		break;

	case ETNAVIV_PARAM_GPU_INSTRUCTION_COUNT:
		*value = gpu->identity.instruction_count;
		break;

	case ETNAVIV_PARAM_GPU_NUM_CONSTANTS:
		*value = gpu->identity.num_constants;
		break;

	case ETNAVIV_PARAM_GPU_NUM_VARYINGS:
		*value = gpu->identity.varyings_count;
		break;

	default:
		log_debug("invalid param: %u", param);
		return -EINVAL;
	}

	return 0;
}

#define etnaviv_is_model_rev(gpu, mod, rev)   \
	((gpu)->identity.model == chipModel_##mod \
	    && (gpu)->identity.revision == rev)
#define etnaviv_field(val, field) (((val)&field##__MASK) >> field##__SHIFT)

static void etnaviv_hw_specs(struct etnaviv_gpu *gpu) {
	if (gpu->identity.minor_features0
	    & chipMinorFeatures0_MORE_MINOR_FEATURES) {
		uint32_t specs[4];
		unsigned int streams;

		specs[0] = gpu_read(gpu, VIVS_HI_CHIP_SPECS);
		specs[1] = gpu_read(gpu, VIVS_HI_CHIP_SPECS_2);
		specs[2] = gpu_read(gpu, VIVS_HI_CHIP_SPECS_3);
		specs[3] = gpu_read(gpu, VIVS_HI_CHIP_SPECS_4);

		gpu->identity.stream_count = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_STREAM_COUNT);
		gpu->identity.register_max = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_REGISTER_MAX);
		gpu->identity.thread_count = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_THREAD_COUNT);
		gpu->identity.vertex_cache_size = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_VERTEX_CACHE_SIZE);
		gpu->identity.shader_core_count = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_SHADER_CORE_COUNT);
		gpu->identity.pixel_pipes = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_PIXEL_PIPES);
		gpu->identity.vertex_output_buffer_size = etnaviv_field(specs[0],
		    VIVS_HI_CHIP_SPECS_VERTEX_OUTPUT_BUFFER_SIZE);

		gpu->identity.buffer_size = etnaviv_field(specs[1],
		    VIVS_HI_CHIP_SPECS_2_BUFFER_SIZE);
		gpu->identity.instruction_count = etnaviv_field(specs[1],
		    VIVS_HI_CHIP_SPECS_2_INSTRUCTION_COUNT);
		gpu->identity.num_constants = etnaviv_field(specs[1],
		    VIVS_HI_CHIP_SPECS_2_NUM_CONSTANTS);

		gpu->identity.varyings_count = etnaviv_field(specs[2],
		    VIVS_HI_CHIP_SPECS_3_VARYINGS_COUNT);

		/* This overrides the value from older register if non-zero */
		streams = etnaviv_field(specs[3], VIVS_HI_CHIP_SPECS_4_STREAM_COUNT);
		if (streams)
			gpu->identity.stream_count = streams;
	}

	/* Fill in the stream count if not specified */
	if (gpu->identity.stream_count == 0) {
		if (gpu->identity.model >= 0x1000)
			gpu->identity.stream_count = 4;
		else
			gpu->identity.stream_count = 1;
	}

	/* Convert the register max value */
	if (gpu->identity.register_max)
		gpu->identity.register_max = 1 << gpu->identity.register_max;
	else if (gpu->identity.model == chipModel_GC400)
		gpu->identity.register_max = 32;
	else
		gpu->identity.register_max = 64;

	/* Convert thread count */
	if (gpu->identity.thread_count)
		gpu->identity.thread_count = 1 << gpu->identity.thread_count;
	else if (gpu->identity.model == chipModel_GC400)
		gpu->identity.thread_count = 64;
	else if (gpu->identity.model == chipModel_GC500
	         || gpu->identity.model == chipModel_GC530)
		gpu->identity.thread_count = 128;
	else
		gpu->identity.thread_count = 256;

	if (gpu->identity.vertex_cache_size == 0)
		gpu->identity.vertex_cache_size = 8;

	if (gpu->identity.shader_core_count == 0) {
		if (gpu->identity.model >= 0x1000)
			gpu->identity.shader_core_count = 2;
		else
			gpu->identity.shader_core_count = 1;
	}

	if (gpu->identity.pixel_pipes == 0)
		gpu->identity.pixel_pipes = 1;

	/* Convert virtex buffer size */
	if (gpu->identity.vertex_output_buffer_size) {
		gpu->identity.vertex_output_buffer_size =
		    1 << gpu->identity.vertex_output_buffer_size;
	}
	else if (gpu->identity.model == chipModel_GC400) {
		if (gpu->identity.revision < 0x4000)
			gpu->identity.vertex_output_buffer_size = 512;
		else if (gpu->identity.revision < 0x4200)
			gpu->identity.vertex_output_buffer_size = 256;
		else
			gpu->identity.vertex_output_buffer_size = 128;
	}
	else {
		gpu->identity.vertex_output_buffer_size = 512;
	}

	switch (gpu->identity.instruction_count) {
	case 0:
		if (etnaviv_is_model_rev(gpu, GC2000, 0x5108)
		    || gpu->identity.model == chipModel_GC880)
			gpu->identity.instruction_count = 512;
		else
			gpu->identity.instruction_count = 256;
		break;

	case 1:
		gpu->identity.instruction_count = 1024;
		break;

	case 2:
		gpu->identity.instruction_count = 2048;
		break;

	default:
		gpu->identity.instruction_count = 256;
		break;
	}

	if (gpu->identity.num_constants == 0)
		gpu->identity.num_constants = 168;

	if (gpu->identity.varyings_count == 0) {
		if (gpu->identity.minor_features1 & chipMinorFeatures1_HALTI0)
			gpu->identity.varyings_count = 12;
		else
			gpu->identity.varyings_count = 8;
	}

	/*
	 * For some cores, two varyings are consumed for position, so the
	 * maximum varying count needs to be reduced by one.
	 */
	if (etnaviv_is_model_rev(gpu, GC5000, 0x5434)
	    || etnaviv_is_model_rev(gpu, GC4000, 0x5222)
	    || etnaviv_is_model_rev(gpu, GC4000, 0x5245)
	    || etnaviv_is_model_rev(gpu, GC4000, 0x5208)
	    || etnaviv_is_model_rev(gpu, GC3000, 0x5435)
	    || etnaviv_is_model_rev(gpu, GC2200, 0x5244)
	    || etnaviv_is_model_rev(gpu, GC2100, 0x5108)
	    || etnaviv_is_model_rev(gpu, GC2000, 0x5108)
	    || etnaviv_is_model_rev(gpu, GC1500, 0x5246)
	    || etnaviv_is_model_rev(gpu, GC880, 0x5107)
	    || etnaviv_is_model_rev(gpu, GC880, 0x5106))
		gpu->identity.varyings_count -= 1;
}

static void etnaviv_hw_identify(struct etnaviv_gpu *gpu) {
	uint32_t chipIdentity;

	chipIdentity = gpu_read(gpu, VIVS_HI_CHIP_IDENTITY);

	/* Special case for older graphic cores. */
	if (etnaviv_field(chipIdentity, VIVS_HI_CHIP_IDENTITY_FAMILY) == 0x01) {
		gpu->identity.model = chipModel_GC500;
		gpu->identity.revision = etnaviv_field(chipIdentity,
		    VIVS_HI_CHIP_IDENTITY_REVISION);
	}
	else {
		gpu->identity.model = gpu_read(gpu, VIVS_HI_CHIP_MODEL);
		gpu->identity.revision = gpu_read(gpu, VIVS_HI_CHIP_REV);

		/*
		 * !!!! HACK ALERT !!!!
		 * Because people change device IDs without letting software
		 * know about it - here is the hack to make it all look the
		 * same.  Only for GC400 family.
		 */
		if ((gpu->identity.model & 0xff00) == 0x0400
		    && gpu->identity.model != chipModel_GC420) {
			gpu->identity.model = gpu->identity.model & 0x0400;
		}

		/* Another special case */
		if (etnaviv_is_model_rev(gpu, GC300, 0x2201)) {
			uint32_t chipDate = gpu_read(gpu, VIVS_HI_CHIP_DATE);
			uint32_t chipTime = gpu_read(gpu, VIVS_HI_CHIP_TIME);

			if (chipDate == 0x20080814 && chipTime == 0x12051100) {
				/*
				 * This IP has an ECO; put the correct
				 * revision in it.
				 */
				gpu->identity.revision = 0x1051;
			}
		}

		/*
		 * NXP likes to call the GPU on the i.MX6QP GC2000+, but in
		 * reality it's just a re-branded GC3000. We can identify this
		 * core by the upper half of the revision register being all 1.
		 * Fix model/rev here, so all other places can refer to this
		 * core by its real identity.
		 */
		if (etnaviv_is_model_rev(gpu, GC2000, 0xffff5450)) {
			gpu->identity.model = chipModel_GC3000;
			gpu->identity.revision &= 0xffff;
		}
	}

	log_info("model: GC%" PRIu32 ", revision: %" PRIu32, gpu->identity.model,
	    gpu->identity.revision);

	gpu->identity.features = gpu_read(gpu, VIVS_HI_CHIP_FEATURE);

	/* Disable fast clear on GC700. */
	if (gpu->identity.model == chipModel_GC700)
		gpu->identity.features &= ~chipFeatures_FAST_CLEAR;

	if ((gpu->identity.model == chipModel_GC500 && gpu->identity.revision < 2)
	    || (gpu->identity.model == chipModel_GC300
	        && gpu->identity.revision < 0x2000)) {
		/*
		 * GC500 rev 1.x and GC300 rev < 2.0 doesn't have these
		 * registers.
		 */
		gpu->identity.minor_features0 = 0;
		gpu->identity.minor_features1 = 0;
		gpu->identity.minor_features2 = 0;
		gpu->identity.minor_features3 = 0;
		gpu->identity.minor_features4 = 0;
		gpu->identity.minor_features5 = 0;
	}
	else
		gpu->identity.minor_features0 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_0);

	if (gpu->identity.minor_features0
	    & chipMinorFeatures0_MORE_MINOR_FEATURES) {
		gpu->identity.minor_features1 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_1);
		gpu->identity.minor_features2 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_2);
		gpu->identity.minor_features3 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_3);
		gpu->identity.minor_features4 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_4);
		gpu->identity.minor_features5 = gpu_read(gpu,
		    VIVS_HI_CHIP_MINOR_FEATURE_5);
	}

	/* GC600 idle register reports zero bits where modules aren't present */
	if (gpu->identity.model == chipModel_GC600) {
		gpu->idle_mask = VIVS_HI_IDLE_STATE_TX | VIVS_HI_IDLE_STATE_RA
		                 | VIVS_HI_IDLE_STATE_SE | VIVS_HI_IDLE_STATE_PA
		                 | VIVS_HI_IDLE_STATE_SH | VIVS_HI_IDLE_STATE_PE
		                 | VIVS_HI_IDLE_STATE_DE | VIVS_HI_IDLE_STATE_FE;
	}
	else {
		gpu->idle_mask = ~VIVS_HI_IDLE_STATE_AXI_LP;
	}

	etnaviv_hw_specs(gpu);
}

static void etnaviv_gpu_load_clock(struct etnaviv_gpu *gpu, uint32_t clock) {
	gpu_write(gpu, VIVS_HI_CLOCK_CONTROL,
	    clock | VIVS_HI_CLOCK_CONTROL_FSCALE_CMD_LOAD);
	gpu_write(gpu, VIVS_HI_CLOCK_CONTROL, clock);
}

static void etnaviv_gpu_update_clock(struct etnaviv_gpu *gpu) {
	unsigned int fscale = 1 << (6 - 1); //gpu->freq_scale);
	uint32_t clock = VIVS_HI_CLOCK_CONTROL_DISABLE_DEBUG_REGISTERS
	                 | VIVS_HI_CLOCK_CONTROL_FSCALE_VAL(fscale);

	etnaviv_gpu_load_clock(gpu, clock);
}

static int etnaviv_hw_reset(struct etnaviv_gpu *gpu) {
	uint32_t control, idle;
	bool failed = true;

	/* TODO
	 *
	 * - clock gating
	 * - puls eater
	 * - what about VG?
	 */

	/* We hope that the GPU resets in under one second */
	while (1) {
		/* enable clock */
		etnaviv_gpu_update_clock(gpu);

		control = gpu_read(gpu, VIVS_HI_CLOCK_CONTROL);

		/* Wait for stable clock.  Vivante's code waited for 1ms */
		usleep(1500);

		/* isolate the GPU. */
		control |= VIVS_HI_CLOCK_CONTROL_ISOLATE_GPU;
		gpu_write(gpu, VIVS_HI_CLOCK_CONTROL, control);

		/* set soft reset. */
		control |= VIVS_HI_CLOCK_CONTROL_SOFT_RESET;
		gpu_write(gpu, VIVS_HI_CLOCK_CONTROL, control);

		/* wait for reset. */
		usleep(1500);

		/* reset soft reset bit. */
		control &= ~VIVS_HI_CLOCK_CONTROL_SOFT_RESET;
		gpu_write(gpu, VIVS_HI_CLOCK_CONTROL, control);

		/* reset GPU isolation. */
		control &= ~VIVS_HI_CLOCK_CONTROL_ISOLATE_GPU;
		gpu_write(gpu, VIVS_HI_CLOCK_CONTROL, control);

		/* read idle register. */
		idle = gpu_read(gpu, VIVS_HI_IDLE_STATE);

		/* try reseting again if FE it not idle */
		if ((idle & VIVS_HI_IDLE_STATE_FE) == 0) {
			log_debug("FE is not idle");
			continue;
		}

		/* read reset register. */
		control = gpu_read(gpu, VIVS_HI_CLOCK_CONTROL);

		/* is the GPU idle? */
		if (((control & VIVS_HI_CLOCK_CONTROL_IDLE_3D) == 0)
		    || ((control & VIVS_HI_CLOCK_CONTROL_IDLE_2D) == 0)) {
			log_debug("GPU is not idle");
			continue;
		}

		failed = false;
		break;
	}

	if (failed) {
		idle = gpu_read(gpu, VIVS_HI_IDLE_STATE);
		control = gpu_read(gpu, VIVS_HI_CLOCK_CONTROL);

		log_error("GPU failed to reset: FE %sidle, 3D %sidle, 2D %sidle",
		    idle & VIVS_HI_IDLE_STATE_FE ? "" : "not ",
		    control & VIVS_HI_CLOCK_CONTROL_IDLE_3D ? "" : "not ",
		    control & VIVS_HI_CLOCK_CONTROL_IDLE_2D ? "" : "not ");

		return -EBUSY;
	}

	/* We rely on the GPU running, so program the clock */
	etnaviv_gpu_update_clock(gpu);

	return 0;
}

static void etnaviv_gpu_enable_mlcg(struct etnaviv_gpu *gpu) {
	uint32_t pmc, ppc;

	/* enable clock gating */
	ppc = gpu_read(gpu, VIVS_PM_POWER_CONTROLS);
	ppc |= VIVS_PM_POWER_CONTROLS_ENABLE_MODULE_CLOCK_GATING;

	/* Disable stall module clock gating for 4.3.0.1 and 4.3.0.2 revs */
	if (gpu->identity.revision == 0x4301 || gpu->identity.revision == 0x4302)
		ppc |= VIVS_PM_POWER_CONTROLS_DISABLE_STALL_MODULE_CLOCK_GATING;

	gpu_write(gpu, VIVS_PM_POWER_CONTROLS, ppc);

	pmc = gpu_read(gpu, VIVS_PM_MODULE_CONTROLS);

	/* Disable PA clock gating for GC400+ except for GC420 */
	if (gpu->identity.model >= chipModel_GC400
	    && gpu->identity.model != chipModel_GC420)
		pmc |= VIVS_PM_MODULE_CONTROLS_DISABLE_MODULE_CLOCK_GATING_PA;

	/*
	 * Disable PE clock gating on revs < 5.0.0.0 when HZ is
	 * present without a bug fix.
	 */
	if (gpu->identity.revision < 0x5000
	    && gpu->identity.minor_features0 & chipMinorFeatures0_HZ
	    && !(gpu->identity.minor_features1
	         & chipMinorFeatures1_DISABLE_PE_GATING))
		pmc |= VIVS_PM_MODULE_CONTROLS_DISABLE_MODULE_CLOCK_GATING_PE;

	if (gpu->identity.revision < 0x5422)
		pmc |= BIT(15); /* Unknown bit */

	pmc |= VIVS_PM_MODULE_CONTROLS_DISABLE_MODULE_CLOCK_GATING_RA_HZ;
	pmc |= VIVS_PM_MODULE_CONTROLS_DISABLE_MODULE_CLOCK_GATING_RA_EZ;

	gpu_write(gpu, VIVS_PM_MODULE_CONTROLS, pmc);
}

void etnaviv_gpu_start_fe(struct etnaviv_gpu *gpu, uint32_t address,
    uint16_t prefetch) {
	gpu_write(gpu, VIVS_FE_COMMAND_ADDRESS, address);
	gpu_write(gpu, VIVS_FE_COMMAND_CONTROL,
	    VIVS_FE_COMMAND_CONTROL_ENABLE
	        | VIVS_FE_COMMAND_CONTROL_PREFETCH(prefetch));
}

static void etnaviv_gpu_setup_pulse_eater(struct etnaviv_gpu *gpu) {
	/*
	 * Base value for VIVS_PM_PULSE_EATER register on models where it
	 * cannot be read, extracted from vivante kernel driver.
	 */
	uint32_t pulse_eater = 0x01590880;

	if (etnaviv_is_model_rev(gpu, GC4000, 0x5208)
	    || etnaviv_is_model_rev(gpu, GC4000, 0x5222)) {
		pulse_eater |= BIT(23);
	}

	if (etnaviv_is_model_rev(gpu, GC1000, 0x5039)
	    || etnaviv_is_model_rev(gpu, GC1000, 0x5040)) {
		pulse_eater &= ~BIT(16);
		pulse_eater |= BIT(17);
	}
	if ((gpu->identity.revision > 0x5420)
	    && (gpu->identity.features & chipFeatures_PIPE_3D)) {
		/* Performance fix: disable internal DFS */
		pulse_eater = gpu_read(gpu, VIVS_PM_PULSE_EATER);
		pulse_eater |= BIT(18);
	}

	gpu_write(gpu, VIVS_PM_PULSE_EATER, pulse_eater);
}

static void etnaviv_gpu_hw_init(struct etnaviv_gpu *gpu) {
	uint16_t prefetch;

	/* enable module-level clock gating */
	etnaviv_gpu_enable_mlcg(gpu);

	/*
	 * Update GPU AXI cache atttribute to "cacheable, no allocate".
	 * This is necessary to prevent the iMX6 SoC locking up.
	 */
	gpu_write(gpu, VIVS_HI_AXI_CONFIG,
	    VIVS_HI_AXI_CONFIG_AWCACHE(2) | VIVS_HI_AXI_CONFIG_ARCACHE(2));

	/* GC2000 rev 5108 needs a special bus config */
	if (etnaviv_is_model_rev(gpu, GC2000, 0x5108)) {
		uint32_t bus_config = gpu_read(gpu, VIVS_MC_BUS_CONFIG);
		bus_config &= ~(VIVS_MC_BUS_CONFIG_FE_BUS_CONFIG__MASK
		                | VIVS_MC_BUS_CONFIG_TX_BUS_CONFIG__MASK);
		bus_config |= VIVS_MC_BUS_CONFIG_FE_BUS_CONFIG(1)
		              | VIVS_MC_BUS_CONFIG_TX_BUS_CONFIG(0);
		gpu_write(gpu, VIVS_MC_BUS_CONFIG, bus_config);
	}

	/* setup the pulse eater */
	etnaviv_gpu_setup_pulse_eater(gpu);

	/* setup the MMU */
	etnaviv_iommu_restore(gpu);

	/* Start command processor */
	gpu_write(gpu, VIVS_HI_INTR_ENBL, ~0U);

	prefetch = etnaviv_buffer_init(gpu);

	etnaviv_gpu_start_fe(gpu, etnaviv_cmdbuf_get_va(gpu->buffer), prefetch);
}

int etnaviv_gpu_init(struct etnaviv_gpu *gpu) {
	etnaviv_hw_identify(gpu);

	if (gpu->identity.model == 0) {
		log_error("Unknown GPU model");
		return -ENXIO;
	}

	/* Exclude VG cores with FE2.0 */
	if (gpu->identity.features & chipFeatures_PIPE_VG
	    && gpu->identity.features & chipFeatures_FE20) {
		panic("Wrong GPU register values, try to restart");
		return -ENXIO;
	}

	/*
	 * Set the GPU linear window to be at the end of the DMA window, where
	 * the CMA area is likely to reside. This ensures that we are able to
	 * map the command buffers while having the linear window overlap as
	 * much RAM as possible, so we can optimize mappings for other buffers.
	 *
	 * For 3D cores only do this if MC2.0 is present, as with MC1.0 it leads
	 * to different views of the memory on the individual engines.
	 */
	if (!(gpu->identity.features & chipFeatures_PIPE_3D)
	    || (gpu->identity.minor_features0 & chipMinorFeatures0_MC20)) {
		uint32_t dma_mask = (uint32_t)dma_get_required_mask(gpu);
		if (dma_mask < PHYS_OFFSET + SZ_2G)
			gpu->memory_base = PHYS_OFFSET;
		else
			gpu->memory_base = dma_mask - SZ_2G + 1;
	}
	else if (PHYS_OFFSET >= SZ_2G) {
		log_info("Need to move linear window on MC1.0, disabling TS");
		gpu->memory_base = PHYS_OFFSET;
		gpu->identity.features &= ~chipFeatures_FAST_CLEAR;
	}

	log_debug("memory base %p\n", (void *)gpu->memory_base);

	if (etnaviv_hw_reset(gpu)) {
		log_error("GPU reset failed");
		return -1;
	}

	if (etnaviv_iommu_init(gpu)) {
		log_error("Failed to instantiate GPU IOMMU");
		return -1;
	}

	gpu->cmdbuf_suballoc = etnaviv_cmdbuf_suballoc_new(gpu);
	if (IS_ERR(gpu->cmdbuf_suballoc)) {
		log_error("Failed to create cmdbuf suballocator\n");
		return -1;
	}
	/* Create buffer: */
	gpu->buffer = etnaviv_cmdbuf_new(gpu->cmdbuf_suballoc, 4096, 0);
	if (!gpu->buffer) {
		log_error("could not create command buffer");
		return -ENOMEM;
	}

	if (gpu->mmu.version == ETNAVIV_IOMMU_V1
	    && etnaviv_cmdbuf_get_va(gpu->buffer) > 0x80000000) {
		log_debug("buffer %p va %p\n", gpu->buffer,
		    etnaviv_cmdbuf_get_va(gpu->buffer));
		log_error("command buffer outside valid memory window");
		return -EINVAL;
	}

	etnaviv_gpu_hw_init(gpu);

	return 0;
}

struct dma_debug {
	uint32_t address[2];
	uint32_t state[2];
};

static void verify_dma(struct etnaviv_gpu *gpu, struct dma_debug *debug) {
	uint32_t i;

	debug->address[0] = gpu_read(gpu, VIVS_FE_DMA_ADDRESS);
	debug->state[0] = gpu_read(gpu, VIVS_FE_DMA_DEBUG_STATE);

	for (i = 0; i < 500; i++) {
		debug->address[1] = gpu_read(gpu, VIVS_FE_DMA_ADDRESS);
		debug->state[1] = gpu_read(gpu, VIVS_FE_DMA_DEBUG_STATE);

		if (debug->address[0] != debug->address[1])
			break;

		if (debug->state[0] != debug->state[1])
			break;
	}
}

#define seq_puts(a, ...)   printf(__VA_ARGS__)
#define seq_printf(a, ...) printf(__VA_ARGS__)
struct seq_file;
int etnaviv_gpu_debugfs(struct etnaviv_gpu *gpu, char *s) {
	struct dma_debug debug;
	uint32_t dma_lo, dma_hi, axi, idle;
	int ret;
	int cmdState, cmdDmaState, cmdFetState, dmaReqState, calState, veReqState;

	static const char *_cmdState[] = {"PAR_IDLE_ST", "PAR_DEC_ST",
	    "PAR_ADR0_ST", "PAR_LOAD0_ST", "PAR_ADR1_ST", "PAR_LOAD1_ST",
	    "PAR_3DADR_ST", "PAR_3DCMD_ST", "PAR_3DCNTL_ST", "PAR_3DIDXCNTL_ST",
	    "PAR_INITREQDMA_ST", "PAR_DRAWIDX_ST", "PAR_DRAW_ST", "PAR_2DRECT0_ST",
	    "PAR_2DRECT1_ST", "PAR_2DDATA0_ST", "PAR_2DDATA1_ST", "PAR_WAITFIFO_ST",
	    "PAR_WAIT_ST", "PAR_LINK_ST", "PAR_END_ST", "PAR_STALL_ST"};

	static const char *_cmdDmaState[] = {"CMD_IDLE_ST", "CMD_START_ST",
	    "CMD_REQ_ST", "CMD_END_ST"};

	static const char *_cmdFetState[] = {"FET_IDLE_ST", "FET_RAMVALID_ST",
	    "FET_VALID_ST"};

	static const char *_reqDmaState[] = {"REQ_IDLE_ST", "REQ_WAITIDX_ST",
	    "REQ_CAL_ST"};

	static const char *_calState[] = {"CAL_IDLE_ST", "CAL_LDADR_ST",
	    "CAL_IDXCALC_ST"};

	static const char *_veReqState[] = {"VER_IDLE_ST", "VER_CKCACHE_ST",
	    "VER_MISS_ST"};

	seq_printf(m, "%s Status:\n", s);

	dma_lo = gpu_read(gpu, VIVS_FE_DMA_LOW);
	dma_hi = gpu_read(gpu, VIVS_FE_DMA_HIGH);
	axi = gpu_read(gpu, VIVS_HI_AXI_STATUS);
	idle = gpu_read(gpu, VIVS_HI_IDLE_STATE);

	verify_dma(gpu, &debug);

	seq_puts(m, "\tfeatures\n");
	seq_printf(m, "\t minor_features0: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features0);
	seq_printf(m, "\t minor_features1: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features1);
	seq_printf(m, "\t minor_features2: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features2);
	seq_printf(m, "\t minor_features3: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features3);
	seq_printf(m, "\t minor_features4: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features4);
	seq_printf(m, "\t minor_features5: 0x%08" PRIu32 "\n",
	    gpu->identity.minor_features5);

	seq_puts(m, "\tspecs\n");
	seq_printf(m, "\t stream_count:  %" PRIu32 "\n",
	    gpu->identity.stream_count);
	seq_printf(m, "\t register_max: %" PRIu32 "\n", gpu->identity.register_max);
	seq_printf(m, "\t thread_count: %" PRIu32 "\n", gpu->identity.thread_count);
	seq_printf(m, "\t vertex_cache_size: %" PRIu32 "\n",
	    gpu->identity.vertex_cache_size);
	seq_printf(m, "\t shader_core_count: %" PRIu32 "\n",
	    gpu->identity.shader_core_count);
	seq_printf(m, "\t pixel_pipes: %" PRIu32 "\n", gpu->identity.pixel_pipes);
	seq_printf(m, "\t vertex_output_buffer_size: %" PRIu32 "\n",
	    gpu->identity.vertex_output_buffer_size);
	seq_printf(m, "\t buffer_size: %" PRIu32 "\n", gpu->identity.buffer_size);
	seq_printf(m, "\t instruction_count: %" PRIu32 "\n",
	    gpu->identity.instruction_count);
	seq_printf(m, "\t num_constants: %" PRIu32 "\n",
	    gpu->identity.num_constants);
	seq_printf(m, "\t varyings_count: %" PRIu8 "\n",
	    gpu->identity.varyings_count);

	seq_printf(m, "\taxi: 0x%08" PRIu32 "\n", axi);
	seq_printf(m, "\tidle: 0x%08" PRIu32 "\n", idle);
	idle |= ~gpu->idle_mask & ~VIVS_HI_IDLE_STATE_AXI_LP;
	if ((idle & VIVS_HI_IDLE_STATE_FE) == 0)
		seq_puts(m, "\t FE is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_DE) == 0)
		seq_puts(m, "\t DE is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_PE) == 0)
		seq_puts(m, "\t PE is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_SH) == 0)
		seq_puts(m, "\t SH is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_PA) == 0)
		seq_puts(m, "\t PA is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_SE) == 0)
		seq_puts(m, "\t SE is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_RA) == 0)
		seq_puts(m, "\t RA is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_TX) == 0)
		seq_puts(m, "\t TX is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_VG) == 0)
		seq_puts(m, "\t VG is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_IM) == 0)
		seq_puts(m, "\t IM is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_FP) == 0)
		seq_puts(m, "\t FP is not idle\n");
	if ((idle & VIVS_HI_IDLE_STATE_TS) == 0)
		seq_puts(m, "\t TS is not idle\n");
	if (idle & VIVS_HI_IDLE_STATE_AXI_LP)
		seq_puts(m, "\t AXI low power mode\n");

	if (gpu->identity.features & chipFeatures_DEBUG_MODE) {
		uint32_t read0 = gpu_read(gpu, VIVS_MC_DEBUG_READ0);
		uint32_t read1 = gpu_read(gpu, VIVS_MC_DEBUG_READ1);
		uint32_t write = gpu_read(gpu, VIVS_MC_DEBUG_WRITE);

		seq_puts(m, "\tMC\n");
		seq_printf(m, "\t read0: 0x%08" PRIu32 "\n", read0);
		seq_printf(m, "\t read1: 0x%08" PRIu32 "\n", read1);
		seq_printf(m, "\t write: 0x%08" PRIu32 "\n", write);
	}

	seq_puts(m, "\tDMA ");

	if (debug.address[0] == debug.address[1]
	    && debug.state[0] == debug.state[1]) {
		seq_puts(m, "seems to be stuck\n");
	}
	else if (debug.address[0] == debug.address[1]) {
		seq_puts(m, "address is constant\n");
	}
	else {
		seq_puts(m, "is running\n");
	}

	cmdState = debug.state[1] & 0x1F;
	cmdDmaState = (debug.state[1] >> 8) & 0x03;
	cmdFetState = (debug.state[1] >> 10) & 0x03;
	dmaReqState = (debug.state[1] >> 12) & 0x03;
	calState = (debug.state[1] >> 14) & 0x03;
	veReqState = (debug.state[1] >> 16) & 0x03;

	seq_printf(m, "\t address 0: 0x%08" PRIu32 "\n", debug.address[0]);
	seq_printf(m, "\t address 1: 0x%08" PRIu32 "\n", debug.address[1]);
	seq_printf(m, "\t state 0: 0x%08" PRIu32 "\n", debug.state[0]);
	seq_printf(m, "\t state 1: 0x%08" PRIu32 "\n", debug.state[1]);
	seq_printf(m, "\t    command state       = %d (%s)\n", cmdState,
	    _cmdState[cmdState]);
	seq_printf(m, "\t    command DMA state   = %d (%s)\n", cmdDmaState,
	    _cmdDmaState[cmdDmaState]);
	seq_printf(m, "\t    command fetch state = %d (%s)\n", cmdFetState,
	    _cmdFetState[cmdFetState]);
	seq_printf(m, "\t    DMA request state   = %d (%s)\n", dmaReqState,
	    _reqDmaState[dmaReqState]);
	seq_printf(m, "\t    cal state           = %d (%s)\n", calState,
	    _calState[calState]);
	seq_printf(m, "\t    VE request state    = %d (%s)\n", veReqState,
	    _veReqState[veReqState]);
	seq_printf(m,
	    "\t last fetch 64 bit word: 0x%08" PRIu32 " 0x%08" PRIu32 "\n", dma_lo,
	    dma_hi);

	ret = 0;

	return ret;
}

int etnaviv_gpu_wait_fence_interruptible(struct etnaviv_gpu *gpu,
    uint32_t fence, struct timespec *timeout) {
	int ret;

	if (fence_after(fence, gpu->next_fence)) {
		log_error("waiting on invalid fence: %u (of %u)\n", fence,
		    gpu->next_fence);
		return -EINVAL;
	}

	if (!timeout) {
		/* No timeout was requested: just test for completion */
		ret = fence_completed(gpu, fence) ? 0 : -EBUSY;
	}
	else {
		unsigned long remaining = etnaviv_timeout_to_jiffies(timeout);

		ret = wait_event_interruptible_timeout(gpu->fence_event,
		    fence_completed(gpu, fence), remaining);
		if (ret == 0) {
			log_debug("timeout waiting for fence: %u (retired: %u completed: "
			          "%u)",
			    fence, gpu->retired_fence, gpu->completed_fence);
			ret = -ETIMEDOUT;
		}
	}

	return ret;
}
/* add bo's to gpu's ring, and kick gpu: */
extern void etnaviv_buffer_dump(struct etnaviv_gpu *gpu,
    struct etnaviv_cmdbuf *buf, uint32_t off, uint32_t len);

int etnaviv_gpu_submit(struct etnaviv_gpu *gpu,
    struct etnaviv_gem_submit *submit, struct etnaviv_cmdbuf *cmdbuf) {
	unsigned int event = 0;
	/*
	 * TODO
	 * - flush
	 * - data endian
	 * - prefetch
	 */
	if (gpu->lastctx != cmdbuf->ctx) {
		gpu->mmu.need_flush = true;
		gpu->switch_context = true;
		gpu->lastctx = cmdbuf->ctx;
	}

	while (gpu->busy) {}; /* Wait for interrupt for previous command buffer */
	gpu->busy = 1;

	etnaviv_buffer_queue(gpu, event, cmdbuf);
	cmdbuf->nr_bos = submit->nr_bos;
	etnaviv_buffer_dump(gpu, cmdbuf, 0, cmdbuf->user_size);
	etnaviv_buffer_dump(gpu, gpu->buffer, 0, gpu->buffer->user_size);

	return 0;
}

int etnaviv_gpu_wait_idle(struct etnaviv_gpu *gpu, unsigned int timeout_ms) {
	timeout_ms *= 0xfff;
	do {
		uint32_t idle = gpu_read(gpu, VIVS_HI_IDLE_STATE);

		if ((idle & gpu->idle_mask) == gpu->idle_mask)
			return 0;

		timeout_ms--;

		if (timeout_ms == 0) {
			log_warning("timed out waiting for idle: idle=0x%x", idle);
			return -ETIMEDOUT;
		}
	} while (1);
}
