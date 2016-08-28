/**
 * @file platform_shutdown_dummy.c
 * @brief Simply jump on reset handler
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2016-06-18
 */

.extern bootldr_start
platform_shutdown:
	b bootldr_start
