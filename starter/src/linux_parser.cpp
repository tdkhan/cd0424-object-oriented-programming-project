#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

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

// improved LinuxParser::Pids()
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

float LinuxParser::MemoryUtilization() { 
  long totalMemory = 0;
  long availableMemory = 0;
  string label, value, line;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> label >> value) {
        if (label == "MemTotal:") totalMemory = std::stol(value);
        else if (label == "MemAvailable:") availableMemory = std::stol(value);
      }
    }
  }

  if (totalMemory == 0) return 0.0f;

  return static_cast<float>(totalMemory - availableMemory) / totalMemory; 
}

long LinuxParser::UpTime() { 
  string value;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream >> value) {
    return std::stol(value);
  }  
  return 0; 
}

long LinuxParser::Jiffies() {
  auto v = CpuUtilization();
  if (v.size() <= kSteal_) return 0;
  return std::stol(v[kUser_]) + std::stol(v[kNice_]) + std::stol(v[kSystem_]) + 
        std::stol(v[kIdle_]) + std::stol(v[kIOwait_]) + std::stol(v[kIRQ_]) +
        std::stol(v[kSoftIRQ_]) + std::stol(v[kSteal_]);
}

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

long LinuxParser::ActiveJiffies() { 
  return Jiffies() - IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  auto v = CpuUtilization();
  if (v.size() <= kIOwait_) return 0;
  return std::stol(v[kIdle_]) + std::stol(v[kIOwait_]);
}

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

int LinuxParser::TotalProcesses() { 
  return static_cast<int>(LinuxParser::Pids().size());
}

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

string LinuxParser::Command(int pid) {
  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::replace(line.begin(), line.end(), '\0', ' ');
  }
  return line; 
}

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
  return "0"; 
}

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

string LinuxParser::User(int pid) { 
  string line;
  string uid = LinuxParser::Uid(pid);

  if (uid.empty()) return "";

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string firstToken, secondToken, thirdToken;
      linestream >> firstToken >> secondToken >> thirdToken;
      if (thirdToken == uid) {
        return firstToken;
      }
    }
  }
  return ""; 
}

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
