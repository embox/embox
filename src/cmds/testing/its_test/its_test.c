/**
 * @file
 * @brief GICv3 ITS command queue and LPI delivery self test.
 *
 * @author zhugengyu
 * @date 06.09.2026
 *
 * Maps a grid of synthetic devices and events onto LPIs through the
 * ITS command queue, then injects INT commands and checks that every
 * LPI reaches its own handler, that disabled LPIs stay silent and
 * that re-enabled LPIs fire again. Requires an interrupt controller
 * with an ITS (e.g. a GIC-600) and the gicv3_its driver.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <drivers/interrupt/gic/gic_lpi.h>
#include <drivers/interrupt/gic/gicv3_its.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#define ITS_TEST_DEV_CNT OPTION_GET(NUMBER, dev_cnt)
#define ITS_TEST_EVT_CNT OPTION_GET(NUMBER, evt_cnt)

/* Synthetic device ids: the values are arbitrary, what matters is
 * that every device owns an independent ITT. Stay out of the low
 * range a PCIe requester id occupies, MAPTI would silently rebind
 * the endpoint's events. */
#define ITS_TEST_DEVID(dev) ((uint32_t)(0xE000 + (dev)))

/* Bounded waits: an LPI arrives within microseconds of the INT
 * command, so exhausting the loop means the interrupt was lost. */
#define ITS_TEST_TRIES 10000000

/* Stride of the interleaved pass, co-prime with the grid sizes so
 * consecutive iterations hop between devices and events. */
#define ITS_TEST_STRIDE 7

static int test_irq[ITS_TEST_DEV_CNT][ITS_TEST_EVT_CNT];

static volatile int test_last_irq = -1;

static irq_return_t its_test_handler(unsigned int irq_nr, void *data) {
	test_last_irq = (int)irq_nr;

	return IRQ_HANDLED;
}

static int its_test_wait_delivery(int expected_irq) {
	int tries = ITS_TEST_TRIES;

	while (--tries >= 0) {
		if (test_last_irq == expected_irq) {
			return 0;
		}
	}

	return -ETIMEDOUT;
}

static int its_test_expect_silence(void) {
	int tries = ITS_TEST_TRIES;

	test_last_irq = -1;
	while (--tries >= 0) {
		/* Nothing to do: a stray delivery shows up as a change
		 * of test_last_irq. */
	}

	return (test_last_irq == -1) ? 0 : -EIO;
}

static int its_test_send(int dev, int evt) {
	return gic_its_send_int(ITS_TEST_DEVID(dev), (uint32_t)evt);
}

static int its_test_map_grid(void) {
	int dev;
	int evt;

	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		int ret = gic_its_device_attach(ITS_TEST_DEVID(dev));

		if (ret != 0) {
			printf("its_test: device_attach(0x%02" PRIx32 ") failed: %d\n",
			    ITS_TEST_DEVID(dev), ret);
			return ret;
		}

		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			int irq = gic_its_event_map(ITS_TEST_DEVID(dev), (uint32_t)evt);

			if (irq < 0) {
				printf("its_test: event_map(0x%02" PRIx32 ", %d) failed: %d\n",
				    ITS_TEST_DEVID(dev), evt, irq);
				return irq;
			}

			/* Attaching arms the LPI: the kernel enables the
			 * line and the ITS driver turns on the property
			 * table entry. */
			ret = irq_attach((unsigned int)irq, its_test_handler, 0, NULL,
			    "its_test");
			if (ret != 0) {
				printf("its_test: irq_attach(%d) failed: %d\n", irq, ret);
				return ret;
			}
			test_irq[dev][evt] = irq;
		}
	}

	return 0;
}

static void its_test_unmap_grid(void) {
	int dev;
	int evt;

	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			if (test_irq[dev][evt] < 0) {
				continue;
			}
			irq_detach((unsigned int)test_irq[dev][evt], NULL);
			gic_its_event_unmap(ITS_TEST_DEVID(dev), (uint32_t)evt);
		}
	}
}

static int its_test_simple_int(void) {
	int ret = its_test_send(0, 0);

	if (ret == 0) {
		ret = its_test_wait_delivery(test_irq[0][0]);
	}
	if (ret != 0) {
		printf("its_test: INT dev 0x%02" PRIx32 " event 0 not delivered: %d\n",
		    ITS_TEST_DEVID(0), ret);
		return ret;
	}

	printf("its_test: single INT delivered to LPI irq %d\n", test_irq[0][0]);

	return 0;
}

