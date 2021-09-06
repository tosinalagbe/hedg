#pragma once
#include "client.h"

namespace hedg {

class Stream_Client : public Client<Stream_Client> {

public:

	template<typename Buff, typename T>
	void send(void(*f)(Buff, T), T p) {
		f(inp_buff_, p);
		boost::asio::write(sock_, inp_buff_);
	}

	std::string recv() {
		boost::asio::read_until(sock_, out_buff_, "\r\n");
		std::istream is(&out_buff_);
		std::string line;
		std::getline(is, line);
		return line;
	}

	std::string recv_buff() {
		int bytes = boost::asio::read(sock_, buffer(buff), ec);
		return std::string(buff, bytes);
	}

};

}
