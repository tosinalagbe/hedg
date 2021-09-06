#pragma once

namespace hedg{

struct Bid {

	float price;
	float size;

	Bid() noexcept : price{0.0}, size{0.0} {}

	Bid(float price_, float size_) {
		price = price_;
		size = size_;
	}

	float get_price() {
		return price;
	}
	float get_size() {
		return size;
	}
};

}