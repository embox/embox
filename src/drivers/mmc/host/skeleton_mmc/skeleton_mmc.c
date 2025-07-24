/**
 * @file
 * @brief 
 * @author Anton Bondarev
 * @version 0.1
 * @date 10.07.2025
 */

#include <util/log.h>

#include <assert.h>

//#include <hal/reg.h>

//#include <drivers/common/memory.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#include <embox/unit.h>
#include <framework/mod/options.h>


EMBOX_UNIT_INIT(skeleton_mmc_init);

static int skeleton_mmc_send_cmd(int cmd, int arg, int resp, int longresp) {

	log_debug("send cmd (%d) arg (%x) resp (%d) longresp (%d)", cmd, arg, resp, longresp);

	return 0;
}

static void skeleton_mmc_mmc_request(struct mmc_host *host, struct mmc_request *req) {
	uint32_t *buff;

	assert(host);
	assert(req);

	skeleton_mmc_send_cmd(req->cmd.opcode,
			req->cmd.arg,
			req->cmd.flags & MMC_RSP_PRESENT,
			req->cmd.flags & MMC_RSP_136);
#if 0
	req->cmd.resp[0] = REG32_LOAD(MCI_RESPONSE0);
	req->cmd.resp[1] = REG32_LOAD(MCI_RESPONSE1);
	req->cmd.resp[2] = REG32_LOAD(MCI_RESPONSE2);
	req->cmd.resp[3] = REG32_LOAD(MCI_RESPONSE3);
#endif
	if (req->cmd.flags & MMC_DATA_XFER) {
		int xfer = req->data.blksz * req->data.blocks;
	
		log_debug("Set datalen %d", req->data.blksz);

		buff = (void *) req->data.addr;
		log_error("buff=%p", buff);

		while (xfer > 0) {
			if (req->cmd.flags & MMC_DATA_READ) {
				log_debug("reading  %d byte", req->data.blksz);
			} else {
				log_debug("writting  %d byte", req->data.blksz);
			}
		}
	}
}

const struct mmc_host_ops skeleton_mmc_mmc_ops = {
	.request = skeleton_mmc_mmc_request,
};



static int skeleton_mmc_init(void) {
	/* It seems that  we can't check directly
	 * if any card present, so we just assume
	 * that we have a single card number zero */
	struct mmc_host *mmc = mmc_alloc_host();
	mmc->ops = &skeleton_mmc_mmc_ops;

	mmc_scan(mmc);

	return 0;
}

