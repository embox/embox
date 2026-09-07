/**
 * @file
 * @brief NVMe block driver.
 *
 * Binds to class 0108h endpoints found by the PCI bus scan and
 * exposes namespace 1 as a block device. With the msi_mode option set
 * the controller is asked for MSI-X (falling back to MSI) vectors and
 * I/O completions sleep on a wait queue woken by the interrupt
 * handler; admin commands and every path without a working message
 * interrupt stay polled.
 *
 * DMA coherency is maintained by hand: the queues and transfer
 * buffers live in static aligned BSS arrays (identity mapped, so a
 * DMA address equals the virtual address) and the driver flushes or
 * invalidates the dcache around every doorbell ring, the same way
 * the on-board GMAC driver does. Transfers are split into chunks of
 * at most two pages so that a command never needs a PRP list.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_regs.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <util/log.h>

#include "nvme.h"

#define NVME_CTRL_QUANTITY OPTION_GET(NUMBER, ctrl_quantity)
#define NVME_MSI_MODE      OPTION_GET(BOOLEAN, msi_mode)

#define NVME_PCI_SUBCLASS_EXPRESS 0x08

#define NVME_PAGE_SIZE 4096
#define NVME_ADMIN_QD  8
#define NVME_IO_QD     8
/* A command covers at most two pages: PRP1 up to the end of its
 * page plus one page-aligned PRP2, no PRP list needed. */
#define NVME_MAX_CHUNK (2 * NVME_PAGE_SIZE)

/* Poll loops are bounded busy waits, the pattern used by the other
 * on-board drivers. */
#define NVME_READY_TRIES 400000000u
#define NVME_CMD_TRIES   50000000u

/* Interrupt mode completion wait, in timer ticks; only hit when the
 * device stops completing commands. */
#define NVME_IRQ_TIMEOUT 1000

/* One completion vector drives the single I/O queue pair; the cap
 * keeps unclaimed LPIs in the ITS budget for other endpoints. */
#define NVME_MAX_VECTORS 2

#define NVME_DEFAULT_NSID 1

struct nvme_ctrl {
	struct pci_slot_dev *pdev;
	uintptr_t regs;
	uint32_t dbr_stride; /* byte stride of a doorbell register */
	uint32_t nsid;
	uint64_t ns_blocks;
	uint32_t lba_size;

	int irq_mode;
	unsigned int irq_num;
	struct waitq completion_wq;

	uint16_t admin_sq_tail;
	uint16_t admin_cq_head;
	uint8_t admin_cq_phase;

	uint16_t io_sq_tail;
	uint16_t io_cq_head;
	uint8_t io_cq_phase;

	char bdev_name[16];
	struct block_dev *bdev;
};

static struct nvme_ctrl nvme_ctrls[NVME_CTRL_QUANTITY];

/* DMA areas. Each array is page aligned so that queue entries and
 * PRP pages never share a cache line with unrelated data. */
static uint8_t nvme_admin_sq[NVME_ADMIN_QD * 64]
    __attribute__((aligned(NVME_PAGE_SIZE)));
static uint8_t nvme_admin_cq[NVME_ADMIN_QD * 16]
    __attribute__((aligned(NVME_PAGE_SIZE)));
static uint8_t nvme_io_sq[NVME_ADMIN_QD * 64]
    __attribute__((aligned(NVME_PAGE_SIZE)));
static uint8_t nvme_io_cq[NVME_ADMIN_QD * 16]
    __attribute__((aligned(NVME_PAGE_SIZE)));
static uint8_t nvme_ident_buf[NVME_PAGE_SIZE]
    __attribute__((aligned(NVME_PAGE_SIZE)));
static uint8_t nvme_bounce_buf[NVME_MAX_CHUNK]
    __attribute__((aligned(NVME_PAGE_SIZE)));

/* The kernel runs identity mapped and the board has no SMMU in the
 * PCIe path, so a DMA address is the virtual address itself. */
