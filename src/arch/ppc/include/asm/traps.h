/**
 * @file
 *
 * @date 08.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_TRAPS_H_
#define PPC_TRAPS_H_

#if 0
/**
 * Exceptions
 */
#define PPC_E_BASE       0x00000000                /* Exception vector base address */
#define PPC_E_RSV_0      (PPC_E_BASE + 0x00000000) /* Reserved */
#define PPC_E_SYS_RST    (PPC_E_BASE + 0x00000100) /* System reset */
#define PPC_E_M_CHK      (PPC_E_BASE + 0x00000200) /* Machine check */
#define PPC_E_DSI        (PPC_E_BASE + 0x00000300) /* DSI */
#define PPC_E_ISI        (PPC_E_BASE + 0x00000400) /* ISI */
#define PPC_E_EXT_INTR   (PPC_E_BASE + 0x00000500) /* External interrupt */
#define PPC_E_ALIGN      (PPC_E_BASE + 0x00000600) /* Alignment */
#define PPC_E_PROG       (PPC_E_BASE + 0x00000700) /* Program */
#define PPC_E_FP_UNAVAIL (PPC_E_BASE + 0x00000800) /* Floating-point unavailable */
#define PPC_E_DECR       (PPC_E_BASE + 0x00000900) /* Decrementer */
#define PPC_E_RSV_A      (PPC_E_BASE + 0x00000A00) /* Reserved */
#define PPC_E_RSV_B      (PPC_E_BASE + 0x00000B00) /* Reserved */
#define PPC_E_SYS_CALL   (PPC_E_BASE + 0x00000C00) /* System call */
#define PPC_E_TRACE      (PPC_E_BASE + 0x00000D00) /* Trace */
#define PPC_E_FP_ASSIST  (PPC_E_BASE + 0x00000E00) /* Floating-point asist */

#endif

#endif /* PPC_TRAPSS_H_ */
