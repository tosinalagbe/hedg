#pragma once
#include "marketgenerator.h"
#include "utility/queue.h"
#include "utility/utils.h"

#include <memory>


namespace hedg{

class Publisher {

public:

	Publisher(MarketGenerator& market_gen, std::unique_ptr<Market_Handler>& handler,
		io_service& service, ssl::context& context, int max_markets = config::max_markets, 
		int time_span = config::trade_span);
	
	void run();
	void try_subscribe(std::queue<std::pair<std::string,Market_Projection> >& runnable_markets);
	void order_subscribe(io_service& service, ssl::context& context);
	void clean_up_markets(io_service& service, ssl::context& context);
	void get_data();
	void get_orders();


private:

	//synchronization

	std::mutex mut_;
	std::condition_variable data_cond_;

	MarketGenerator& market_gen_;
	std::unique_ptr<Market_Handler>& handler_;

	//socket data structures
	std::queue<int> free_socks_;
	std::vector<std::unique_ptr<Client<Stream_Client> > > socks;
	std::deque<std::atomic<bool> >sock_flags;
	std::unique_ptr<Client<Stream_Client> > order_socket_;
	std::atomic<bool> order_socket_flag;

	//for cleanup
	std::deque<std::pair<std::string, Market_Projection>> subscribed_;
	std::unordered_map<std::string, std::pair<int, std::vector<std::string> > > last_ids;
	int max_markets_;
	int time_span_;
	void cache_socket(int sock_idx, std::deque<std::pair<std::string, Market_Projection> >& subscription_buffer);
};

}
