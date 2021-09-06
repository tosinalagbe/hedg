#pragma once

#include "market/structures/orderbook.h"
#include "fmt/core.h"
#include "fmt/format.h"

#include <string>
#include <vector>


namespace hedg{

enum Order_Type {LIMIT=0, SP};

struct Order {

	Order() : order_type{Order_Type::LIMIT} {}
	Order(int selection_id_, Bid bid_, Order_Type order_type_ = Order_Type::LIMIT):
		selection_id{ selection_id_ }, bid{ bid_ }, order_type{ order_type_ }{}
	Order(const Order& order);
	int selection_id;
	Bid bid;
	Order_Type order_type;
};


struct Lay {
	std::string instruction(std::string market_id, std::vector<Order>& orders);
};

struct Back {
	std::string instruction(std::string market_id, std::vector<Order>& orders);
};

}