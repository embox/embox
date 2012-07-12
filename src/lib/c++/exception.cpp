/**
 * @file
 * @brief Exception Handling support
 * @date 12.07.12
 * @author Ilia Vaprol
 */

#include <exception>
#include <cstdlib>

static std::terminate_handler __terminate_handler = std::abort;

std::terminate_handler std::set_terminate(std::terminate_handler handler) throw() {
	std::terminate_handler prev_handler = __terminate_handler;
	__terminate_handler = handler;
	return prev_handler;
}

void std::terminate(void) {
	__terminate_handler();
}

