/**
 * @file
 * @brief Command that changes the shared STDIN flags.
 */

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
	return fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}
