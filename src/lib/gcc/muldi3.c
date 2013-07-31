/**
 * @file
 *
 * @brief
 *
 * @date 25.05.2012
 * @author Anton Bondarev
 */


#include <stdint.h>
//TODO endian (now only for big endian)

struct dw_struct {
	int32_t high;
	int32_t low;
};

union dw_union {
	struct dw_struct st;
	int64_t val;
};

#define WORD_LENGTH __WORDSIZE

#define highpart(v) (v >> (WORD_LENGTH / 2))
#define lowpart(v)  (v & ((1 << (WORD_LENGTH / 2)) - 1))

uint64_t __muldi3(int64_t u, int64_t v) {
	union dw_union res;
	union dw_union op1, op2;
	uint16_t hw_1l, hw_1h, hw_2l, hw_2h; /* half words */
	uint32_t hs_ll, hs_lh, hs_hl, hs_hh;

	op1.val = u;
	op2.val = v;


	hw_1l = lowpart(op1.st.low);
	hw_1h = highpart(op1.st.low);
	hw_2l = lowpart(op2.st.low);
	hw_2h = highpart(op2.st.low);

	hs_ll = hw_1l * hw_2l;
	hs_lh = hw_1l * hw_2h;
	hs_hl = hw_1h * hw_2l;
	hs_hh = hw_1h * hw_2h;

	hs_lh += highpart(hs_ll);
	hs_lh += hs_hl;
	if(hs_lh < hs_hl) {
		hs_hh += 1 << (WORD_LENGTH / 2);
	}
	res.st.high = hs_hh + highpart(hs_lh);
	res.st.low  = (lowpart(hs_lh) << (WORD_LENGTH / 2)) + lowpart(hs_ll);

	return res.val;
}