static uint64_t nvme_va2pa(const void *va) {
	return (uint64_t)(uintptr_t)va;
}

static int nvme_wait_ready(struct nvme_ctrl *c, int ready) {
	uint32_t tries = NVME_READY_TRIES;

	while (tries-- > 0) {
		uint32_t rdy = REG32_LOAD(c->regs + NVME_CSTS) & NVME_CSTS_RDY;

		if ((rdy != 0) == (ready != 0)) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}

static void nvme_ring_sq_doorbell(struct nvme_ctrl *c, int io, uint16_t tail) {
	uint16_t qid = io ? 1 : 0;

	dsb(st);
	REG32_STORE(NVME_DBR_SQ(c->regs, c->dbr_stride, qid), tail);
}

static void nvme_ring_cq_doorbell(struct nvme_ctrl *c, int io, uint16_t head) {
	uint16_t qid = io ? 1 : 0;

	dmb(sy);
	REG32_STORE(NVME_DBR_CQ(c->regs, c->dbr_stride, qid), head);
}

static volatile struct nvme_completion *nvme_cq_entry(struct nvme_ctrl *c,
    int io, uint16_t head) {
	uint8_t *cq = io ? nvme_io_cq : nvme_admin_cq;

	return (volatile struct nvme_completion *)(cq + head * 16);
}

/* Submit one command on the given queue pair and wait for its
 * completion. Exactly one command is in flight at a time. */
static int nvme_submit(struct nvme_ctrl *c, int io, struct nvme_command *cmd,
    void *buf, size_t len) {
	uint8_t *sq = io ? nvme_io_sq : nvme_admin_sq;
	uint16_t *tail = io ? &c->io_sq_tail : &c->admin_sq_tail;
	uint16_t *head = io ? &c->io_cq_head : &c->admin_cq_head;
	uint8_t *phase = io ? &c->io_cq_phase : &c->admin_cq_phase;
	volatile struct nvme_completion *cqe;
	uint32_t tries;
	int ret = 0;

	cmd->cid = 0;

	memcpy(sq + *tail * 64, cmd, sizeof(*cmd));
	dcache_flush(sq + *tail * 64, 64);
	if (buf != NULL) {
		if (cmd->opcode == NVME_OPC_IO_WRITE) {
			dcache_flush(buf, len);
		}
		else {
			/* The endpoint owns the buffer until the
			 * completion: drop the stale lines before it
			 * DMAs and again before reading the data. */
			dcache_inval(buf, len);
		}
	}
	dsb(st);

	/* The doorbell announces the tail one past the entry just
	 * written: advance first, then ring. */
	*tail = (*tail + 1) % (io ? NVME_IO_QD : NVME_ADMIN_QD);
	nvme_ring_sq_doorbell(c, io, *tail);

	cqe = nvme_cq_entry(c, io, *head);
	if (io && c->irq_mode) {
		/* Fast path: the device usually completes within the
		 * microseconds before this thread gets to sleep. */
		dcache_inval((const void *)cqe, sizeof(*cqe));
		if (NVME_CQE_PHASE(cqe) != *phase) {
			struct waitq_link *wql = &thread_self()->schedee.waitq_link;

			waitq_link_init(wql);
			for (;;) {
				waitq_wait_prepare(&c->completion_wq, wql);
				dcache_inval((const void *)cqe, sizeof(*cqe));
				if (NVME_CQE_PHASE(cqe) == *phase) {
					break;
				}
				ret = sched_wait_timeout(NVME_IRQ_TIMEOUT, NULL);
				if (ret != 0) {
					waitq_wait_cleanup(&c->completion_wq, wql);
					log_error("nvme: completion timed out"
					          " (opcode %#x, irq %u)",
					    cmd->opcode, c->irq_num);
					return -ETIMEDOUT;
				}
			}
			waitq_wait_cleanup(&c->completion_wq, wql);
		}

		/* The interrupt handler only wakes the waiter; the submitter
		 * advances the completion queue and releases the slot back to
		 * the device. */
		*head = (*head + 1) % (io ? NVME_IO_QD : NVME_ADMIN_QD);
		if (*head == 0) {
			/* The phase tag inverts on every pass of the ring */
			*phase ^= 1;
		}
		nvme_ring_cq_doorbell(c, io, *head);
	}
	else {
		for (tries = 0; tries < NVME_CMD_TRIES; tries++) {
			dcache_inval((const void *)cqe, sizeof(*cqe));
			if (NVME_CQE_PHASE(cqe) == *phase) {
				break;
			}
		}
		if (tries == NVME_CMD_TRIES) {
			log_error("nvme: command %#x timed out (phase %u head %u tail %u)",
			    cmd->opcode, *phase, *head, *tail);
			log_error("nvme: cqe %08x %08x %08x %08x", cqe->result,
			    cqe->reserved, ((uint32_t)cqe->sq_head << 16) | cqe->sq_id,
			    ((uint32_t)cqe->cid << 16) | cqe->status);
			log_error("nvme: csts %08x aqa %08x asq %08x%08x acq "
			          "%08x%08x stride %u",
			    REG32_LOAD(c->regs + NVME_CSTS), REG32_LOAD(c->regs + NVME_AQA),
			    REG32_LOAD(c->regs + NVME_ASQ + 4),
			    REG32_LOAD(c->regs + NVME_ASQ), REG32_LOAD(c->regs + NVME_ACQ + 4),
			    REG32_LOAD(c->regs + NVME_ACQ), c->dbr_stride);
			return -ETIMEDOUT;
		}

		*head = (*head + 1) % (io ? NVME_IO_QD : NVME_ADMIN_QD);
		if (*head == 0) {
			/* The phase tag inverts on every pass of the ring */
			*phase ^= 1;
		}
		nvme_ring_cq_doorbell(c, io, *head);
	}

	if (!NVME_CQE_OK(cqe)) {
		log_error("nvme: command %#x failed, status %#x", cmd->opcode, cqe->status);
	}

	if (buf != NULL && cmd->opcode != NVME_OPC_IO_WRITE) {
		dcache_inval(buf, len);
	}

	return NVME_CQE_OK(cqe) ? 0 : -EIO;
}

static int nvme_admin(struct nvme_ctrl *c, struct nvme_command *cmd, void *buf,
    size_t len) {
	return nvme_submit(c, 0, cmd, buf, len);
}

static int nvme_create_io_queue(struct nvme_ctrl *c, uint8_t opcode,
    uint32_t cdw11) {
	struct nvme_command cmd = {0};
	uint8_t *ring = (opcode == NVME_OPC_CREATE_CQ) ? nvme_io_cq : nvme_io_sq;

	cmd.opcode = opcode;
	cmd.prp1 = nvme_va2pa(ring);
	cmd.cdw10 = ((NVME_IO_QD - 1) << 16) | 1; /* size | qid 1 */
	cmd.cdw11 = cdw11;

	return nvme_admin(c, &cmd, NULL, 0);
}

/* Enable the controller and bring up the admin queue pair. The
 * CC.EN=0 transition resets controller state, so message interrupt
 * resources must only be armed after this has completed -- the
 * proven bare-metal setup arms MSI-X strictly after the controller
 * reports ready. */
static int nvme_ctrl_enable(struct nvme_ctrl *c) {
	uint64_t cap = REG64_LOAD(c->regs + NVME_CAP);
	uint32_t cc;
	int ret;

	if (NVME_CAP_MQES(cap) < NVME_ADMIN_QD - 1) {
		log_error("nvme: MQES %d too small", NVME_CAP_MQES(cap));
		return -EINVAL;
	}
	c->dbr_stride = 4u << NVME_CAP_DSTRD(cap);

	/* Disable and wait for not-ready */
	REG32_STORE(c->regs + NVME_CC, 0);
	ret = nvme_wait_ready(c, 0);
	if (ret != 0) {
		log_error("nvme: controller did not leave ready state");
		return ret;
	}

	REG32_STORE(c->regs + NVME_AQA,
	    ((NVME_ADMIN_QD - 1) << 16) | (NVME_ADMIN_QD - 1));
	REG64_STORE(c->regs + NVME_ASQ, nvme_va2pa(nvme_admin_sq));
	REG64_STORE(c->regs + NVME_ACQ, nvme_va2pa(nvme_admin_cq));

	c->admin_sq_tail = 0;
	c->admin_cq_head = 0;
	c->admin_cq_phase = 1; /* entries start with a zero phase tag */
	c->io_sq_tail = 0;
	c->io_cq_head = 0;

	cc = NVME_CC_EN | NVME_CC_CSS_NVM | NVME_CC_MPS(NVME_CAP_MPSMIN(cap))
	     | NVME_CC_IOSQES6 | NVME_CC_IOCQES4;
	REG32_STORE(c->regs + NVME_CC, cc);

	ret = nvme_wait_ready(c, 1);
	if (ret != 0) {
		log_error("nvme: controller enable timed out");
		return ret;
	}

	return 0;
}

/* Create the I/O queue pair; called after the message interrupt
 * resources are armed so the CQ is born with interrupts enabled. */
static int nvme_io_queues_setup(struct nvme_ctrl *c) {
	/* Interrupts on the I/O completion queue only; the admin queue
	 * stays polled, it completes before I/O ever starts. */
	int ret = nvme_create_io_queue(c, NVME_OPC_CREATE_CQ,
	    c->irq_mode ? 0x3 /* PC, IEN=1 */ : 0x1 /* PC, IEN=0 */);
	if (ret != 0) {
		return ret;
	}
	ret = nvme_create_io_queue(c, NVME_OPC_CREATE_SQ,
	    0x10003 /* PC, medium prio, cqid 1 */);
	if (ret != 0) {
		return ret;
	}
	c->io_cq_phase = 1;

	return 0;
}

static int nvme_identify_namespace(struct nvme_ctrl *c) {
	struct nvme_command cmd = {0};
	uint8_t *id = nvme_ident_buf;
	uint64_t nsze;
	uint8_t lbaf;
	uint8_t lbads;

	cmd.opcode = NVME_OPC_IDENTIFY;
	cmd.nsid = c->nsid;
	cmd.prp1 = nvme_va2pa(id);
	cmd.cdw10 = 0; /* CNS 0: identify namespace */

	dcache_inval(id, NVME_PAGE_SIZE);
	if (nvme_admin(c, &cmd, id, NVME_PAGE_SIZE) != 0) {
		return -EIO;
	}

	memcpy(&nsze, id + NVME_ID_NS_NSZE_OFF, sizeof(nsze));
	lbaf = id[NVME_ID_NS_LBAF_OFF] & 0xf;
	lbads = id[NVME_ID_NS_LBAFS_OFF + 4 * lbaf + 2];
	if (lbads < 9 || lbads > 31) {
		log_error("nvme: bad LBADS %u", lbads);
		return -EIO;
	}
	c->ns_blocks = nsze;
	c->lba_size = 1u << lbads;

	return 0;
}

static int nvme_prp_fit(const void *buf, size_t len) {
	uintptr_t pa = (uintptr_t)buf;
	size_t first = NVME_PAGE_SIZE - (pa & (NVME_PAGE_SIZE - 1));

	return len <= first + NVME_PAGE_SIZE;
}

/* Single NVMe read/write command of at most NVME_MAX_CHUNK bytes.
 * Falls back to the bounce buffer when the caller buffer cannot be
 * described by PRP1/PRP2 or is not a whole number of blocks. */
static int nvme_rw_chunk(struct nvme_ctrl *c, uint8_t opcode, void *buf,
    size_t len, uint64_t lba) {
	struct nvme_command cmd = {0};
	void *xfer = buf;
	size_t xfer_len = (len + c->lba_size - 1) & ~((size_t)c->lba_size - 1);
	int bounced = 0;
	int ret;

	if (xfer_len > NVME_MAX_CHUNK) {
		return -EINVAL;
	}
	if ((len & (c->lba_size - 1)) != 0 || ((uintptr_t)buf & 3) != 0
	    || !nvme_prp_fit(buf, xfer_len)) {
		xfer = nvme_bounce_buf;
		bounced = 1;
		if (opcode == NVME_OPC_IO_WRITE) {
			memcpy(xfer, buf, len);
			/* The padded tail of the last block must not
			 * leak stale bounce buffer content to disk. */
			memset((char *)xfer + len, 0, xfer_len - len);
		}
	}

	cmd.opcode = opcode;
	cmd.nsid = c->nsid;
	cmd.prp1 = nvme_va2pa(xfer);
	if (xfer_len > NVME_PAGE_SIZE - ((uintptr_t)xfer & (NVME_PAGE_SIZE - 1))) {
		cmd.prp2 = nvme_va2pa(
		    (void *)(((uintptr_t)xfer + NVME_PAGE_SIZE) & ~(NVME_PAGE_SIZE - 1)));
	}
	cmd.cdw10 = (uint32_t)lba;
	cmd.cdw11 = (uint32_t)(lba >> 32);
	cmd.cdw12 = xfer_len / c->lba_size - 1; /* NLB */

	ret = nvme_submit(c, 1, &cmd, xfer, xfer_len);

	if (bounced && ret == 0 && opcode == NVME_OPC_IO_READ) {
		memcpy(buf, xfer, len);
	}

	return ret;
}

static int nvme_bdev_read(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct nvme_ctrl *c = block_dev_priv(bdev);
	size_t done = 0;
	int ret = 0;

	while (done < count) {
		size_t chunk = count - done;
		uint64_t lba = (uint64_t)blkno + done / c->lba_size;

		if (chunk > NVME_MAX_CHUNK) {
			chunk = NVME_MAX_CHUNK;
		}
		ret = nvme_rw_chunk(c, NVME_OPC_IO_READ, buffer + done, chunk, lba);
		if (ret != 0) {
			return ret;
		}
		done += chunk;
	}

	return done;
}

static int nvme_bdev_write(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct nvme_ctrl *c = block_dev_priv(bdev);
	size_t done = 0;
	int ret = 0;

	while (done < count) {
		size_t chunk = count - done;
		uint64_t lba = (uint64_t)blkno + done / c->lba_size;

		if (chunk > NVME_MAX_CHUNK) {
			chunk = NVME_MAX_CHUNK;
		}
		ret = nvme_rw_chunk(c, NVME_OPC_IO_WRITE, buffer + done, chunk, lba);
		if (ret != 0) {
			return ret;
		}
		done += chunk;
	}

	return done;
}

static int nvme_bdev_ioctl(struct block_dev *bdev, int cmd, void *args,
    size_t size) {
	struct nvme_ctrl *c = block_dev_priv(bdev);

	switch (cmd) {
	case IOCTL_GETBLKSIZE:
		return c->lba_size;
	case IOCTL_GETDEVSIZE:
		return c->ns_blocks;
	default:
		return -ENOSYS;
	}
}

static const struct block_dev_ops nvme_bdev_ops = {
    .bdo_ioctl = nvme_bdev_ioctl,
    .bdo_read = nvme_bdev_read,
    .bdo_write = nvme_bdev_write,
};

static irq_return_t nvme_irq_handler(unsigned int irq_nr, void *data) {
	struct nvme_ctrl *c = data;

	/* The submitter owns the completion queue bookkeeping: it consumes
	 * the entry and rings the CQ doorbell. This handler only releases
	 * the waiter. The device posts the completion into the CQ before
	 * it raises the message interrupt, so the entry is already visible
	 * in memory by the time this runs. */
	waitq_wakeup(&c->completion_wq, 1);

	return IRQ_HANDLED;
}

/* Ask the PCI layer for message vectors. A single vector drives the
 * I/O completion queue; failure keeps the driver in polled mode. */
static void nvme_setup_irqs(struct nvme_ctrl *c) {
	int nvec;

	if (!NVME_MSI_MODE) {
		return;
	}

	nvec = pci_alloc_irq_vectors(c->pdev, 1, NVME_MAX_VECTORS,
	    PCI_IRQ_MSIX | PCI_IRQ_MSI);
	if (nvec < 1) {
		log_info("nvme: no message vectors available, staying polled");
		return;
	}

	c->irq_num = pci_irq_vector(c->pdev, 0);
	if (irq_attach(c->irq_num, nvme_irq_handler, 0, c, "nvme") != 0) {
		log_error("nvme: irq_attach failed, staying polled");
		return;
	}

	waitq_init(&c->completion_wq);
	c->irq_mode = 1;
	log_info("nvme: %d message vector(s), completion irq %u", nvec, c->irq_num);
}

static int nvme_probe_one(struct nvme_ctrl *c, struct pci_slot_dev *pdev) {
	uint16_t pci_cmd;
	static int idx;

	memset(c, 0, sizeof(*c));
	c->pdev = pdev;
	c->nsid = NVME_DEFAULT_NSID;

	if (pdev->bar[0] == 0 || pdev->bar[0] == 0xffffffff) {
		log_error("nvme: %04x:%04x has no BAR0", pdev->vendor, pdev->device);
		return -ENODEV;
	}
	c->regs = pci_resource_start(pdev, 0);

	/* Enable memory space decoding and bus mastering */
	pci_read_config_word(pdev, PCI_COMMAND, &pci_cmd);
	pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config_word(pdev, PCI_COMMAND, pci_cmd);

	/* Enable the controller first: arming message interrupts while
	 * the controller is disabled lets the CC.EN reset tear down the
	 * freshly programmed endpoint state behind our back. The proven
	 * bare-metal setup arms MSI-X strictly after CSTS.RDY=1. */
	if (nvme_ctrl_enable(c) != 0) {
		return -EIO;
	}
	nvme_setup_irqs(c);
	if (nvme_io_queues_setup(c) != 0) {
		return -EIO;
	}
	if (nvme_identify_namespace(c) != 0) {
		return -EIO;
	}

	snprintf(c->bdev_name, sizeof(c->bdev_name), "/dev/nvme%d", idx++);
	c->bdev = block_dev_create(c->bdev_name, &nvme_bdev_ops, c);
	if (c->bdev == NULL) {
		log_error("nvme: block_dev_create failed");
		return -ENODEV;
	}
	block_dev_set_block_size(c->bdev, c->lba_size);
	c->bdev->size = c->ns_blocks * c->lba_size;

	log_info("nvme%d: %04x:%04x at %p, ns%d: %llu blocks of %d bytes"
	         " (%llu MiB)",
	    idx - 1, pdev->vendor, pdev->device, (void *)c->regs, c->nsid, c->ns_blocks,
	    c->lba_size, (unsigned long long)c->ns_blocks * c->lba_size >> 20);

	return 0;
}

static int nvme_init(void) {
	struct pci_slot_dev *dev;
	int found = 0;

	pci_foreach_dev(dev) {
		if (found >= NVME_CTRL_QUANTITY) {
			break;
		}
		if (dev->baseclass != PCI_BASE_CLASS_STORAGE
		    || dev->subclass != NVME_PCI_SUBCLASS_EXPRESS) {
			continue;
		}
		if (nvme_probe_one(&nvme_ctrls[found], dev) == 0) {
			found++;
		}
	}

	return 0;
}

EMBOX_UNIT_INIT(nvme_init);
