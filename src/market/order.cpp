#include "market/order.h"

namespace hedg {

std::string Lay::instruction(std::string market_id, std::vector<Order>& orders) {

	std::string instructions;

	if (!orders.empty()) {
		for (int i = 0; i < orders.size() - 1; ++i) {

			if (orders[i].order_type == LIMIT) {
				fmt::format_to(std::back_inserter(instructions),
					"{{"
					"\"selectionId\": \"{}\","
					"\"handicap\": \"0\","
					"\"side\": \"LAY\","
					"\"orderType\": \"LIMIT\","
					"\"limitOrder\": {{"
					"\"size\": \"{}\","
					"\"price\": \"{}\","
					"\"persistenceType\": \"LAPSE\""
					"}}"
					"}},", orders[i].selection_id, orders[i].bid.get_size(), orders[i].bid.get_price());
			}
			else if (orders[i].order_type == SP) {
				fmt::format_to(std::back_inserter(instructions),
					"{{"
					"\"selectionId\": \"{}\","
					"\"handicap\": \"0\","
					"\"side\": \"LAY\","
					"\"orderType\": \"MARKET_ON_CLOSE\","
					"\"marketOnCloseOrder\": {{"
					"\"liability\": \"{}\""
					"}}"
					"}},", orders[i].selection_id, orders[i].bid.get_size());
			}
		}
		if (orders[orders.size() - 1].order_type == LIMIT) {
			fmt::format_to(std::back_inserter(instructions),
				"{{"
				"\"selectionId\": \"{}\","
				"\"handicap\": \"0\","
				"\"side\": \"LAY\","
				"\"orderType\": \"LIMIT\","
				"\"limitOrder\": {{"
				"\"size\": \"{}\","
				"\"price\": \"{}\","
				"\"persistenceType\": \"LAPSE\""
				"}}"
				"}}", orders[orders.size() - 1].selection_id, orders[orders.size() - 1].bid.get_size(), orders[orders.size() - 1].bid.get_price());
		}
		else if (orders[orders.size() - 1].order_type == SP) {
			fmt::format_to(std::back_inserter(instructions),
				"{{"
				"\"selectionId\": \"{}\","
				"\"handicap\": \"0\","
				"\"side\": \"LAY\","
				"\"orderType\": \"MARKET_ON_CLOSE\","
				"\"marketOnCloseOrder\": {{"
				"\"liability\": \"{}\""
				"}}"
				"}}", orders[orders.size() - 1].selection_id, orders[orders.size() - 1].bid.get_size());
		}

	}
	std::string body;

	fmt::format_to(std::back_inserter(body),"["
		"{{"
		"\"jsonrpc\": \"2.0\","
		"\"method\": \"SportsAPING/v1.0/placeOrders\","
		"\"params\": {{"
		"\"marketId\": \"{}\","
		"\"instructions\": ["
		"{}"
		"]"
		"}},"
		"\"id\": 1"
		"}}"
		"]", market_id, instructions);
	
	return body;
}



std::string Back::instruction(std::string market_id, std::vector<Order>& orders) {
	

	std::string instructions;
	if (!orders.empty()) {
		for (int i = 0; i < orders.size() - 1; ++i) {

			if (orders[i].order_type == LIMIT) {
				fmt::format_to(std::back_inserter(instructions),
					"{{"
					"\"selectionId\": \"{}\","
					"\"handicap\": \"0\","
					"\"side\": \"BACK\","
					"\"orderType\": \"LIMIT\","
					"\"limitOrder\": {{"
					"\"size\": \"{}\","
					"\"price\": \"{}\","
					"\"persistenceType\": \"LAPSE\""
					"}}"
					"}},", orders[i].selection_id, orders[i].bid.get_size(), orders[i].bid.get_price());
			}
			else if (orders[i].order_type == SP) {
				fmt::format_to(std::back_inserter(instructions),
					"{{"
					"\"selectionId\": \"{}\","
					"\"handicap\": \"0\","
					"\"side\": \"BACK\"",
					"\"orderType\": \"MARKET_ON_CLOSE\","
					"\"marketOnCloseOrder\": {{"
					"\"liability\": \"{}\""
					"}}"
					"}},", orders[i].selection_id, orders[i].bid.get_size());
			}
		}
		if (orders[orders.size() - 1].order_type == LIMIT) {

			fmt::format_to(std::back_inserter(instructions),
				"{{"
				"\"selectionId\": \"{}\","
				"\"handicap\": \"0\","
				"\"side\": \"BACK\","
				"\"orderType\": \"LIMIT\","
				"\"limitOrder\": {{"
				"\"size\": \"{}\","
				"\"price\": \"{}\","
				"\"persistenceType\": \"LAPSE\""
				"}}"
				"}}", orders[orders.size() - 1].selection_id, orders[orders.size() - 1].bid.get_size(), orders[orders.size() - 1].bid.get_price());
		}
		else if (orders[orders.size() - 1].order_type == SP) {
			fmt::format_to(std::back_inserter(instructions),
				"{{"
				"\"selectionId\": \"{}\","
				"\"handicap\": \"0\","
				"\"side\": \"BACK\","
				"\"orderType\": \"MARKET_ON_CLOSE\","
				"\"marketOnCloseOrder\": {{"
				"\"liability\": \"{}\""
				"}}"
				"}}", orders[orders.size() - 1].selection_id, orders[orders.size() - 1].bid.get_size());
		}

	}
	

	std::string body;

	fmt::format_to(std::back_inserter(body), "["
		"{{"
		"\"jsonrpc\": \"2.0\","
		"\"method\": \"SportsAPING/v1.0/placeOrders\","
		"\"params\": {{"
		"\"marketId\": \"{}\","
		"\"instructions\": ["
		"{}"
		"]"
		"}},"
		"\"id\": 1"
		"}}"
		"]", market_id, instructions);

	return body;
}

Order::Order(const Order& order){
	selection_id = order.selection_id;
	bid = order.bid;
	order_type = order.order_type;
}

}