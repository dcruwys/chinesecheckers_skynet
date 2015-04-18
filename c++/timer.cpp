#include <chrono>
#include <thread>
namespace timer
{
	auto duration = std::chrono::milliseconds(10000 - 200); //10s
	bool timeUp;
	void startTimer(){
		bool timeUp = false;
		auto t = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });		
	}
	bool timeUp(){
		if(&timeUp == true){
			return true;
		}
		return false;
	}
};
