
#include "market/structures/tradedbook.h"
#include <iostream>

namespace hedg{

float round_2dp(float var)
{
	float value = (int)(var * 100 + .5);
	return (float)value / 100;
}


void Traded_Book::update_traded_range(int& idx, Bid& bid) {
	if (bid.get_price() < min_price.load()) {
		min_price.store(bid.get_price());
		min_price_idx.store(idx);
	}
	if (bid.get_price() > max_price.load()) {
		max_price.store(bid.get_price());
		max_price_idx.store(idx);
	}

}
void Traded_Book::update_price(int& idx, Bid& bid) {
	idx = get_index(bid.get_price());
	book[idx].store(bid);

}

void Traded_Book::update_traded_limits(int& idx, Bid& bid) {
	if (bid.get_size() > max_size.load()) {
		max_size.store(bid.get_size());
		max_traded_price.store(bid.get_price());
		max_traded_price_idx.store(idx);
	}
	if (bid.get_size() < min_size.load()) {
		min_size.store(bid.get_size());
		min_traded_price.store(bid.get_price());
		min_traded_price_idx.store(idx);
	}

}

Traded_Book::Traded_Book() {

	max_price.store(std::numeric_limits<float>::min());
	min_price.store(std::numeric_limits<float>::max());
	max_size.store(std::numeric_limits<float>::min());
	min_size.store(std::numeric_limits<float>::max());
	min_price_idx.store(0);
	max_price_idx.store(-1);
	max_traded_price_idx.store(0);
	min_traded_price_idx.store(0);

	float x = 1.01;
	int space = 0;
	for (int i = 0, t_idx = 0; i < 350; ++i) {

		if (1 <= x && x < 2)
			t_idx = 0;
		else if (2 <= x && x < 3)
			t_idx = 1;
		else if (3 <= x && x < 4)
			t_idx = 2;
		else if (4 <= x && x < 6)
			t_idx = 3;
		else if (6 <= x && x < 10)
			t_idx = 4;
		else if (10 <= x && x < 20)
			t_idx = 5;
		else if (20 <= x && x < 30)
			t_idx = 6;
		else if (30 <= x && x < 50)
			t_idx = 7;
		else if (50 <= x && x < 100)
			t_idx = 8;
		else if (100 <= x && x < 1000)
			t_idx = 9;

		x = round_2dp(x);
		book[i].store(Bid(x, 0));
		x += ticks[t_idx];
	}
}


Bid Traded_Book::get_max_traded() {
	float price = max_traded_price;
	float size = book[max_traded_price_idx].load().get_size();
	return Bid(price, size);
}

Bid Traded_Book::get_min_traded() {
	float price = min_traded_price;
	float size = book[min_traded_price_idx].load().get_size();
	return Bid(price, size);
}

float Traded_Book::get_ltp(){
	return ltp_.load();
}

float Traded_Book::get_total_traded_volume() {
	return traded_volume_.load();
}




void Traded_Book::update(std::optional<float> ltp, std::optional<float> traded_volume, Bid& bid) {
	int idx = 0;
	update_price(idx, bid);
	update_traded_range(idx, bid);
	update_traded_limits(idx, bid);
	if (ltp.has_value())
		ltp_.store(ltp.value());
	if (traded_volume.has_value())
		traded_volume_.store(traded_volume.value());
}

int Traded_Book::min_range() {
	return min_price_idx.load();
}
int Traded_Book::max_range() {
	return max_price_idx.load();
}

float Traded_Book::get_size(float price) {
	int idx = get_index(price);
	return book[idx].load().get_size();
}

float Traded_Book::get_price(int idx) {
	return book[idx].load().get_price();
}

float Traded_Book::get_size(int idx) {

	float ffs = (book + idx)->load().get_size();
	return book[idx].load().get_size();
}




int Traded_Book::get_index(float price) {
	int t_idx = 0;
	if (1 <= price && price < 2) {
		t_idx = 0;
		return indexes[t_idx] - 1 + (int)((price - 1.01) / ticks[t_idx] + 0.5);
	}
	else if (2 <= price && price < 3) {
		t_idx = 1;
		return indexes[t_idx] - 1 + (int)((price - 2) / ticks[t_idx] + 0.5);
	}
	else if (3 <= price && price < 4) {
		t_idx = 2;
		return  indexes[t_idx] - 1 + (int)((price - 3) / ticks[t_idx] + 0.5);

	}
	else if (4 <= price && price < 6) {
		t_idx = 3;
		return indexes[t_idx] - 1 + (int)((price - 4) / ticks[t_idx] + 0.5);
	}
	else if (6 <= price && price < 10) {
		t_idx = 4;
		return  indexes[t_idx] - 1 + (int)((price - 6) / ticks[t_idx] + 0.5);
	}
	else if (10 <= price && price < 20) {
		t_idx = 5;
		return indexes[t_idx] - 1 + (int)((price - 10) / ticks[t_idx] + 0.5);
	}
	else if (20 <= price && price < 30) {
		t_idx = 6;
		return indexes[t_idx] - 1 + (int)((price - 20) / ticks[t_idx] + 0.5);

	}
	else if (30 <= price && price < 50) {
		t_idx = 7;
		return indexes[t_idx] - 1 + (int)((price - 30) / ticks[t_idx] + 0.5);
	}
	else if (50 <= price && price < 100) {
		t_idx = 8;
		return  indexes[t_idx] - 1 + (int)((price - 50) / ticks[t_idx] + 0.5);
	}
	else if (100 <= price && price < 1000) {
		t_idx = 9;
		return indexes[t_idx] - 1 + (int)((price - 100) / ticks[t_idx] + 0.5);
	}
	return 0;
}

}
