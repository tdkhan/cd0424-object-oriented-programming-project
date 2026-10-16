#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
	
	long hours = seconds / 3600;
	long minutes = (seconds % 3600) / 60;
	long remainingSeconds = seconds % 60;

	std::ostringstream time;
	time << std::setfill('0')
		<< std::setw(2) << hours << ":"
		<< std::setw(2) << minutes << ":"
		<< std::setw(2) << remainingSeconds;

	return time.str(); 
}