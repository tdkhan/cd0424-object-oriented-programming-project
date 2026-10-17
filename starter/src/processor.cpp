#include "processor.h"
#include "linux_parser.h"

Processor::Processor() : previousActive(LinuxParser::ActiveJiffies()),
						previousIdle(LinuxParser::IdleJiffies()) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
	long currentActive = LinuxParser::ActiveJiffies();
	long currentIdle = LinuxParser::IdleJiffies();

	long activeDelta = currentActive - previousActive;
	long idleDelta = currentIdle - previousIdle;

	previousActive = currentActive;
	previousIdle = currentIdle;

	long totalDelta = activeDelta + idleDelta;

	if (totalDelta <= 0) return 0.0f;

	return static_cast<float>(activeDelta) / totalDelta;
}