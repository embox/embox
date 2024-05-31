/**
 * @file
 * @brief
 *
 * @date 7.05.23
 * @author Aleksey Zhmulin
 */
#ifndef NETINET_TCP_H_
#define NETINET_TCP_H_

/* Options specific for tcp socket */
#define TCP_NODELAY 1 /* Don't delay send to coalesce packets  */
#define TCP_MAXSEG  2 /* Set maximum segment size  */
#define TCP_DEFER_ACCEPT	 9  /* Wake up listener only when data arrive */
#endif /* NETINET_TCP_H_ */
