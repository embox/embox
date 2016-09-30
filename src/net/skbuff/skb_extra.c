/**
 * @file
 * @details sk_buff management API implementation.
 * @date 20.10.09
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <assert.h>

#include <util/math.h>
#include <util/log.h>
#include <util/member.h>
#include <util/binalign.h>

#include <hal/ipl.h>

#include <mem/misc/pool.h>

#include <net/skbuff.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SKB_EXTRA OPTION_GET(NUMBER, amount_skb_extra)
#define MODOPS_EXTRA_SIZE       OPTION_GET(NUMBER, extra_size)
#define MODOPS_EXTRA_ALIGN      OPTION_GET(NUMBER, extra_align)
#define MODOPS_EXTRA_PADTO      OPTION_GET(NUMBER, extra_padto)

#define EXTRA_PAD_SIZE \
	PAD_SIZE(MODOPS_EXTRA_SIZE, MODOPS_EXTRA_PADTO)
#define EXTRA_ATTR \
	__attribute__((aligned(MODOPS_EXTRA_ALIGN)))

struct sk_buff_extra {
	unsigned char extra[MODOPS_EXTRA_SIZE];
	char __extra_pad[EXTRA_PAD_SIZE];
} EXTRA_ATTR;

POOL_DEF(skb_extra_pool, struct sk_buff_extra, MODOPS_AMOUNT_SKB_EXTRA);

size_t skb_extra_max_size(void) {
	return member_sizeof(struct sk_buff_extra, extra);
}

void * skb_extra_cast_in(struct sk_buff_extra *skb_extra) {
	assert(skb_extra != NULL);
	return &skb_extra->extra[0];
}

struct sk_buff_extra * skb_extra_cast_out(void *extra) {
	assert(extra != NULL);
	return member_cast_out(extra, struct sk_buff_extra, extra);
}

struct sk_buff_extra * skb_extra_alloc(void) {
	ipl_t sp;
	struct sk_buff_extra *skb_extra;

	sp = ipl_save();
	{
		skb_extra = pool_alloc(&skb_extra_pool);
	}
	ipl_restore(sp);

	if (skb_extra == NULL) {
		log_error("skb_extra_alloc: error: no memory\n");
		return NULL; /* error: no memory */
	}

	return skb_extra;
}

void skb_extra_free(struct sk_buff_extra *skb_extra) {
	ipl_t sp;

	sp = ipl_save();
	{
		pool_free(&skb_extra_pool, skb_extra);
	}
	ipl_restore(sp);
}
