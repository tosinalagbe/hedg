#pragma once
#include "market/bid.h"
#include <atomic>
#include <limits>
#include <optional>


namespace hedg {

class Traded_Book {
private:

	//TODO: move to separate structure for full tick support
	std::atomic<Bid> book[350] = {};
	const float ticks[10]{ 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10 };
	const int indexes[10]{ 1,100,150,170,190,210,230,240,250,260 };
	std::atomic<float> max_price;
	std::atomic<int> max_price_idx;
	std::atomic<float> min_price;
	std::atomic<int> min_price_idx;
	std::atomic<float> max_size;
	std::atomic<float> min_size;
	std::atomic<float> max_traded_price;
	std::atomic<int> max_traded_price_idx;
	std::atomic<float> min_traded_price;
	std::atomic<int> min_traded_price_idx;
	std::atomic<float> ltp_;
	std::atomic<float> traded_volume_;

	void update_traded_range(int& idx, Bid& bid);
	void update_price(int& idx, Bid& bid);
	void update_traded_limits(int& idx, Bid& bid);

public:

	Traded_Book();


	Bid get_max_traded();
	Bid get_min_traded();
	float get_ltp();
	float get_total_traded_volume();
	

	void update(std::optional<float> ltp, std::optional<float> traded_volume, Bid& bid);

	int min_range();
	int max_range();

	float get_size(float price);
	float get_size(int idx);
	float get_price(int idx);
	int get_index(float price);

};

}

