/**
 * @file sock.c
 *
 * @details Generic socket support routines.
 * @date 12.08.09
 * @author Nikolay Korotky
 */
#include <string.h>
#include <common.h>
#include <kernel/module.h>
#include <kernel/irq.h>
#include <net/skbuff.h>
#include <net/sock.h>

typedef struct sock_info {
        struct sock sk;
        struct list_head list __attribute__ ((aligned (4)));
} sock_info_t __attribute__ ((aligned (4)));

static sock_info_t socks_pull[MAX_KERNEL_SOCKETS];
static LIST_HEAD(head_free_sock);

void __init sk_init(void) {
        int i;
        for (i = 0; i < array_len(socks_pull); i++) {
                list_add(&(&socks_pull[i])->list, &head_free_sock);
        }
}

static struct sock *sk_prot_alloc(struct proto *prot, gfp_t priority,
                int family) {
        struct sock *sock;
        struct list_head *entry;

        unsigned long irq_old = local_irq_save();

        if (list_empty(&head_free_sock)) {
                local_irq_restore(irq_old);
                return NULL;
        }
        entry = (&head_free_sock)->next;
        list_del_init(entry);
        sock = (struct sock *) list_entry(entry, sock_info_t, list);

        local_irq_restore(irq_old);

        return sock;
}

static void sk_prot_free(struct proto *prot, struct sock *sk) {
        sock_info_t *sock_info;
        unsigned long irq_old;
        if (NULL == sk) {
                return;
        }

        irq_old = local_irq_save();
        sock_info = (sock_info_t *) sk;
        list_add(&sock_info->list, &head_free_sock);
        local_irq_restore(irq_old);
}

struct sock *sk_alloc(/*struct net *net,*/ int family, gfp_t priority,
                      struct proto *prot) {
        struct sock *sk;
        sk = sk_prot_alloc(prot, 0, family);
        if(sk) {
    		sk->sk_family = family;
    		sk->sk_prot = prot;
	}
	return sk;
}

void sk_free(struct sock *sk) {
	if (sk->sk_destruct) {
                sk->sk_destruct(sk);
        }
        sk_prot_free(sk->sk_prot, sk);
}

int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	skb_queue_tail(&sk->sk_receive_queue, skb);
        return 0;
}
