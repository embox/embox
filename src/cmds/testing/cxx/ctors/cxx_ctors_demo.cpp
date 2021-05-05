/**
 * @file
 * @brief
 *
 * @date   05.05.2021
 * @author Alexander Kalmuk
 */

#include <cstdio>
#include <cassert>

class HelloWorld {
	public:
		HelloWorld() {
			printf("Hello CTOR!\n");
			param = 17;
		}

		~HelloWorld() {
			printf("Hello DTOR!\n");
		}

		void inc() {
			param++;
		}

		int getparam() {
			return param;
		}

	private:
		int param = 0;
};

HelloWorld hello;

int main(int argc, char *argv[]) {
	assert(hello.getparam() == 17);

	printf("main() enter\n");
	hello.inc();
	assert(hello.getparam() == 18);
	printf("main() exit\n");

	return 0;
}
