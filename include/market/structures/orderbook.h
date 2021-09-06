#pragma once
#include "market/bid.h"
#include <atomic>

namespace hedg{

class Order_Book {

	std::atomic<Bid> lay_book[10] = {};
	std::atomic<Bid> back_book[10] = {};
	float last_traded_price;


public:
	Order_Book() {
		for (int i = 0; i < 10; ++i) {
			lay_book[i].store(Bid());
			back_book[i].store(Bid());
		}
		last_traded_price = 0;
	}

	float get_lay_price(int index) {
		return lay_book[index].load().get_price();
	}
	float get_lay_size(int index) {
		return lay_book[index].load().get_size();
	}
	float get_back_price(int index) {
		return back_book[index].load().get_price();
	}
	float get_back_size(int index) {
		return back_book[index].load().get_size();
	}
	Bid get_back_bid(int index) {
		return back_book[index].load();
	}
	Bid get_lay_bid(int index) {
		return lay_book[index].load();
	}
	void update_lay(int level, Bid& bid) {
		lay_book[level].store(bid);
	}
	void update_back(int level, Bid& bid) {
		back_book[level].store(bid);
	}

	Bid best_back() {
		return back_book[0].load();
	}
	Bid best_lay() {
		return lay_book[0].load();
	}

};

}
