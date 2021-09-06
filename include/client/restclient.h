#pragma once
#include "client.h"

namespace hedg {

class Rest_Client : public Client<Rest_Client> {
public:
	template<typename Buff, typename T>
	void send(void(*f)(Buff&, T), T p) {
		http::request<http::string_body> req{ http::verb::post, p.end_point, 11 };
		f(req, p);
		http::write(sock_, req);
	}

	std::string recv() {
		http::response<http::string_body> res;
		http::read(sock_, out_buff_, res);
		return res.body();
	}
};

}