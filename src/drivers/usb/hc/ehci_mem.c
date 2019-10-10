/**
 * @file
 *
 * @date Aug 20, 2019
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <mem/misc/pool.h>

#include "ehci.h"

#define EHCI_MAX_REQUESTS 0x40

POOL_DEF(ehci_qtd_pool, struct ehci_qtd_hw, EHCI_MAX_REQUESTS);
POOL_DEF(ehci_qh_hw_pool, struct ehci_qh_hw, EHCI_MAX_REQUESTS);
POOL_DEF(ehci_itd_pool, struct ehci_itd, EHCI_MAX_REQUESTS);
POOL_DEF(ehci_sitd_pool, struct ehci_sitd, EHCI_MAX_REQUESTS);
POOL_DEF(ehci_qh_pool, struct ehci_qh, EHCI_MAX_REQUESTS);

static uint32_t periodic_table[1024] __attribute__((aligned(4096)));

static void ehci_qtd_init(struct ehci_hcd *ehci, struct ehci_qtd_hw *qtd, uintptr_t dma) {
	memset (qtd, 0, sizeof *qtd);
	qtd->hw_token = EHCI_QTD_STS_HALT;
	qtd->hw_next = EHCI_LIST_END(ehci);
	qtd->hw_alt_next = EHCI_LIST_END(ehci);
}

struct ehci_qtd_hw *ehci_qtd_alloc(struct ehci_hcd *ehci) {
	struct ehci_qtd_hw *qtd;

	qtd = pool_alloc(&ehci_qtd_pool);
	if (qtd != NULL) {
		ehci_qtd_init(ehci, qtd, (uintptr_t)qtd);
	}
	return qtd;
}

void ehci_qtd_free(struct ehci_hcd *ehci, struct ehci_qtd_hw *qtd) {
	pool_free(&ehci_qtd_pool, qtd);
}

struct ehci_qh *ehci_qh_alloc(struct ehci_hcd *ehci) {
	struct ehci_qh *qh;

	qh = pool_alloc(&ehci_qh_pool);
	if (!qh) {
		return NULL;
	}
	qh->hw = pool_alloc(&ehci_qh_hw_pool);
	if (!qh->hw) {
		return NULL;
	}
	memset(qh->hw, 0, sizeof(*qh->hw));
	qh->qh_dma = (uintptr_t)qh->hw;

	return qh;
}

void ehci_qh_free(struct ehci_hcd *ehci, struct ehci_qh *qh) {
	pool_free(&ehci_qh_hw_pool, qh->hw);
	pool_free(&ehci_qh_pool, qh);
}

int ehci_mem_init(struct ehci_hcd *ehci_hcd) {
	int i;

	ehci_hcd->qh_pool = &ehci_qh_pool;
	ehci_hcd->qtd_pool = &ehci_qtd_pool;
	ehci_hcd->itd_pool = &ehci_itd_pool;
	ehci_hcd->sitd_pool = &ehci_sitd_pool;

	ehci_hcd->async = ehci_qh_alloc(ehci_hcd);

	memset(periodic_table, 0, sizeof(periodic_table));
	ehci_hcd->periodic = periodic_table;
	ehci_hcd->periodic_dma = (uintptr_t)periodic_table;
	for (i = 0; i < sizeof(periodic_table)/sizeof(periodic_table[0]); i++) {
		ehci_hcd->periodic[i] = EHCI_LIST_END(ehci_hcd);
	}

	return 0;
}
