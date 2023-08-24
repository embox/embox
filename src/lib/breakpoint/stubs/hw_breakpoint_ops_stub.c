/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <debug/breakpoint.h>

HW_BREAKPOINT_OPS_DEF({
    .set = NULL,
    .remove = NULL,
    .enable = NULL,
    .disable = NULL,
    .count = NULL,
});
