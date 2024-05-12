#include <cassert>
#include <iostream>

#include "matr.hpp"

#define UNUSE(x) (void)(x)


int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	// // бесконечная матрица int заполнена значениями -1
	matr::Matrix<int, 2> matrix(-1);
	assert(matrix.size() == 0); // все ячейки свободны
	auto a = matrix[0][0];
	assert(a == -1);
	assert(matrix.size() == 0);
	matrix[100][100] = 314;
	assert(matrix[100][100] == 314);
	assert(matrix.size() == 1);
	// выведется одна строка
	// 100100314
	for (auto c : matrix) {
		int x;
		int y;
		int v;
		std::tie(x, y, v) = c;
		std::cout << x << y << v << std::endl;
	}
	((matrix[100][100] = 314) = 0) = 217;

	return EXIT_SUCCESS;
}
