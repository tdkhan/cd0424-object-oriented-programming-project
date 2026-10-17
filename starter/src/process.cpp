#include <unistd.h>
#include <string>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;


Process::Process(int pid) : pid_(pid), cpu_(0.0f) {
	long activeJiffies = LinuxParser::ActiveJiffies(pid_);
	long ticksPerSecond = sysconf(_SC_CLK_TCK);
	long uptime = LinuxParser::UpTime(pid_);

	if (ticksPerSecond > 0 && uptime > 0) {
		cpu_ = static_cast<float>(activeJiffies) / ticksPerSecond / uptime;
	}
}

int Process::Pid() const {
	return pid_;
}

float Process::CpuUtilization() const { 
	return cpu_;
}

string Process::Command() const { 
	return LinuxParser::Command(pid_);
}

string Process::Ram() const {
	return to_string(std::stol(LinuxParser::Ram(pid_)) / 1024);
}

string Process::User() const { 
	return LinuxParser::User(pid_); 
}

long int Process::UpTime() const { 
	return LinuxParser::UpTime(pid_);
}

bool Process::operator<(Process const& a) const { 
	return cpu_ < a.cpu_; 
}