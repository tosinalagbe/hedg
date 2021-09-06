#pragma once

#include "config/config.h"
#include "market/marketprojection.h"
#include "timing/timing.h"
#include "client/authclient.h"
#include "client/streamclient.h"
#include "client/restclient.h"
#include "parser.h"
#include "api/messages.h"

#include <future>
#include <queue>
#include <mutex>

namespace hedg {

class MarketGenerator {

public:
	MarketGenerator(int time_span, Timing::time_unit time_unit = Timing::time_unit::hours);
	void generate();
	std::queue<std::pair<std::string, Market_Projection> > markets();
	void poll_within(int time1, int time2);
	std::queue<std::pair<std::string, Market_Projection> >markets_within(int time1, int time2);
	void wait_for_gen();
	std::string get_sessionid() {
		return session_id_;
	}

private:
	std::string session_id_;
	int time_span_;
	Timing::time_unit time_unit_;
	std::condition_variable data_cond_;
	std::mutex mut_;
	std::queue<std::pair<std::string, Market_Projection> > markets_;
	std::chrono::seconds calc_time();

};

}

