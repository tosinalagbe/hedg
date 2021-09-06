#pragma once

#include "client/client.h"
#include "client/streamclient.h"
#include "client/restclient.h"
#include "config/config.h"
#include "market/marketprojection.h"

#include <vector>
#include <memory>
#include <string>
#include <queue>
#include <deque>


namespace hedg {

namespace utils {

	std::vector<std::unique_ptr<Client <Stream_Client> > > create_streamers(io_service& service, ssl::context& context, int max_streamers = config::max_markets);

	std::unique_ptr < Client <Stream_Client> > new_streamer(io_service& service, ssl::context& context);

	std::pair<std::string, int > get_ids(std::queue<std::pair<std::string, Market_Projection> >& market_ids, std::deque< std::pair<std::string, Market_Projection> >& subscribed,
		int max_markets = config::max_markets);

	std::unique_ptr<Client <Rest_Client> > new_client(io_service& service, ssl::context& context);

}

}