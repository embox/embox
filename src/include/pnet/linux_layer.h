/**
 * @file
 * @brief
 *
 * @date 24.10.2011
 * @author Anton Kozlov
 */

#ifndef _PNET_LINUX_LAYER_H
#define _PNET_LINUX_LAYER_H

extern int pnet_netif_rx(struct sk_buff *skb);

#ifdef CONFIG_PNET_LINUX_LAYER
#define netif_rx pnet_netif_rx
#endif /* CONFIG_PNET_LINUX_LAYER */

#endif
