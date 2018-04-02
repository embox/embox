/**
 * @file
 *
 * @date Mar 29, 2018
 * @author Anton Bondarev
 */


#include "lynxfb.h"
#include "lynxfb_hw750.h"

#include "ddk750_chip.h"
void volatile *mmio750;

int lynxfb_hw750_map(struct sm750_share *share, struct pci_dev *pdev) {
	return 0;
}

int lynxfb_hw750_inithw(struct sm750_share *share, struct pci_dev *pdev) {
	return 0;
}

/* 	chip specific g_option configuration routine */
void lynxfb_hw750_setup(struct sm750_share *share) {

}

int lynxfb_hw750_set_drv(struct lynxfb_par *par) {
	return 0;
}
