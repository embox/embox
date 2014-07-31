/**
 * @file
 * @brief Example of using try, catch, and throw statements
 * @date 16.07.12
 * @author Ilia Vaprol
 */

#include <framework/example/self.h>

#include <exception>
#include <cstdio>

EMBOX_EXAMPLE(run);

class MyException : public std::exception
{
public:
	MyException() { }
	virtual ~MyException() throw() { }
	virtual const char* what() const throw() { return "MyException"; }
};

static int run(int argc, char **argv) {
	int num = 1;
	{
		std::printf("%d -- throw an exception and catch it\n", num);
		try {
			std::printf("throw.. ");
			throw MyException();
		}
		catch (MyException e) {
			std::printf("catch: %s\n", e.what());
		}
		std::printf("%d -- done\n", num++);
	}
	{
		std::printf("%d -- throw an exception and will not catch it\n", num);
		throw MyException();
		std::printf("it will never be printed\n");
	}

	return 0;
}

