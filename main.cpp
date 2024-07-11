#include <cassert>
#include <memory>
#include <string>
#include <atomic>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "async.h"

#define UNUSE(x) (void)(x)


class Session : public std::enable_shared_from_this<Session> {
private:
	enum {
		max_length = 1024
	};

	boost::asio::ip::tcp::socket m_tcp_socket;

	char m_data[max_length];

	std::shared_ptr<Context> bulk_context = nullptr;

public:
	Session(boost::asio::ip::tcp::socket socket, std::size_t bulk_size)
		: m_tcp_socket(std::move(socket)) {
		// std::cout << "Create session for client" << std::endl;

		bulk_context = connect(bulk_size);
	}

	~Session() {
		disconnect(bulk_context);
	}

	void start() {
		do_read();
	}

private:
	void do_read() {
		// std::cout << "Put on execution read handler" << std::endl;
		auto self = shared_from_this();
		m_tcp_socket.async_read_some(boost::asio::buffer(m_data, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
			if (ec) {
				return;
			}

			// std::cout << "Receive message on the server: " << std::string{ m_data, length } << std::endl;
			receive(m_data, length, bulk_context);


			do_read();
		});
	}
};

class Server {
private:
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::size_t m_bulk_size;

public:
	Server(boost::asio::io_context& io_context, std::uint16_t port, std::size_t bulk_size)
		: m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		m_bulk_size(bulk_size) {
		// : m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port)) {
		do_accept();
	}

private:
	void do_accept() {

		// std::cout << "Start wait client in server" << std::endl;

		m_acceptor.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
			if (ec) {
				std::cout << "Error in acceptor of server: " << ec.message() << std::endl;
				return;
			}

			// std::cout << "Processing accepting of client" << std::endl;

			std::make_shared<Session>(std::move(socket), m_bulk_size)->start();

			do_accept();
		});
	}
};

class ThreadServer {
private:
	std::thread m_thread;

	boost::asio::io_context io_context;
	Server m_server;

public:
	ThreadServer(std::uint16_t port, std::size_t bulk_size) :
		m_server(io_context, port, bulk_size) {
	}

	~ThreadServer() {
		if (m_thread.joinable()) {
			m_thread.join();
		}
	}

	void Run() {
		m_thread = std::thread([this]() {
			// std::cout << "Running server thread" << std::endl;
			io_context.run();
			// std::cout << "Finish server iocontext" << std::endl;
		});
	}
};


class ClientQuery {
public:
	// std::vector<std::string> m_list_cmds;
	std::stringstream m_list_cmds;

	std::string host;
	std::string port;
public:
	ClientQuery(std::istream& is) {
		Parse(Read(is));
	}
	~ClientQuery() {}

private:
	std::vector<std::string> Read(std::istream& is) {
		std::string query_str{};
		std::getline(is, query_str);
		std::stringstream query_ss(query_str);

		std::string buffer{};
		std::vector<std::string> query{};
		while (getline(query_ss, buffer, ' ')) {
			query.push_back(buffer);
		}
		return query;
	}

	void Parse(std::vector<std::string> query) {
		if (query[0] == "seq") {
			for (std::size_t i = static_cast<std::size_t>(std::stoll(query[1])); i <= static_cast<std::size_t>(std::stoll(query[2])); i++) {
				// m_list_cmds.push_back(std::to_string(i));
				m_list_cmds << std::to_string(i) << " ";
			}
		}
		if (query[4] == "nc") {
			host = query[5];
			port = query[6];
		}
	}
};

class Client {
private:
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::socket m_tcp_socket;

public:
	Client(std::uint16_t port) :
		m_tcp_socket(io_context) {
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), port);
		// Connect to the server
		m_tcp_socket.connect(ep, ec);

		// Check
		if (ec) {
			std::cout << "Connect to server error: " << ec.message() << std::endl;
		}

	}
	~Client() {}

	void Write(const std::string str) {
		// std::cout << "Send the message to server: " << str << std::endl;

		// Send the message
		boost::asio::write(m_tcp_socket, boost::asio::buffer(str.data(), str.size()));
	}
};

class ThreadClient {
private:
	std::thread m_thread;

	Client m_client;

public:
	ThreadClient(std::uint16_t port) :
		m_client(port) {
	}

	~ThreadClient() {
		if (m_thread.joinable()) {
			m_thread.join();
		}
	}

	void Run(std::string list_cmds) {
		m_thread = std::thread([this, list_cmds]() {
			// std::cout << "Running server thread" << std::endl;
			m_client.Write(list_cmds);
		});
	}
};

int main(int argc, char const* argv []) {
	using namespace std::chrono_literals;

	UNUSE(argc);
	UNUSE(argv);

	std::uint16_t port = static_cast<std::uint16_t>(std::stoll(std::string(argv[1])));
	std::size_t bulk_size = static_cast<std::size_t>(std::stoll(std::string(argv[2])));

	// std::cout << port << "   " << bulk_size << std::endl;

	ThreadServer thread_server(port, bulk_size);
	thread_server.Run();

	std::vector<std::shared_ptr<ThreadClient>> v_clients{};
	while (true) {
		ClientQuery query_to_server(std::cin);
		std::uint16_t port = static_cast<std::uint16_t>(std::stoll(query_to_server.port));
		v_clients.push_back(std::make_shared<ThreadClient>(port));
		v_clients.back()->Run(query_to_server.m_list_cmds.str());
	}

	return EXIT_SUCCESS;
}
