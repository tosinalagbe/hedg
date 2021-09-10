[![Build Status](https://app.travis-ci.com/tosinalagbe/hedg.svg?branch=main)](https://app.travis-ci.com/tosinalagbe/hedg)

# Hedg
Fast and easy to use, high frequency trading framework for betfair

<!-- ABOUT THE PROJECT -->
## About Hedg
In the sports trading industry, low latency is really important. The need to be first in line on the queue as well as the need to be able to cancel or close out bad orders quickly are some of the latency sensitive needs involved. Hedg was built with these in mind. Also enables quants and automated traders who want to use efficient c++ libraries to build strategies that are latency sensitive.(At the moment, this framework only supports trading pre-off markets on betfair).

<!-- GETTING STARTED -->
## Getting Started
To get up and running, follow these steps.

### Prerequisites
Hedg makes use of the boost c++ library and more importanly needs a version up to 1.67 that comes with boost beast.
For Windows users, I recommend using the [vcpkg](https://github.com/microsoft/vcpkg#quick-start-windows) package manager to install the latest version of boost. Once vcpkg is installed, install boost:
```
vcpkg install boost:x64-windows
```
For Linux Users, install boost with the apt package manager:
```
sudo apt-get update
sudo apt-get install libboost-all-dev
```

### Buiding and Running
1. Clone the repo
2. Locate the config.cpp file in the src/config directory.
3. Edit the config.cpp file to suit your betfair log in details as well as the path where your key and certificate files are stored (note that the path is relative to where your final executable is).
4. - For Windows Users, create a new C++ project with Visual Studio and add the header and source files, then let Visual Studio build and compile it automatically.
 - For Linux Users, build with cmake:
  ```
  mkdir build
  cd build/
  cmake ..
  make
  ```
5. The steps above runs the example vwapstrategy in the repo. For linux users, add your strategy(example usage below) and update the CMakeLists.txt file to include your strategy source code as required.

## Usage
To implement your strategies to be run on Hedg, there are three requirements
1. All strategies should derive from ```Base_Strategy```
2. ```clone()``` function that returns a pointer to your strategy class must be implemented.
3. ```run_strat()``` should be implemented. run_strat is the crux of your strategy.
A not so concise example is shown below:
```
#include "hedg/framework.h"

struct vwap_strategy : public hedg::Base_Strategy {

	vwap_strategy() {}
	vwap_strategy* clone() {
		return new vwap_strategy();
	}

	//parameters to be set only at the beginning of the strategy
	void init() {
		market_id = get_market_id();
		spdlog::info("starting vwap strategy on " + market_id);
		sorted_runners = handler->get_runner_idx_sorted_by_price(market_id);

		//warm up the cache
		//send empty orders
		for (int i = 0; i < 10; ++i) {
			std::vector<hedg::Order> order;
			send_lay_order(order);
		}
	}
	void run_strat() override {

		//I recommend this pattern
		init();
		while (handler->is_active(market_id)) {
			int fave_runner_idx = *sorted_runners.begin();	
			float tot_vol = handler->get_traded_volume(market_id, fave_runner_idx);
			float fave = handler->traded_book(market_id, fave_runner_idx)->get_ltp();
			int runner_id = handler->get_runner_id(market_id, fave_runner_idx);

			//read your market parameters
			const auto& fave_runner_order_book = handler->order_book(market_id, fave_runner_idx);
			const auto& fave_runner_traded_book = handler->traded_book(market_id, fave_runner_idx);
			hedg::Bid fave_runner_best_available_back = fave_runner_order_book->best_back();
			hedg::Bid fave_runner_best_available_lay = fave_runner_order_book->best_back();
			float fave_runner_vwap = vwap(fave_runner_idx);
			float fave_runner_wom = wom(fave_runner_idx);

			//opening signal to place order (leave unmatched at bsp)
			//wait for wom to favour laying the place lay when there's little money on the lay size to be at top of queue
			if (fave_runner_wom > 75 && fave_runner_best_available_lay.get_size() < 10
			&& entry_price != fave_runner_best_available_back.get_price()) {
				//create order
				float order_price = fave_runner_best_available_lay.get_price();
				//using 0 pound stake
				float order_size = 0;
				hedg::Bid bid{ order_price, order_size };
				//Order_Type::SP means take sp if unmatched and market closes
				std::vector<hedg::Order> order{ hedg::Order(runner_id, bid, hedg::Order_Type::SP ) };
				send_lay_order(order);
				//avoid logging in hot path!!! only using as an example
				spdlog::info(market_id + ": Sent opening lay order on "+ std::to_string(runner_id) +" of "
					+ std::to_string(bid.get_size()) + " at " + std::to_string(bid.get_price()));
				//store order to be closed
				open_orders.push(order[0]);
				entry_price = order_price;
				
			}

		
			//close open orders (leave unmatched at bsp)
			while (!open_orders.empty()) {
				auto order = open_orders.front();
				open_orders.pop();
				int curr_index = fave_runner_traded_book->get_index(order.bid.get_price());
				//place closing order, 5 ticks away
				int closing_index = curr_index + 5;
				float closing_price = handler->traded_book(market_id, fave_runner_idx)->get_price(closing_index);
				float order_size = 0;
				hedg::Bid bid{ closing_price, order_size };
				std::vector<hedg::Order> new_order{ hedg::Order(runner_id, bid, hedg::Order_Type::SP) };
				send_back_order(new_order);
				//avoid logging in hot path!!! only using as an example
				spdlog::info(market_id + ": Sent closing back order on " + std::to_string(runner_id) + " of "
					+ std::to_string(bid.get_size()) + " at " + std::to_string(bid.get_price()));
			}
			
			//TODO: implement support for closing unmatched orders(if not leaving at bsp) before market closes
		}
		spdlog::info("vwap strategy finished on " + market_id );
	}

	//volume weighted average price
	float vwap(int idx) {
		float vwap = 0;
		float tot_prod = 0;
		float tot_vol = 1;
		const auto& traded = handler->traded_book(market_id , idx);
		for (int i = traded->min_range(); i <= traded->max_range(); ++i) {
			tot_prod += traded->get_price(i) * traded->get_size(i);
			tot_vol += traded->get_size(i);
		}
		return tot_prod / tot_vol;
	}

	//weight of money
	float wom(int idx) {
		float wom = 0;
		const auto& order_book = handler->order_book(market_id, idx);
		//wom from best available to back and lay prices 3 levels deep
		float total_back_size = 0;
		float total_lay_size = 0;
		for (int i = 0; i < 3; ++i) {
			total_back_size += order_book->get_back_size(i);
			total_lay_size += order_book->get_lay_size(i);
		}
		return (total_lay_size / (total_back_size + total_lay_size)) * 100;
	}

private:
	std::string market_id;
	std::vector<int> sorted_runners;
	std::queue<hedg::Order >open_orders;
	//to avoid repeating opening order consecutively
	float entry_price = 0.0;
};

int main() {
	hedg::Framework betfair;
	//use shared_ptr to initialize strategy	
	std::shared_ptr<hedg::Base_Strategy> my_strat = std::make_shared<vwap_strategy>();
	my_strat->set_country_code({ "US", "NZ" })->set_event_types({ "7","4339" });
	betfair.add_strategy(my_strat);
	//can add as many strategies as possible
	betfair.start();

}

```
6. For full api details of the library and it's functions, please go through the Documentation.

<!-- ROADMAP -->
## Roadmap
At the time of writing this, support for order streaming is yet to be implemented. 
Also support for closing open orders before market elapses is not yet implemented.
I welcome contributions towards these goals.

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.