static int its_test_disable_enable(void) {
	const int dev = ITS_TEST_DEV_CNT - 1;
	const int evt = ITS_TEST_EVT_CNT - 1;
	const unsigned int irq = (unsigned int)test_irq[dev][evt];
	int ret;

	irqctrl_disable(irq);
	ret = its_test_send(dev, evt);
	if (ret == 0) {
		ret = its_test_expect_silence();
	}
	if (ret == 0) {
		/* Drop anything the ITS buffered while disabled, then
		 * arm the LPI again and expect the next INT to fire. */
		gic_its_send_clear(ITS_TEST_DEVID(dev), (uint32_t)evt);
		irqctrl_enable(irq);
		ret = its_test_send(dev, evt);
	}
	if (ret == 0) {
		ret = its_test_wait_delivery(test_irq[dev][evt]);
	}

	if (ret != 0) {
		printf("its_test: disable/re-enable of irq %u failed: %d\n", irq, ret);
		return ret;
	}

	printf("its_test: disabled LPI stayed silent, re-enabled one fired\n");

	return 0;
}

static int its_test_grid_silent_then_refire(void) {
	int dev;
	int evt;
	int ret;

	/* Disable the whole grid, then hit every pair: nothing may be
	 * delivered while the property entries say disabled. */
	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			irqctrl_disable((unsigned int)test_irq[dev][evt]);
		}
	}

	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			ret = its_test_send(dev, evt);
			if (ret != 0) {
				printf("its_test: INT dev %d event %d failed: %d\n", dev, evt, ret);
				return ret;
			}
		}
	}
	ret = its_test_expect_silence();
	if (ret != 0) {
		printf("its_test: a disabled LPI was delivered: %d\n", ret);
		return ret;
	}

	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			gic_its_send_clear(ITS_TEST_DEVID(dev), (uint32_t)evt);
			irqctrl_enable((unsigned int)test_irq[dev][evt]);
		}
	}

	for (dev = 0; dev < ITS_TEST_DEV_CNT; dev++) {
		for (evt = 0; evt < ITS_TEST_EVT_CNT; evt++) {
			test_last_irq = -1;
			ret = its_test_send(dev, evt);
			if (ret == 0) {
				ret = its_test_wait_delivery(test_irq[dev][evt]);
			}
			if (ret != 0) {
				printf("its_test: re-armed dev %d event %d failed: %d\n", dev,
				    evt, ret);
				return ret;
			}
		}
	}

	printf("its_test: %d LPIs silent when disabled, all fired when "
	       "re-armed\n",
	    ITS_TEST_DEV_CNT * ITS_TEST_EVT_CNT);

	return 0;
}

static int its_test_interleaved_loop(void) {
	const int total = ITS_TEST_DEV_CNT * ITS_TEST_EVT_CNT;
	int i;
	int ret;

	for (i = 0; i < total; i++) {
		const int n = (i * ITS_TEST_STRIDE) % total;
		const int dev = n / ITS_TEST_EVT_CNT;
		const int evt = n % ITS_TEST_EVT_CNT;

		test_last_irq = -1;
		ret = its_test_send(dev, evt);
		if (ret == 0) {
			ret = its_test_wait_delivery(test_irq[dev][evt]);
		}
		if (ret != 0) {
			printf("its_test: interleaved dev %d event %d failed: %d\n", dev,
			    evt, ret);
			return ret;
		}
	}

	printf("its_test: %d interleaved INTs all delivered\n", total);

	return 0;
}

int main(int argc, char **argv) {
	int ret;

	if (argc > 1) {
		printf("Usage: its_test\n");
		printf("Exercises the ITS command queue and LPI delivery over a "
		       "%dx%d device/event grid\n",
		    ITS_TEST_DEV_CNT, ITS_TEST_EVT_CNT);
		return 0;
	}

	if (ITS_TEST_DEV_CNT * ITS_TEST_EVT_CNT > GIC_LPI_QUANTITY) {
		printf("its_test: %dx%d grid exceeds the %d LPI window\n",
		    ITS_TEST_DEV_CNT, ITS_TEST_EVT_CNT, GIC_LPI_QUANTITY);
		return -EINVAL;
	}

	printf("its_test: doorbell 0x%08llx, %dx%d device/event grid\n",
	    (unsigned long long)gic_its_trans_addr(), ITS_TEST_DEV_CNT,
	    ITS_TEST_EVT_CNT);

	memset(test_irq, 0xff, sizeof(test_irq));

	ret = its_test_map_grid();
	if (ret == 0) {
		ret = its_test_simple_int();
	}
	if (ret == 0) {
		ret = its_test_disable_enable();
	}
	if (ret == 0) {
		ret = its_test_grid_silent_then_refire();
	}
	if (ret == 0) {
		ret = its_test_interleaved_loop();
	}

	its_test_unmap_grid();

	if (ret != 0) {
		printf("its_test: FAIL (%d)\n", ret);
		return ret;
	}

	printf("its_test: PASS\n");

	return 0;
}
