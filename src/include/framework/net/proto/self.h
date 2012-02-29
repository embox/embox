/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PROTO_SELF_H_
#define FRAMEWORK_NET_PROTO_SELF_H_

#include __impl_x(framework/net/proto/self_impl.h)

/**
 * @brief Add new transport level protocol handler
 * @param _type        - type of protocol (e.g. UDP)
 * @param _handler     - handle pack (e.g. receive function)
 * @param _err_handler - errors handler
 * @param _init        - initialization function
 */
#define EMBOX_NET_PROTO_INIT(_type, _handler, _err_handler, _init)   \
	static int _handler(sk_buff_t *pack); 			     \
	static int _init(void);				 	     \
	static net_protocol_t _##_type = { 			     \
			.type = _type,				     \
			.handler = _handler,			     \
			.err_handler = _err_handler		     \
		};						     \
		__EMBOX_NET_PROTO(_##_type, _init)

/**
 * @see above EMBOX_NET_PROTO_INIT
 */
#define EMBOX_NET_PROTO(_type, _handler, _err_handler)		    \
	static int _handler(sk_buff_t *pack); 			    \
	static net_protocol_t _##_type = { 			    \
			.type = _type,				    \
			.handler = _handler,			    \
			.err_handler = _err_handler		    \
		};						    \
		__EMBOX_NET_PROTO(_##_type, NULL)


#endif /* FRAMEWORK_NET_PROTO_SELF_H_ */
