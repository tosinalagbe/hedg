#pragma once

#include "utility/queue.h"
#include "market/structures/orderbook.h"
#include "market/structures/tradedbook.h"
#include "market/marketprojection.h"
#include "market/order.h"

#include <unordered_map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <thread>


namespace hedg{

class Market_Handler {

public:
	Market_Handler(threadsafe_queue<std::pair<std::string, Market_Projection> >& active_markets)
		: active_markets_{ active_markets } {}
	

	float get_lay_price(std::string market_id, int runner_index, int level) {
		return market_book[market_id].at(runner_index)->get_lay_bid(level).get_price();
	}
	float get_back_price(std::string market_id, int runner_index, int level) {
		return market_book[market_id].at(runner_index)->get_back_bid(level).get_price();
	}
	float get_traded_volume(std::string market_id, int runner_index) {
		return traded[market_id].at(runner_index)->get_total_traded_volume();
	}
	float get_market_traded_volume(std::string market_id) {
		return traded_volume[market_id].load();
	}
	std::unique_ptr<Order_Book>& order_book(std::string market_id, int runner_index) {
		return market_book[market_id].at(runner_index);
	}
	std::unique_ptr<Traded_Book>& traded_book(std::string market_id, int runner_index) {
		return traded[market_id].at(runner_index);
	}
	const std::vector<std::unique_ptr<Order_Book> >& order_books(std::string market_id) {
		return market_book[market_id];
	}
	const std::vector<std::unique_ptr<Traded_Book> >& traded_books(std::string market_id) {
		return traded[market_id];
	}

	int get_runner_id(std::string market_id, int runner_idx) {
		return runner_ids[market_id].at(runner_idx);
	}


	bool is_active(std::string market_id) {
		if(active.find(market_id) != active.end())
			return active[market_id].load();
		return false;
	}

	void deactivate_market(std::string market_id) {
		active[market_id].store(false);
	}

	void activate_market(std::string market_id, Market_Projection market_projection) {
		active[market_id].store(true);
		active_markets_.push({ market_id, market_projection });
	}

	void add_new_market(std::string market_id) {
		market_book[market_id] = std::vector<std::unique_ptr<Order_Book>>();
	}

	void add_runner(std::string market_id, int runner_id) {

		market_book[market_id].push_back(std::make_unique<Order_Book>());
		traded[market_id].push_back(std::make_unique<Traded_Book>());
		int idx = market_book[market_id].size() - 1;
		runner_loc[market_id][runner_id] = idx;
		runner_ids[market_id][idx] = runner_id;
	}

	void update_back(std::string market_id, int runner_id, int level, Bid& bid) {
		market_book[market_id][runner_loc[market_id][runner_id]]->update_back(level, bid);
	}
	void update_lay(std::string market_id, int runner_id, int level, Bid& bid) {
		market_book[market_id][runner_loc[market_id][runner_id]]->update_lay(level, bid);
	}

	void update_traded(std::string market_id, int runner_id, std::optional<float> ltp, std::optional<float> traded_volume, Bid& bid) {
		traded[market_id][runner_loc[market_id][runner_id]]->update(ltp, traded_volume,bid);
	}

	void update_total_traded_volume(std::string market_id, float total_traded_volume) {
		traded_volume[market_id].store(total_traded_volume);
	}
	

	//for performance, call only at the beginning of strategy (before looping)
	std::vector<int> get_runner_idx_sorted_by_price(std::string market_id) {

		std::vector<int> indexes(market_book[market_id].size());
		std::iota(indexes.begin(), indexes.end(), 0);
		std::sort(indexes.begin(), indexes.end(),
			[&](int left, int right) {return market_book[market_id][left]->get_lay_price(0)
								< market_book[market_id][right]->get_lay_price(0); });

		return indexes;
	}
	void update_market(std::string market_id) {

	}

	const std::unordered_map < std::string, std::vector<std::unique_ptr<Order_Book>> >& all_market_books() {
		return market_book;
	}

	const std::unordered_map <std::string, std::vector<std::unique_ptr<Traded_Book>> >& all_traded_books() {
		return traded;
	}

	void clear_market(std::string market_id) {
		active.erase(market_id);
		std::this_thread::sleep_for(std::chrono::seconds(30));
		market_book.erase(market_id);
		traded.erase(market_id);
		traded_volume.erase(market_id);
		runner_loc.erase(market_id);
		runner_ids.erase(market_id);

	}

private:
	std::unordered_map < std::string, std::vector<std::unique_ptr<Order_Book>> >market_book;
	std::unordered_map <std::string, std::vector<std::unique_ptr<Traded_Book>> >traded;
	std::unordered_map<std::string, std::atomic<float> > traded_volume;
	std::unordered_map<std::string, std::unordered_map<int, int>> runner_loc;
	std::unordered_map<std::string, std::unordered_map<int, int>> runner_ids;
	std::unordered_map<std::string, std::atomic<bool> > active;
	threadsafe_queue<std::pair<std::string, Market_Projection> >& active_markets_;
};

}
