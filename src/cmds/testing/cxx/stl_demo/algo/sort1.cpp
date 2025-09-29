/*
 *  The following code example is taken from the book
 *  The C++ Standard Library - A Tutorial and Reference
 *  by Nicolai M. Josuttis, Addison-Wesley, 1999
 *  © Copyright Nicolai M. Josuttis 1999
*/

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>

/* PRINT_ELEMENTS()
 * - prints optional C-string optcstr followed by
 * - all elements of the collection coll
 * - separated by spaces
 */
template <class T>
inline void PRINT_ELEMENTS(const T &coll, const char *optcstr = "") {
	typename T::const_iterator pos;

	std::cout << optcstr;
	for (pos = coll.begin(); pos != coll.end(); ++pos) {
		std::cout << *pos << ' ';
	}
	std::cout << std::endl;
}

/* INSERT_ELEMENTS (collection, first, last)
 * - fill values from first to last into the collection
 * - NOTE: NO half-open range
 */
template <class T>
inline void INSERT_ELEMENTS(T &coll, int first, int last) {
	for (int i = first; i <= last; ++i) {
		coll.insert(coll.end(), i);
	}
}

using namespace std;

int main(int argc, char *argv[]) {
	deque<int> coll;

	INSERT_ELEMENTS(coll, 1, 9);
	INSERT_ELEMENTS(coll, 1, 9);

	PRINT_ELEMENTS(coll, "on entry: ");

	// sort elements
	sort(coll.begin(), coll.end());

	PRINT_ELEMENTS(coll, "sorted:   ");

	// sorted reverse
	sort(coll.begin(), coll.end(), // range
	    greater<int>());           // sorting criterion

	PRINT_ELEMENTS(coll, "sorted >: ");

	return 0;
}
