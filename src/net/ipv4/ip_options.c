/*
 * @file
 * @brief IP options processing module
 *
 * @date 14.04.2010
 * @author Roman Kolobov
 */

#include <string.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/inetdevice.h>
#include <net/inet_sock.h>

int ip_options_compile(sk_buff_t *skb, ip_options_t *opt) {

	unsigned char* endopts = opt->__data + opt->optlen - sizeof(struct ip_options);
	iphdr_t *iph = ip_hdr(skb);
	/* curropt points to current option in question
	 * optsfault points to first problem occurred in options
	 */
 	unsigned char *curropt = (unsigned char*) iph + IP_MIN_HEADER_SIZE;
	int curroptlen;
	unsigned char *optsfault = NULL;
	_Bool secappeared = false;
	_Bool sidappeared = false;
	unsigned int* timestamp = NULL;

	for (curropt = opt->__data; curropt < endopts; ) {

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
		if ((curroptlen < 2) || (curroptlen >= (endopts - curropt))) {
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
					memcpy(
						&curropt[curropt[2]-1],
						(void*)(in_dev_get(skb->dev)->ifa_address),
						4);
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
					switch(curropt[3]&0x0F)
					{
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
							memcpy(
								&curropt[curropt[2]-1],
								(void*)((in_dev_get(skb->dev))->ifa_address),
								4);
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
			// TODO case IPOPT_RA
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
	icmp_send(skb, ICMP_PARAMETERPROB, 0, htonl(curroptlen << 24));
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
