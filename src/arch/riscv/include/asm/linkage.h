/**
 * @file
 *
 * @date 22.08.2024
 * @author: Zeng Zixian
 */

#ifndef LINKAGE_H_
#define LINKAGE_H_


#define C_ENTRY(name) .globl name; .align 4; name

#define C_LABEL(name) .globl name; name


#endif /* LINKAGE_H_ */
