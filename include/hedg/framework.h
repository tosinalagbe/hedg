#pragma once
#include "basestrategy.h"
#include "workers/marketgenerator.h"
#include "workers/publisher.h"
#include "utility/ctpl.h"


namespace hedg{

//generates new markets every 60 mins
constexpr int gen_frequency = 60;
class Framework {
public:
	Framework()
		: gen{ gen_frequency, Timing::mins }, pub{ gen, handler, service, context }{}

	void initialize() {
		threads.emplace_back(&MarketGenerator::generate, &gen);
		threads.emplace_back(&Publisher::run, &pub);
		threads.emplace_back(&Publisher::order_subscribe, &pub, std::ref(service), std::ref(context));
		threads.emplace_back(&Publisher::clean_up_markets, &pub, std::ref(service), std::ref(context));
		threads.emplace_back(&Publisher::get_data, &pub);
		handler = std::make_unique<Market_Handler>(Market_Handler(markets));
		t_pool.resize(std::thread::hardware_concurrency());
		initialized = true;

	}

	~Framework() {
		for (auto& t : threads) {
			if (t.joinable()) {
				t.join();
			}
		}
	}

	void add_strategy(std::shared_ptr<Base_Strategy> strategy) {
		strats.push_back(std::move(strategy));
	}

	void start() {

		if (!initialized)
			initialize();
		if (strats.empty()) {
			spdlog::warn("No strategies added, please add at least one and restart.");
			return;
		}
		for (;;) {
			std::pair<int, int> new_me;
			std::pair<std::string, Market_Projection> new_market;
			markets.wait_and_pop(new_market);
			for (auto& strat : strats) {
				if (strat->is_country_allowed(new_market.second.country_code_) &&
					strat->is_event_allowed(new_market.second.event_type_)) {
					t_pool.push([=](int _) {
						std::unique_ptr<Base_Strategy> running_strat(strat->clone());
						running_strat->init(service, context, gen.get_sessionid(), new_market.first, handler);
						running_strat->run_strat();
						});
				}
			}
		}
	}

private:
	io_service service;
	ssl::context context{ ssl::context::tlsv12};
	threadsafe_queue<std::pair<std::string, Market_Projection> > markets;
	std::unique_ptr<Market_Handler> handler;
	std::vector<std::shared_ptr<Base_Strategy>> strats;
	std::vector<std::thread> threads;
	ctpl::thread_pool t_pool;
	MarketGenerator gen;
	Publisher pub;
	bool initialized{ false };

};

}
