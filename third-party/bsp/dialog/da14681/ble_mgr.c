/**
 * @file
 *
 * @date May 8, 2020
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <stdbool.h>

bool ble_mgr_skip_latency_get_from_isr(uint16_t conn_idx)
{
#if defined(dg_configBLE_SKIP_LATENCY_API) && (dg_configBLE_SKIP_LATENCY_API == 1)
        bool enabled;
        uint8_t mask = (1 << conn_idx);

        enabled = (skip_latency_mask & mask);

        return enabled;
#else
       return false;
#endif
}
