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

Process::Process(int pid) : pid(pid) {}

// TODO: Return this process's ID
int Process::Pid() {
	return pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
	long activeJiffies = LinuxParser::ActiveJiffies(pid);
	long ticksPerSecond = sysconf(_SC_CLK_TCK);
	long uptime = LinuxParser::UpTime(pid);

	if (ticksPerSecond <= 0 || uptime <=0) return 0.0f;

	float cputTime = static_cast<float>(activeJiffies) / ticksPerSecond;

	return cputTime / uptime; 
}

// TODO: Return the command that generated this process
string Process::Command() { 
	return LinuxParser::Command(pid);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
	return LinuxParser::Ram(pid);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
	return LinuxParser::User(pid); 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
	return LinuxParser::UpTime(pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
	return CpuUtilization() < a.CpuUtilization(); 
}