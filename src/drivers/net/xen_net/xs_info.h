/**
 * @file 
 * @brief Shows information from XenStore
 *
 * @date 
 * @author 
 */

#ifndef XS_INFO_H_
#define XS_INFO_H_

#define XS_KEY_LEN 		256
#define XS_VALUE_LEN 	512
#define XS_NAME_LEN 	16
#define XS_DOMID_LEN 	4

#define XS_LS_N_REC 	0	/* List info only about current directory */
#define XS_LS_REC 		1	/* List info about current directory
							 * and all its subdirectories */

extern void xenstore_info();

#endif /* XS_INFO_H_ */
