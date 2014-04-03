/**
 * @file
 * @brief Encrypting packet API
 *
 * @date 22.01.14
 * @author Ilia Vaprol
 */

#ifndef NET_L0_NET_CRYPT_
#define NET_L0_NET_CRYPT_

extern struct sk_buff * net_encrypt(struct sk_buff *skb);
extern struct sk_buff * net_decrypt(struct sk_buff *skb);

#endif /* NET_L0_NET_CRYPT_ */
