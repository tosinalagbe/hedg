#include "utility/utils.h"
#include <spdlog/spdlog.h>

namespace hedg {

namespace utils {

	std::vector<std::unique_ptr<Client<Stream_Client> > > create_streamers(io_service& service, ssl::context& context, int max_streamers ) {

		std::vector<std::unique_ptr<Client<Stream_Client> > > vec;
		try {
			for (int i = 0; i < max_streamers; ++i) {
				vec.push_back(std::move(std::unique_ptr<Client<Stream_Client>>(new Client<Stream_Client>{ config::stream_host, config::stream_port, service, context })));
			}
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
			spdlog::error("Unable to initialize, due to network failure"); 
			throw boost::system::system_error(e);
		}
		return vec;

	}

	std::unique_ptr < Client < Stream_Client> > new_streamer(io_service& service, ssl::context& context) {
		
		std::unique_ptr<Client<Stream_Client> > streamer;
		try {
			streamer = std::move(std::unique_ptr<Client<Stream_Client>>(new Client<Stream_Client>{ config::stream_host, config::stream_port, service, context }));
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
		}	
		return streamer;
	}

	std::unique_ptr<Client<Rest_Client>> new_client(io_service& service, ssl::context& context) {
		std::unique_ptr<Client<Rest_Client>> client;
		try {
			client = std::move(std::unique_ptr<Client<Rest_Client>>(new Client<Rest_Client>{ config::betting_host, config::betting_port, service, context }));
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
		}
		return client;
	}

	std::pair<std::string, int > get_ids(std::queue<std::pair<std::string, Market_Projection> >& market_ids, std::deque< std::pair<std::string, Market_Projection> >& subscribed,
		int max_markets) {


		std::string id_string;
		int no_of_markets = 0;
		if (!market_ids.empty()) {
			id_string += "\"";
			id_string += market_ids.front().first;
			id_string += "\"";
			subscribed.push_back(market_ids.front());
			market_ids.pop();
			no_of_markets += 1;
			--max_markets;
		}
		while (!market_ids.empty() && max_markets--) {

			id_string += ",";
			id_string += "\"";
			id_string += market_ids.front().first;
			id_string += "\"";
			subscribed.push_back(market_ids.front());
			market_ids.pop();
			no_of_markets += 1;
		}

		//discard excess markets 
		while (!market_ids.empty()) {
			market_ids.pop();
		}

		return { id_string, no_of_markets };
	}

}

}