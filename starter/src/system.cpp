#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::vector;

Processor& System::Cpu() { 
	return cpu_; 
}

vector<Process>& System::Processes() {
	processes_.clear();
	for (int process : LinuxParser::Pids()) {
			processes_.emplace_back(process);
		}
	std::sort(processes_.begin(), processes_.end(),
			[](const Process& a, const Process& b) {return b < a;}); 
	return processes_; 
}

std::string System::Kernel() { 
	return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
	return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
	return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() { 
	return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() { 
	return LinuxParser::TotalProcesses(); 
}

long int System::UpTime() { 
	return LinuxParser::UpTime(); 
}
