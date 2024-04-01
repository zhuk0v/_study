#include <cstdlib>
#include <type_traits>

#include <string>
#include <vector>
#include <list>
#include <tuple>

#include <iostream>
#include <algorithm>

#define UNUSE(x) (void)(x)

// Print int8_t, int16_t, int32_t, int64_t
template<typename T>
void print_byte(T& val, int pos) {
	std::cout << (((std::make_unsigned_t<T>)val >> (pos * 8)) & (std::make_unsigned_t<T>)0xFF);
}

template <typename T>
struct cond_0 { static constexpr bool value = !std::is_class<T>::value; };

template<typename T, std::size_t n_byte = sizeof(T), typename Fake = typename std::enable_if<cond_0<T>::value>::type>
void print_ip(T val) {
	for (std::size_t i = n_byte - 1; i >= 1; i--) {
		print_byte(val, i);
		std::cout << ".";
	}
	print_byte(val, 0);
	std::cout << std::endl;
}

// std::string
template<typename T, typename std::enable_if<
	(!std::is_integral<T>::value) &&
	(std::is_same<T, std::basic_string<typename T::value_type, typename T::traits_type, typename T::allocator_type>>::value)
	>::type* = nullptr>
void print_ip(T val) {
	std::cout << val << std::endl;
}

// std::vector
// std::list
template<typename T, typename std::enable_if<
	!std::is_integral<T>::value &&
	(std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type>>::value ||
	std::is_same<T, std::list<typename T::value_type, typename T::allocator_type>>::value)
	>::type* = nullptr>
void print_ip(T val) {
	auto it = val.begin();
	for (std::size_t i = 0; i < val.size() - 1; i++, it++) {
		std::cout << *it << '.';
	}
	std::cout << *it << std::endl;
}


// std::tuple
template<class TupleType, std::size_t... I>
void print_tuple_values(const TupleType& val, std::index_sequence<I...>) {
    (..., (std::cout << std::get<I>(val) << "."));
}

template<typename T, std::size_t n_el = std::tuple_size<T>::value, typename std::enable_if<
	!std::is_integral<T>::value
	>::type* = nullptr>
void print_ip(T val) {
	print_tuple_values(val, std::make_index_sequence<n_el-1>());
	std::cout << std::get<n_el-1>(val) << std::endl;
}

int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	print_ip(int8_t{ -1 }); // 255
	print_ip(int16_t{ 0 }); // 0.0
	print_ip(int32_t{ 2130706433 }); // 127.0.0.1
	print_ip(int64_t{ 8875824491850138409 });// 123.45.67.89.101.112.131.41
	print_ip(std::string{ "Hello, World!" }); // Hello, World!
	print_ip(std::vector<int>{100, 200, 300, 400}); // 100.200.300.400
	print_ip(std::list<short>{400, 300, 200, 100}); // 400.300.200.100
	print_ip(std::make_tuple(123, 456, 789, 0)); // 123.456.789.0

	return EXIT_SUCCESS;
}
