#include "workers/publisher.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"


namespace hedg {

Publisher::Publisher(MarketGenerator& market_gen, std::unique_ptr<Market_Handler>& handler, 
	io_service& service, ssl::context& context, int max_markets, int time_span) :
	market_gen_{ market_gen }, handler_{ handler }, max_markets_{ max_markets }, time_span_{ time_span } {
	socks = utils::create_streamers(service, context, max_markets_);

	for (int i = 0; i < max_markets_; ++i) {
		sock_flags.emplace_back(0);
		free_socks_.push(i);
	}
	order_socket_ = utils::new_streamer(service, context);
	order_socket_flag = 0;
	
}

void Publisher::run() {

	for (;;) {

		auto runnable_markets = market_gen_.markets_within(2, 2+time_span_);
		try_subscribe(runnable_markets);
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::seconds(5));

	}
		
}

void Publisher::try_subscribe(std::queue<std::pair<std::string, Market_Projection> >& runnable_markets) {

	std::deque<std::pair<std::string, Market_Projection> >subscription_buffer;
	int no_subscribed = subscribed_.size();
	auto pending = utils::get_ids(runnable_markets, subscription_buffer, max_markets_ - no_subscribed);
	std::string market_ids = pending.first;
	int number_pending = pending.second;
	if (number_pending && no_subscribed + number_pending <= max_markets_ && !free_socks_.empty()) {

		int idx = free_socks_.front();
		free_socks_.pop();

		try{
			socks[idx]->send_request(stream::authenticate_msg, params::stream_authenticate(market_gen_.get_sessionid()));
			socks[idx]->send_request(stream::subscribe_msg, params::stream_subscribe(market_ids));
			sock_flags[idx].store(1);
			spdlog::info("Subscribed to " + market_ids);


			for (auto subs : subscription_buffer) {
				subscribed_.push_back(subs);
			}
			cache_socket(idx, subscription_buffer);
		}catch (boost::system::system_error e) {
			spdlog::error(e.what());
			free_socks_.push(idx);
		}

	}

}

void Publisher::order_subscribe(io_service& service, ssl::context& context) {

	for (;;) {
		market_gen_.wait_for_gen();
		order_socket_flag.store(0);
		order_socket_.reset();
		order_socket_ = utils::new_streamer(service, context);
		if (!order_socket_)
			continue;
		order_socket_->send_request(stream::authenticate_msg, params::stream_authenticate(market_gen_.get_sessionid()));
		order_socket_->send_request(stream::order_subscribe_msg, params::stream_order());
		order_socket_flag.store(1);
		//poll every hour
		std::this_thread::sleep_for(std::chrono::minutes(60));		
	}
}




void Publisher::clean_up_markets(io_service& service, ssl::context& context) {

	std::queue<std::string>pending_removal;
	for (;;) {

		if (!subscribed_.empty()) {

			auto market_id = subscribed_.front().first;
			auto expiry_time = subscribed_.front().second.market_start_time_;
			double diff = Timing::time_diff_mins(expiry_time, Timing::now());
			int diff_in_secs = diff * 60;

			if (diff_in_secs < 10) {


				//to notify threads using data that it's time up 
				spdlog::info("Rendering " + market_id + " inactive");
				handler_->deactivate_market(market_id);
				subscribed_.pop_front();

				if (last_ids.find(market_id) != last_ids.end()) {
					int sock_idx = last_ids[market_id].first;
					//socket is now free
					sock_flags[sock_idx].store(0);
					std::this_thread::sleep_for(std::chrono::seconds(10));
					socks[sock_idx].reset();
					socks[sock_idx] = utils::new_streamer(service, context);
					if(socks[sock_idx])
						free_socks_.push(sock_idx);

					//now clear inactive markets
					for (auto mkts : last_ids[market_id].second) {
						handler_->clear_market(mkts);
						spdlog::info("Removed " + mkts + " finally");
					}
					//finally discard market_id from last_ids
					last_ids.erase(market_id);
				}

				std::this_thread::sleep_for(std::chrono::seconds(10));

			}
			else
				std::this_thread::sleep_for(std::chrono::seconds(diff_in_secs - 10));

		}
		else {
			std::this_thread::sleep_for(std::chrono::seconds(30));
		}

	}

	
}



void Publisher::get_data() {

	Parser parser(handler_);
	std::string msg;
	msg.reserve(size_t(10000));
	for (;;) {

		for (int i = 0; i < socks.size(); ++i) {
			
			if (sock_flags[i].load()) {
				try {
					msg = socks[i]->recv_buff();
					parser.parse(i, msg).parse_and_update(subscribed_);
				}
				catch (boost::system::system_error e) {
					spdlog::error(e.what());
					spdlog::error("Stopped collecting market data due to network failure");
				}
				
			}
			else {
				parser.reset(i);
			}		
		}
	}
}

void Publisher::get_orders() {
	Parser parser(handler_);
	std::string msg;
	msg.reserve(size_t(10000));
	for (;;) {
		if (order_socket_flag.load()) {
			try {
				msg = order_socket_->recv_response();
			}
			catch (boost::system::system_error e) {
				spdlog::error(e.what());
				spdlog::error("Stopped collecting order data due to network failure");
			}
			
			//TODO: parse order to appropriate data structure
		}
		
	}
}

void Publisher::cache_socket(int sock_idx, std::deque<std::pair<std::string, Market_Projection> >& subscription_buffer) {

	std::string last_id = subscription_buffer.back().first;
	last_ids[last_id].second.push_back(last_id);
	last_ids[last_id].first = sock_idx;
	subscription_buffer.pop_back();

	while (!subscription_buffer.empty()) {
		last_ids[last_id].second.push_back(subscription_buffer.back().first);
		subscription_buffer.pop_back();
	}
	spdlog::debug("Completed socket caching");
}

}