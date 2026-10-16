#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
/*string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}*/

// improved LinuxParser::OperatingSystem()
string LinuxParser::OperatingSystem() {
  string line;
  std::ifstream filestream(kOSPath);

  while (std::getline(filestream, line)) {
    const string key = "PRETTY_NAME=";

    if (line.rfind(key, 0) == 0) {
      string value = line.substr(key.size());

      if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
      }
      return value;
    }
  }
  return "";
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
/*vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}*/

// Updates LinuxParser::Pids()
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  std::filesystem::path proc_path{kProcDirectory};

  /*loop over entries in the proc_dir 
  see which entries are directories
  which folders have a name that are digits
  convert them to int and push_back to pid vector*/

  for (const auto& entry : std::filesystem::directory_iterator(proc_path)) {
      if (!entry.is_directory())
        continue;

      string dir_name = entry.path().filename().string();

      if (dir_name.empty())
        continue;

      if (std::all_of(dir_name.begin(),
       dir_name.end(), 
       [](unsigned char c){
         return std::isdigit(c);
        })) 
      {
        int pid = stoi(dir_name);
        pids.push_back(pid);
      }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  long totalMemory = 0;
  long freeMemory = 0;
  string label, value, line;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> label >> value) {
        if (label == "MemTotal:") totalMemory = std::stol(value);
        if (label == "MemFree:") freeMemory = std::stol(value);
      }
    }
  }

  if (totalMemory == 0) return 0.0f;

  return static_cast<float>(totalMemory - freeMemory) / totalMemory; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string value;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream >> value) {
    return std::stol(value);
  }  
  return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line, cpu;
  long jiffies = 0;
  long value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 8 && linestream >> value; ++i) {
        jiffies += value;
      }
  }
  return jiffies; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long utime = 0, stime = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    size_t pos = line.rfind(')');
    if (pos != string::npos) {
      string substr = line.substr(pos+2);
      std::istringstream linestream(substr);
    
      string state;

      for (int i = 0; i < 11; ++i) linestream >> state;

      if (linestream >> utime >> stime) {
        return utime + stime;
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line, cpu;
  long activeJiffies = 0;
  long value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 8 && linestream >> value; ++i) {
      if (i != 3 && i != 4) activeJiffies += value;
    }
  }
  return activeJiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line, cpu;
  long idleJiffies = 0;
  long value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 3 && linestream >> value; ++i) {
      if (!(linestream >> value)) return 0;
    }
    if (linestream >> value) idleJiffies += value;
  }
  return idleJiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, token;
  vector<string> cpuUtils {};

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> token;
    while (linestream >> token) cpuUtils.push_back(token);
  }
  return cpuUtils;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  return static_cast<int>(LinuxParser::Pids().size());
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, firstToken;
  int runningProcs = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> firstToken;
      if (firstToken == "procs_running") {
        linestream >> runningProcs;
        break;
      }
    }
  }
  return runningProcs;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::replace(line.begin(), line.end(), '\0', ' ');
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, firstToken, ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> firstToken;
      if (firstToken == "VmRSS:") {
        linestream >> ram;
        return ram;
      }
    }
  }
  return ""; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, firstToken, uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> firstToken;
      if (firstToken == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }
  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, firstToken, secondToken, thirdToken;
  string uid = LinuxParser::Uid(pid);

  if (uid.empty()) return "";

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> firstToken >> secondToken >> thirdToken;
      if (thirdToken == uid) {
        return firstToken;
      }
    }
  }
  return ""; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  long processUptime = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      size_t pos = line.rfind(')');
      if (pos != string::npos) {
        string substr = line.substr(pos+2);
        std::istringstream linestream(substr);
      
        for (int i = 0; i < 20; ++i) {
          if (!(linestream >> value)) {
            return 0;
          }
        }

        long ticksPerSecond = sysconf(_SC_CLK_TCK);

        if (ticksPerSecond <= 0) {
            return 0;
        }

        long startTime = std::stol(value) / ticksPerSecond;

        long systemUptime = LinuxParser::UpTime();
        processUptime = systemUptime - startTime;

        if (processUptime < 0) {
            return 0;
        }
      }
    }
  }
  return processUptime;
}
