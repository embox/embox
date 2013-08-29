/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#ifndef WCHAR_T_DEF_H_
#define WCHAR_T_DEF_H_

#ifndef __cplusplus
#ifdef __WCHAR_TYPE__
 typedef __WCHAR_TYPE__ wchar_t;
#else
 typedef int wchar_t;
#endif /*__wchar_t_defined */

#endif /* __cplusplus */

#endif /* WCHAR_T_DEF_H_ */
