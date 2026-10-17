#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid), cpu_(0.0f) {
	long activeJiffies = LinuxParser::ActiveJiffies(pid_);
	long ticksPerSecond = sysconf(_SC_CLK_TCK);
	long uptime = LinuxParser::UpTime(pid_);

	if (ticksPerSecond > 0 && uptime > 0) {
		cpu_ = static_cast<float>(activeJiffies) / ticksPerSecond / uptime;
	}
}

// TODO: Return this process's ID
int Process::Pid() const {
	return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
	return cpu_;
}

// TODO: Return the command that generated this process
string Process::Command() const { 
	return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() const {
	return to_string(std::stol(LinuxParser::Ram(pid_)) / 1024);
}

// TODO: Return the user (name) that generated this process
string Process::User() const { 
	return LinuxParser::User(pid_); 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { 
	return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
	return cpu_ < a.cpu_; 
}