/**
 * @file
 *
 * @data 08.02.2016
 * @author: Anton Bondarev
 */
#include <hal/reg.h>
#include <drivers/net/ti816x.h>
#include <util/log.h>

#include <framework/mod/options.h>
#if (OPTION_GET(NUMBER, speed) == 100)
void emac_mdio_config(void) {
}

#else

static void emac_mdelay(int value) {
	volatile int delay = value;

	while (delay--) ;

}

static void emac_mdio_writereg(unsigned char reg_num,unsigned short data)
{
	unsigned int tmp = 0,tmp1 = 0;

	tmp = (1<<31);
	log_debug("EMW 1 tmp=%x",tmp);
	tmp1 = 1<<30;
	log_debug("EMW 2 tmp1=%x",tmp1);
	tmp |= tmp1;
	tmp1 = ((reg_num&0x1f)<<21);
	log_debug("EMW 3 tmp1=%x",tmp1);
	tmp |= tmp1;
	tmp1 = ((0x4&0x1f)<<16);
	log_debug("EMW 4 tmp1=%x",tmp1);
	tmp |= tmp1;
	tmp1 = data&0xffff;
	log_debug("EMW 5 tmp1=%x",tmp1);
	tmp |= tmp1;
	log_debug("EMW tmp=%x",tmp);
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERACCESS0,tmp);
	tmp1 = 0;
	while (REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW) == 0)
	{
		tmp1++;
	}
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW,
		REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW));
	/*res=REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERACCESS0); */
}

static int emac_mdio_readreg(unsigned char reg_num)
{
	unsigned int res = 0;
	unsigned int tmp = 0,tmp1 = 0;

	tmp = (1<<31);
	log_debug("EMR 1 tmp=%x",tmp);
	tmp1 = ((reg_num&0x1f)<<21);
	log_debug("EMR 2 tmp1=%x",tmp1);
	tmp |= tmp1;
	tmp1 = ((0x4&0x1f)<<16);
	log_debug("EMR 3 tmp1=%x",tmp1);
	tmp |= tmp1;
	log_debug("EMR tmp=%x",tmp);
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERACCESS0,tmp);
	tmp1 = 0;
	while (REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW) == 0)
	{
		tmp1++;
	}
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW,
		REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERINTRAW));

	res = REG_LOAD(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERACCESS0);
	if (((res>>29)&0x1) == 0)
	{
		log_error("emac_mdio_readreg err - ACC not - res=%x",res);
		return 0;
	}
	return res&0xffff;
}

#define MAX_PAGESEL 31
#define MAX_IR 20
#define MAX_CR 19
#define MAX_GMIICR 18
#define MAX_PTPCR1 16
#define MAX_BMCR 0
#define MAX_DISSPI_SETPG2 0x12
#define MAX_DISSPI_SETPG1 0x11
#define MAX_DISSPI_SETPG0 0x10

void emac_mdio_config(void)
{
	unsigned int res = 0;

	log_debug("emac_mdio_config started");
	/*REG_ORIN(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_CONTROL,0x40000000); */
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_CONTROL,0x40000020);
	emac_mdelay(10000);
	REG_STORE(MDIO_BASE_ADDR+MDIO_OFFSET+MDIO_R_USERPHYSEL0,0x4);
	emac_mdio_writereg(MAX_CR,res);
	res = emac_mdio_readreg(MAX_GMIICR);
	log_debug("EMC res3=%x",res);
	res |= 0x8480;
	log_debug("EMC res4=%x",res);
	emac_mdio_writereg(MAX_GMIICR,res);
	log_debug("emac_mdio_config ended");
}
#endif
