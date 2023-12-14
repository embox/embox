/**
 * @file cpuinfo.h
 * @brief This module helps to detect the revision and stuff i.MX6 CPU
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.05.2017
 */

#ifndef ARCH_ARM_IMX_CPUINFO_H_
#define ARCH_ARM_IMX_CPUINFO_H_

#define MXC_CPU_MX23		0x23
#define MXC_CPU_MX25		0x25
#define MXC_CPU_MX27		0x27
#define MXC_CPU_MX28		0x28
#define MXC_CPU_MX31		0x31
#define MXC_CPU_MX35		0x35
#define MXC_CPU_MX51		0x51
#define MXC_CPU_MX53		0x53
#define MXC_CPU_MX6SL		0x60
#define MXC_CPU_MX6DL		0x61
#define MXC_CPU_MX6SX		0x62
#define MXC_CPU_MX6Q		0x63
#define MXC_CPU_MX6UL		0x64
#define MXC_CPU_MX6ULL		0x65
#define MXC_CPU_MX6SOLO		0x66 /* dummy */
#define MXC_CPU_MX6SLL		0x67
#define MXC_CPU_MX6D		0x6A
#define MXC_CPU_MX6DP		0x68
#define MXC_CPU_MX6QP		0x69
#define MXC_CPU_MX7S		0x71 /* dummy ID */
#define MXC_CPU_MX7D		0x72
#define MXC_CPU_MX7ULP		0x81 /* Temporally hard code */
#define MXC_CPU_VF610		0xF6 /* dummy ID */

void print_imx_cpuinfo(void);

#endif /* ARCH_ARM_IMX_CPUINFO_H_ */
