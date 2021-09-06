#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

using namespace boost::asio;

typedef ssl::stream<ip::tcp::socket> ssl_socket;

namespace hedg {

template <typename Derived>
class Client {

public:
	
	Client(const std::string& host_name, const std::string& port_num, io_service& service, ssl::context& ssl_context)
		: sock_{ service, ssl_context }, host_name_{ host_name }, port_num_{ port_num }{

		is_connected = false;
		ip::tcp::resolver resolver(service);
		ip::tcp::resolver::query query(host_name_, port_num_);
		boost::asio::connect(sock_.lowest_layer(), resolver.resolve(query));
	}

	template<typename Buff, typename T>
	void send_request(void (*f) (Buff&, T), T p) {

		if (!is_connected) {
			sock_.handshake(ssl_socket::client);
			is_connected = true;
		}
		static_cast<Derived*>(this)->send(f, p);
	}

	void disconnect() {
		is_connected = false;
	}


	std::string recv_response() {
		return static_cast<Derived*>(this)->recv();
	}

	std::string recv_buff() {
		return static_cast<Derived*>(this)->recv_buff();
	}

protected:
	bool is_connected;
	ssl_socket sock_;
	std::string host_name_;
	std::string port_num_;
	boost::asio::streambuf inp_buff_;
	boost::asio::streambuf out_buff_;
	std::string session_id_;
	char buff[15];
	boost::system::error_code ec;

};

}