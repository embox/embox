/**
 * @file
 *
 * @data 17 нояб. 2015 г.
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_SAMBA_EXT_INCLUDE_SYS_UN_H_
#define THIRD_PARTY_SAMBA_EXT_INCLUDE_SYS_UN_H_

struct sockaddr_un {
	unsigned short sun_family;  /* AF_UNIX */
	char sun_path[108];
};

#endif /* THIRD_PARTY_SAMBA_EXT_INCLUDE_SYS_UN_H_ */
