#include <algorithm>
#include <iostream>

using namespace std;

// Defining the binary function
bool comp(int a, int b) {
	return (a < b);
}

int main() {
	int a = 5;
	int b = 7;
	cout << std::min(a, b, comp) << "\n";

	// Returns the first one if both the numbers
	// are same
	cout << std::min(7, 7, comp);

	return 0;
}
