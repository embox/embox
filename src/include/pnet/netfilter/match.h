/**
 * @file
 * @brief
 *
 * @date 13.03.12
 * @author Alexander Kalmuk
 */

#ifndef PNET_MATCH_H_
#define PNET_MATCH_H_

#include <pnet/node.h>

extern net_node_t matcher_alloc(void);
extern int matcher_free(net_node_t node);

#endif /* PNET_MATCH_H_ */
