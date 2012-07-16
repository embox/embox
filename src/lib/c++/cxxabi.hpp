/**
 * @file
 * @brief C++ ABI support header
 * @date 13.07.12
 * @author Ilia Vaprol
 */

#ifndef CXXABI_HPP_
#define CXXABI_HPP_

#include <typeinfo>

namespace __cxxabiv1 {

	class __class_type_info : public std::type_info {
	public:
		explicit __class_type_info(const char *name) : type_info(name) { }
		virtual ~__class_type_info() { }
	};

	class __si_class_type_info : public __class_type_info {
	public:
		const __class_type_info* base_type;
		explicit __si_class_type_info(const char *name, const __class_type_info *base): __class_type_info(name), base_type(base) { }
		virtual ~__si_class_type_info() { }
	};

} // namespace __cxxabiv1

#endif // CXXABI_HPP_
