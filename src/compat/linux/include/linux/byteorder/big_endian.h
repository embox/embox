/**
 * @file
 * @brief
 *
 * @date 24.09.2013
 * @author Andrey Gazukin
 */

#ifndef BIG_ENDIAN_H_
#define BIG_ENDIAN_H_

#ifndef __BIG_ENDIAN_BITFIELD
#define __BIG_ENDIAN_BITFIELD
#endif

#include <swab.h>
#include <linux/types.h>

#define __constant_htonl(x) (( __be32)(__u32)(x))
#define __constant_ntohl(x) (( __u32)(__be32)(x))
#define __constant_htons(x) (( __be16)(__u16)(x))
#define __constant_ntohs(x) (( __u16)(__be16)(x))
#define __constant_cpu_to_le64(x) (( __le64)swab64((x)))
#define __constant_le64_to_cpu(x) swab64(( __u64)(__le64)(x))
#define __constant_cpu_to_le32(x) (( __le32)swab32((x)))
#define __constant_le32_to_cpu(x) swab32(( __u32)(__le32)(x))
#define __constant_cpu_to_le16(x) (( __le16)swab16((x)))
#define __constant_le16_to_cpu(x) swab16(( __u16)(__le16)(x))
#define __constant_cpu_to_be64(x) (( __be64)(__u64)(x))
#define __constant_be64_to_cpu(x) (( __u64)(__be64)(x))
#define __constant_cpu_to_be32(x) (( __be32)(__u32)(x))
#define __constant_be32_to_cpu(x) (( __u32)(__be32)(x))
#define __constant_cpu_to_be16(x) (( __be16)(__u16)(x))
#define __constant_be16_to_cpu(x) (( __u16)(__be16)(x))
#define __cpu_to_le64(x) (( __le64)swab64((x)))
#define __le64_to_cpu(x) swab64(( __u64)(__le64)(x))
#define __cpu_to_le32(x) (( __le32)swab32((x)))
#define __le32_to_cpu(x) swab32(( __u32)(__le32)(x))
#define __cpu_to_le16(x) (( __le16)swab16((x)))
#define __le16_to_cpu(x) swab16(( __u16)(__le16)(x))
#define __cpu_to_be64(x) (( __be64)(__u64)(x))
#define __be64_to_cpu(x) (( __u64)(__be64)(x))
#define __cpu_to_be32(x) (( __be32)(__u32)(x))
#define __be32_to_cpu(x) (( __u32)(__be32)(x))
#define __cpu_to_be16(x) (( __be16)(__u16)(x))
#define __be16_to_cpu(x) (( __u16)(__be16)(x))

static inline __le64 __cpu_to_le64p(const __u64 *p) {
	return (__le64)swab64p(p);
}

static inline __u64 __le64_to_cpup(const __le64 *p) {
	return swab64p((__u64 *)p);
}

static inline __le32 __cpu_to_le32p(const __u32 *p) {
	return (__le32)swab32p(p);
}

static inline __u32 __le32_to_cpup(const __le32 *p) {
	return swab32p((__u32 *)p);
}

static inline __le16 __cpu_to_le16p(const __u16 *p) {
	return ( __le16)swab16p(p);
}

static inline __u16 __le16_to_cpup(const __le16 *p) {
	return swab16p((__u16 *)p);
}

static inline __be64 __cpu_to_be64p(const __u64 *p) {
	return ( __be64)*p;
}

static inline __u64 __be64_to_cpup(const __be64 *p) {
	return ( __u64)*p;
}
static inline __be32 __cpu_to_be32p(const __u32 *p) {
	return ( __be32)*p;
}

static inline __u32 __be32_to_cpup(const __be32 *p) {
	return ( __u32)*p;
}

static inline __be16 __cpu_to_be16p(const __u16 *p) {
	return ( __be16)*p;
}

static inline __u16 __be16_to_cpup(const __be16 *p) {
	return ( __u16)*p;
}

#define __cpu_to_le64s(x) swab64s((x))
#define __le64_to_cpus(x) swab64s((x))
#define __cpu_to_le32s(x) swab32s((x))
#define __le32_to_cpus(x) swab32s((x))
#define __cpu_to_le16s(x) swab16s((x))
#define __le16_to_cpus(x) swab16s((x))
#define __cpu_to_be64s(x) do { (void)(x); } while (0)
#define __be64_to_cpus(x) do { (void)(x); } while (0)
#define __cpu_to_be32s(x) do { (void)(x); } while (0)
#define __be32_to_cpus(x) do { (void)(x); } while (0)
#define __cpu_to_be16s(x) do { (void)(x); } while (0)
#define __be16_to_cpus(x) do { (void)(x); } while (0)

#include <linux/byteorder/generic.h>

#endif /* BIG_ENDIAN_H_ */
