/**
 * @file
 * @brief Exception Handling support
 * @date 12.07.12
 * @author Ilia Vaprol
 */

#include <exception>
#include <cstdlib>

static std::terminate_handler __terminate_handler = std::abort;

static std::unexpected_handler __unexpected_handler = std::terminate;


std::terminate_handler std::set_terminate(std::terminate_handler handler) throw() {
	std::terminate_handler prev_handler = __terminate_handler;
	__terminate_handler = handler;
	return prev_handler;
}

void std::terminate() throw() {
	try {
		__terminate_handler();
		std::abort();
	}
	catch (...) {
		std::abort();
	}
}

std::unexpected_handler set_unexpected(std::unexpected_handler handler) throw() {
	std::unexpected_handler prev_handler = __unexpected_handler;
	__unexpected_handler = handler;
	return prev_handler;
}

void std::unexpected() {
	__unexpected_handler();
	std::terminate();
}

