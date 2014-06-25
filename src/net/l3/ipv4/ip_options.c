/*
 * @file
 * @brief IP options processing module
 *
 * @date 14.04.10
 * @author Roman Kolobov
 */

#include <net/l3/ipv4/ip_options.h>
#include <net/skbuff.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/icmpv4.h>
#include <net/inetdevice.h>
#include <net/socket/inet_sock.h>

int ip_options_compile(sk_buff_t *skb, ip_options_t *opt) {
	struct iphdr *iph;

	uint8_t * endopts;

	/* curropt points to current option in question
	 * optsfault points to first problem occurred in options
	 */
	uint8_t *curropt;
	int curroptlen;

	uint8_t *optsfault = NULL;
	_Bool secappeared = false;
	_Bool sidappeared = false;
	unsigned int* timestamp = NULL;

	iph = ip_hdr(skb);
	curropt	= (unsigned char*) iph + IP_MIN_HEADER_SIZE;
	endopts = curropt + opt->optlen;

	for (; curropt < endopts; ) {
		switch (*curropt) {
		case IPOPT_END:
			for (++curropt ; curropt < endopts; ++curropt) {
				if (IPOPT_END != *curropt) {
					*curropt = IPOPT_END;
					opt->is_changed = 1;
				}
			}
			goto end;
		case IPOPT_NOOP:
			continue;
		}

		curroptlen = curropt[1];
		if ((curroptlen < 2) || (curroptlen > (endopts - curropt))) {
			optsfault = curropt + 1;
			goto error;
		}

		switch(*curropt) {
		case IPOPT_SEC:
			if (secappeared) {
				optsfault = curropt;
				goto error;
			}
			if (curroptlen != 11) {
				optsfault = curropt + 1;
				goto error;
			}
			secappeared = true;
			memcpy(&opt->__data, curropt, 12);
			break;
		case IPOPT_LSRR:
		case IPOPT_SSRR:
			if (opt->srr) {
				optsfault = curropt;
				goto error;
			}
			if (curroptlen < 3) {
				optsfault = curropt + 1;
				goto error;
			}
			if (curropt[2] < 4) {
				optsfault = curropt + 2;
				goto error;
			}
			opt->srr = (unsigned char) (curropt - (unsigned char*) iph);
			opt->is_strictroute = (*curropt == IPOPT_SSRR);
			break;
		case IPOPT_RR:
			if (opt->rr) {
				optsfault = curropt;
				goto error;
			}
			if (curroptlen < 3) {
				optsfault = curropt + 1;
				goto error;
			}
			if (curropt[2] < 4) {
				optsfault = curropt + 2;
				goto error;
			}
			if (curropt[2] <= curroptlen) {
				if (curropt[2] + 3 > curroptlen) {
					optsfault = curropt + 2;
					goto error;
				}
				//TODO ask whether ...->ifa_address is a correct transmitter (i.e. our) address
				*(in_addr_t *)(curropt + curropt[2] - 1) = inetdev_get_by_dev(skb->dev)->ifa_address;
				curropt[2] += 4;
				opt->is_changed = 1;
				opt->rr_needaddr = 1;
			}
			opt->rr = (unsigned char) (curropt - (unsigned char*) iph);
			break;
		case IPOPT_SID:
			if (sidappeared) {
				optsfault = curropt;
				goto error;
			}
			if (curropt[1] != 4) {
				optsfault = curropt + 1;
				goto error;
			}
			sidappeared = true;
		case IPOPT_TIMESTAMP:
			if (opt->ts) {
				optsfault = curropt;
				goto error;
			}
			if (curroptlen < 4) {
				optsfault = curropt + 1;
				goto error;
			}
			if (curropt[2] < 5) {
				optsfault = curropt + 2;
				goto error;
			}
			if (curropt[2] <= curroptlen) {
				if (curropt[2] + 3 > curroptlen) {
					optsfault = curropt + 2;
					goto error;
				}
				switch(curropt[3]&0x0F) {
				case IPOPT_TS_TSONLY:
					opt->ts = (unsigned char) (curropt - (unsigned char*) iph);
					opt->ts_needtime = 1;
					timestamp = (unsigned int*)(&curropt[curropt[2]-1]);
					curropt[2] += 4;
					break;
				case IPOPT_TS_TSANDADDR:
					if (curropt[2] + 7 > curroptlen)
					{
						optsfault = curropt + 2;
						goto error;
					}
					*(in_addr_t *)(curropt + curropt[2] - 1) = inetdev_get_by_dev(skb->dev)->ifa_address;
					timestamp = (unsigned int*)(&curropt[curropt[2]+3]);
					opt->ts = (unsigned char) (curropt - (unsigned char*) iph);
					opt->ts_needaddr = 1;
					opt->ts_needtime = 1;
					curropt[2] += 8;
					break;
				case IPOPT_TS_PRESPEC:
					if (curropt[2] + 7 > curroptlen)
					{
						optsfault = curropt + 2;
						goto error;
					}
					opt->ts = (unsigned char) (curropt - (unsigned char*) iph);
					//TODO i still don't understand meaning of this option
					break;
				default:
					optsfault = curropt + 3;
					goto error;
				}
				if (timestamp) {
					//TODO get timestamp and record, gettimeofday seems to be unimplemented
					opt->is_changed = 1;
				}
			}
			else {
				unsigned short tsoverflow = curropt[3] >> 4;

				if (0xFF == tsoverflow) {
					optsfault = curropt + 3;
					goto error;
				}
				curropt[3] &= 0x0F;
				curropt[3] |= (++tsoverflow) << 4;
				opt->is_changed = 1;
				opt->ts = (unsigned char) (curropt - (unsigned char*) iph);
			}
			break;
		// TODO case IPOPT_CIPSO - don't still know what to do here
		case IPOPT_RA:
			opt->router_alert = curropt[3];
			break;
		default:
			optsfault = curropt;
			goto error;
		}

		curropt += curroptlen;

	}

end:
	if (!optsfault)
	{
		return 0;
	}

error:
	curroptlen = (int) (optsfault - (unsigned char*) iph);
	icmp_discard(skb, ICMP_PARAM_PROB, ICMP_PTR_ERROR,
			(uint8_t)curroptlen);
	//TODO : is it an adequate return value? maybe -EINVAL will suit better?
	return -1;

}

int ip_options_handle_srr(sk_buff_t *skb)
{
	ip_options_t *opt = (ip_options_t*)(skb->cb);
//	iphdr_t *iph = ip_hdr(skb);

	if (!opt->srr) {
		return 0;
	}

	//TODO search for addresses consequently, try to route

	return 0;
}

uint16_t skb_get_secure_level(struct sk_buff *skb) {
	uint16_t level;
	ip_options_t *opt;

	opt = (ip_options_t *)skb->cb;

	memcpy(&level, &opt->__data + 2, sizeof(level));

	return ntohs(level);
}
