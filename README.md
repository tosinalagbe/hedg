[![Build Status](https://app.travis-ci.com/tosinalagbe/hedg.svg?branch=main)](https://app.travis-ci.com/tosinalagbe/hedg)

# hedg
Fast and easy to use, high frequency trading framework for betfair

<!-- ABOUT THE PROJECT -->
## About Hedg
In the sports trading industry, low latency is really important. The need to be first in line on the queue as well as the need to be able to cancel or close out bad orders quickly are some of the latency sensitive needs involved. Hedg was built with these in mind. Also enables quants and automated traders who want to use efficient c++ libraries to build strategies that are latency sensitive.

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






