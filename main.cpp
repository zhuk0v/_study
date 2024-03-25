#include <cstdlib>
#include <vector>
#include <map>

#include <iostream>

#include "cust_alloc.hpp"
#include "cust_container.hpp"

#define UNUSE(x) (void)(x)



template <typename Key, typename Tp>
using map_1 = std::map<Key, Tp>;

template <typename T>
using vct_1 = std::vector<T>;

template <typename Key, typename Tp, std::size_t N, bool Fix>
using resource_map = custom::AllocatorResource<std::pair<const Key, Tp>, N, Fix>;

template <typename Key, typename Tp>
using map_2 = std::pmr::map<Key, Tp>;

template <typename T, std::size_t N, bool Fix>
using resource_vct = custom::AllocatorResource<T, N, Fix>;

template <typename T>
using vct_2 = std::pmr::vector<T>;

template <typename T, std::size_t N, bool Fix>
using resource_cnt = resource_vct<T, N, Fix>;

template <typename T>
using cnt = custom::Container<T>;


auto fill_map = [](auto& l_map) {
	// заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
	l_map[0] = 1;
	for (std::size_t i = 1; i < 10; i++) {
		l_map[i] = l_map[i - 1] * i;
	}
};

auto print_map = [](const auto& l_map) {
	// вывод на экран всех значений (ключ и значение разделены пробелом) хранящихся в контейнере
	for (const auto& pair : l_map) {
		std::cout << pair.first << " " << pair.second << std::endl;
	}
};

auto fill_vector = [](auto& l_vect) {
	// заполнение 10 элементами от 0 до 9
	for (std::size_t i = 0; i < 10; i++) {
		l_vect.push_back(i);
	}
};

auto print_vector = [](const auto& l_vect) {
	// вывод на экран всех значений, хранящихся в контейнере
	for (const auto& el : l_vect) {
		std::cout << el << std::endl;
	}
};


int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	const std::size_t num_elements = 10;
	const bool fix_my_mem = false;

	// создание экземпляра std::map<int, int>
	map_1<int, int> factorials;
	// заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
	fill_map(factorials);

	// выделение памяти
	resource_map<int, int, num_elements, fix_my_mem> map_2_res;
	// создание экземпляра std::map<int, int> с новым аллокатором, ограниченным 10 элементами
	map_2<int, int> factorials_with_cust_alloc(&map_2_res);
	// заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
	fill_map(factorials_with_cust_alloc);

	// вывод на экран всех значений (ключ и значение разделены пробелом) хранящихся в контейнере
	print_map(factorials);
	print_map(factorials_with_cust_alloc);

	// создание экземпляра std::vector<int>
	vct_1<int> seq;
	// заполнение 10 элементами от 0 до 9
	fill_vector(seq);

	// выделение памяти
	resource_vct<int, num_elements, fix_my_mem> vct_2_res;
	// создание экземпляра std::vector<int> с новым аллокатором, ограниченным 10 элементами
	vct_2<int> seq_with_cust_alloc(&vct_2_res);
	// заполнение 10 элементами от 0 до 9
	fill_vector(seq_with_cust_alloc);

	// вывод на экран всех значений, хранящихся в контейнере
	print_vector(seq);
	print_vector(seq_with_cust_alloc);

	// выделение памяти
	resource_cnt<cnt<int>::value_type, num_elements, fix_my_mem> cnt_res;
	// создание экземпляра своего контейнера для хранения значений типа int
	cnt<int> seq_with_cust_cont_and_cust_alloc(&cnt_res);
	// заполнение 10 элементами от 0 до 9
	for (std::size_t i = 0; i < 10; i++) {
		seq_with_cust_cont_and_cust_alloc.append(i);
	}
	// вывод на экран всех значений, хранящихся в контейнере
	for (auto& el : seq_with_cust_cont_and_cust_alloc) {
		std::cout << el << std::endl;
	}

	return EXIT_SUCCESS;
}
