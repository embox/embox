/**
 * @file
 *
 * @brief
 *
 * @date 06.07.2011
 * @author Anton Bondarev
 */

#ifndef FRAMEWORK_EXAMPLE_TYPES_H_
#define FRAMEWORK_EXAMPLE_TYPES_H_

#include <framework/mod/self.h>
#include <framework/mod/ops.h>

typedef int (*example_exec_t)(int argc, char **argv);

struct example {
	example_exec_t exec;
};

struct example_mod {
	struct mod mod;
	struct example example;
};

#endif /* FRAMEWORK_EXAMPLE_TYPES_H_ */
