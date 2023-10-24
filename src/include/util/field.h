/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#ifndef UTIL_FIELD_H_
#define UTIL_FIELD_H_

#define FIELD(name, v) (((v) & (name##_MASK)) << (name##_SHIFT))

#define FIELD_GET(x, name) (((x) >> (name##_SHIFT)) & (name##_MASK))

#define FIELD_SET(x, name, v)                   \
	(((x) & ~((name##_MASK) << (name##_SHIFT))) \
	    | (((v) & (name##_MASK)) << (name##_SHIFT)))

#endif /* UTIL_FIELD_H_ */
