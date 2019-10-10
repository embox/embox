/*
 *  The following code example is taken from the book
 *  The C++ Standard Library - A Tutorial and Reference
 *  by Nicolai M. Josuttis, Addison-Wesley, 1999
 *  © Copyright Nicolai M. Josuttis 1999
*/


#if defined(__GNUC__) && (__GNUC__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
	#include "algostuff.hpp"
#if defined(__GNUC__) && (__GNUC__ >= 6)
#pragma GCC diagnostic pop
#endif

using namespace std;

int main(int argc, char *argv[])
{
    deque<int> coll;

    INSERT_ELEMENTS(coll,1,9);
    INSERT_ELEMENTS(coll,1,9);

    PRINT_ELEMENTS(coll,"on entry: ");

    // sort elements
    sort (coll.begin(), coll.end());

    PRINT_ELEMENTS(coll,"sorted:   ");

    // sorted reverse
    sort (coll.begin(), coll.end(),    // range
          greater<int>());             // sorting criterion

    PRINT_ELEMENTS(coll,"sorted >: ");

    return 0;
}
