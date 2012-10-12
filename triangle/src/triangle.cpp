#include <iostream>
using namespace std;
int triangle(int x, int y);
int main() {
	enum {
		MAX = 6
	};

	// making header
	std::cout << "\t";
	for (int y = 0; y < MAX; ++y) {
		std::cout << y << "\t";
	}
	std::cout << std::endl;
	std::cout << " +\t";
	for (int y = 0; y < MAX; ++y) {
		std::cout << "-\t";
	}
	std::cout << std::endl;

	//calculating
	for (int x = 0; x < MAX; ++x) {
		std::cout << x << "|\t";
		for (int y = 0; y < MAX; ++y) {

			std::cout << triangle(x, y) << "\t";
		}
		std::cout << std::endl;
	}
	return 0;
}

// 		0	1	2	3	4	5
// +	-	-	-	-	-	-
// 0|	1	1	1	1	1	1
// 1|	1	2	3	4	5	6
// 2|	1	3	6	10	15	21
// 3|	1	4	10	20	35	56
// 4|	1	5	15	35	70	126
// 5|	1	6	21	56	126	252

int triangle(int x, int y) {
	if (0 >= x || 0 >= y) {
		return 1;
	} else {
		return triangle(x - 1, y) + triangle(x, y - 1);
	}
	// never here
	return -1;
}
