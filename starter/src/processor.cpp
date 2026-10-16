#include "processor.h"
#include "linux_parser.h"

Processor::Processor() : previousActive(linux_parser::ActiveJiffies()),
						previousIdle(linux_parser::IdleJiffies()) {}
						
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
	long currentActive = linux_parser::ActiveJiffies();
	long currentIdle = linux_parser::IdleJiffies();

	long activeDelta = currentActive - previousActive;
	long idleDelta = currentIdle - previousIdle;

	previousActive = currentActive;
	previousIdle = currentIdle;

	long totalDelta = activeDelta + idleDelta;

	if (totalDelta <= 0) return 0.0f;

	return static_cast<float>(activeDelta) / totalDelta;
}