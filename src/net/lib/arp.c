/**
 * @file
 * @brief
 *
 * @date 19.01.14
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l3/arp.h>
#include <net/lib/arp.h>
#include <string.h>

void arp_build(struct arphdr *arph, uint16_t hrd, uint16_t pro,
		uint8_t hln, uint8_t pln, uint16_t op, const void *sha,
		const void *spa, const void *tha, const void *tpa) {
	struct arpbody arpb;

	assert(arph != NULL);
	arph->ar_hrd = htons(hrd);
	arph->ar_pro = htons(pro);
	arph->ar_hln = hln;
	arph->ar_pln = pln;
	arph->ar_op = htons(op);

	arp_make_body(arph, &arpb);
	if (sha != NULL) {
		memcpy(arpb.ar_sha, sha, hln);
	}
	else {
		memset(arpb.ar_sha, 0, hln);
	}
	if (spa != NULL) {
		memcpy(arpb.ar_spa, spa, pln);
	}
	else {
		memset(arpb.ar_spa, 0, pln);
	}
	if (tha != NULL) {
		memcpy(arpb.ar_tha, tha, hln);
	}
	else {
		memset(arpb.ar_tha, 0, hln);
	}
	if (tpa != NULL) {
		memcpy(arpb.ar_tpa, tpa, pln);
	}
	else {
		memset(arpb.ar_tpa, 0, pln);
	}
}

void arp_make_body(struct arphdr *arph,
		struct arpbody *out_arpb) {
	assert(arph != NULL);
	assert(out_arpb != NULL);
	out_arpb->ar_sha = &arph->ar_body[0];
	out_arpb->ar_spa = out_arpb->ar_sha + arph->ar_hln;
	out_arpb->ar_tha = out_arpb->ar_spa + arph->ar_pln;
	out_arpb->ar_tpa = out_arpb->ar_tha + arph->ar_hln;
}
