#include <iostream>
#include <fstream>

#include <string>
#include <array>
#include <vector>
#include <tuple>

#include <utility>
#include <algorithm>

class IpAddress {
public:
	std::array<int, 4> m_ip_addresses;
public:
	IpAddress(std::vector<std::string>& ip_addresses) : m_ip_addresses() { fill(ip_addresses); }
	IpAddress(std::vector<std::string>&& ip_addresses) : m_ip_addresses() { fill(ip_addresses); }
	~IpAddress() = default;

	bool operator==(const IpAddress& rhs) const {
		if (m_ip_addresses.size() != rhs.m_ip_addresses.size()) {
			return false;
		}

		for (std::size_t i = 0; i < m_ip_addresses.size(); i++) {
			if (m_ip_addresses[i] != rhs.m_ip_addresses[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator>(const IpAddress& rhs) const {
		for (std::size_t i = 0; i < std::min(m_ip_addresses.size(), rhs.m_ip_addresses.size()); i++) {
			if (m_ip_addresses[i] > rhs.m_ip_addresses[i]) {
				return true;
			}
			else if (m_ip_addresses[i] == rhs.m_ip_addresses[i]) {
				continue;
			}
			else {
				return false;
			}
		}
		return false;
	}

	bool operator<(const IpAddress& rhs) const {
		return rhs > *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const IpAddress& obj);

private:

	void fill(std::vector<std::string>& ip_addresses) {
		for (std::size_t i = 0; i < 4; i++) {
			m_ip_addresses[i] = std::stoi(ip_addresses[i]);
		}
	}
};

std::ostream& operator<<(std::ostream& os, const IpAddress& obj) {
	for (auto ip_part = obj.m_ip_addresses.cbegin(); ip_part != obj.m_ip_addresses.cend(); ++ip_part) {
		if (ip_part != obj.m_ip_addresses.cbegin()) {
			os << ".";
		}
		os << *ip_part;
	}
	os << std::endl;
	return os;
}

std::vector<std::string> split(const std::string& str, char d) {
	std::vector<std::string> r;
	std::string::size_type start = 0;
	std::string::size_type stop = str.find_first_of(d);
	while (stop != std::string::npos) {
		r.push_back(str.substr(start, stop - start));

		start = stop + 1;
		stop = str.find_first_of(d, start);
	}
	r.push_back(str.substr(start));
	return r;
}

template <typename stream_t>
auto read_ip_address(stream_t&& stream) {
	std::vector<IpAddress> ip_pool{};

	std::string line;
	for (std::string line; !stream.eof() && std::getline(stream, line);) {
		std::vector<std::string> v = split(line, '\t');
		ip_pool.push_back(split(v.at(0), '.'));
	}
	return ip_pool;
}

auto read_ip_address(int argc, char const* argv []) {
	// return read_ip_address(std::ifstream("/home/sergey/otus/_study/data/ip_filter.tsv"));
	if (argc == 1) {
		return read_ip_address(std::cin);
	}
	else if (argc == 2) {
		return read_ip_address(std::ifstream(argv[1]));
	}
	else {
		throw std::logic_error("Unexpected behaviour");
	}
}

namespace sort_funcs {

	void reverse_lexicographically_sort(std::vector<IpAddress>& addresses) {
		std::sort(addresses.begin(), addresses.end(), [](IpAddress& a, IpAddress& b) { return a > b; });
	}


}

namespace filter_funcs {

	auto&& _consistent_filter(std::vector<IpAddress>&& addresses, std::size_t) {
		return std::move(addresses);
	}

	template<typename ...Args>
	auto&& _consistent_filter(std::vector<IpAddress>&& addresses, std::size_t i, int subnet, Args&&... arg) {
		addresses.erase(std::remove_if(addresses.begin(), addresses.end(),
			[&i, &subnet](IpAddress& x) { return x.m_ip_addresses[i] != subnet; }),
			addresses.end());
		return _consistent_filter(std::move(addresses), ++i, arg...);
	}

	template<typename ...Args>
	auto consistent_filter(std::vector<IpAddress> addresses, Args&&... arg) {
		return _consistent_filter(std::move(addresses), 0, arg...);
	}

	auto any_filter(std::vector<IpAddress> addresses, int subnet) {
		addresses.erase(
			std::remove_if(
				addresses.begin(),
				addresses.end(),
				[&subnet](IpAddress& x) {
			return !std::any_of(
				x.m_ip_addresses.cbegin(),
				x.m_ip_addresses.cend(),
				[&subnet](const auto& sn) { return sn == subnet; });
		}
			),
			addresses.end()
		);
		return addresses;
	}


}

void print_vector(std::vector<IpAddress>& addresses) {
	for (auto ip_address = addresses.cbegin(); ip_address != addresses.cend(); ++ip_address) {
		std::cout << *ip_address;
	}
}


int main(int argc, char const* argv []) {
	try {
		auto ip_addresses = read_ip_address(argc, argv);

		if (ip_addresses.empty()) {
			return EXIT_FAILURE;
		}
		// TODO reverse lexicographically sort
		sort_funcs::reverse_lexicographically_sort(ip_addresses);
		print_vector(ip_addresses);
		// 222.173.235.246
		// 222.130.177.64
		// 222.82.198.61
		// ...
		// 1.70.44.170
		// 1.29.168.152
		// 1.1.234.8

		// TODO filter by first byte and output
		auto ip_addresses_f1 = filter_funcs::consistent_filter(ip_addresses, 1);
		print_vector(ip_addresses_f1);
		// 1.231.69.33
		// 1.87.203.225
		// 1.70.44.170
		// 1.29.168.152
		// 1.1.234.8

		// TODO filter by first and second bytes and output
		auto ip_addresses_f2 = filter_funcs::consistent_filter(ip_addresses, 46, 70);
		print_vector(ip_addresses_f2);
		// 46.70.225.39
		// 46.70.147.26
		// 46.70.113.73
		// 46.70.29.76

		// TODO filter by any byte and output
		auto ip_addresses_f3 = filter_funcs::any_filter(ip_addresses, 46);
		print_vector(ip_addresses_f3);
		// 186.204.34.46
		// 186.46.222.194
		// 185.46.87.231
		// 185.46.86.132
		// 185.46.86.131
		// 185.46.86.131
		// 185.46.86.22
		// 185.46.85.204
		// 185.46.85.78
		// 68.46.218.208
		// 46.251.197.23
		// 46.223.254.56
		// 46.223.254.56
		// 46.182.19.219
		// 46.161.63.66
		// 46.161.61.51
		// 46.161.60.92
		// 46.161.60.35
		// 46.161.58.202
		// 46.161.56.241
		// 46.161.56.203
		// 46.161.56.174
		// 46.161.56.106
		// 46.161.56.106
		// 46.101.163.119
		// 46.101.127.145
		// 46.70.225.39
		// 46.70.147.26
		// 46.70.113.73
		// 46.70.29.76
		// 46.55.46.98
		// 46.49.43.85
		// 39.46.86.85
		// 5.189.203.46
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
}
