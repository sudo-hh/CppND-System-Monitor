#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

#define UPTIME_TOKEN 22
#define STARTTIME_TOKEN 22
#define UTIME_TOKEN 14

using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key, value;
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
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
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
vector<int> LinuxParser::Pids() {
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
}

float LinuxParser::MemoryUtilization() { 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    float total_used_memory{0.0};
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        total_used_memory = std::stof(value);
      } else if (key == "MemAvailable:") {
        total_used_memory = (total_used_memory - std::stof(value)) / total_used_memory;

        return total_used_memory;
      }
    }
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return std::stol(uptime);
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<string> stat_vector{};
  string stat_data;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream) {
      linestream >> stat_data;
      stat_vector.push_back(stat_data);
    }
  }
  return stat_vector;
}

int LinuxParser::TotalProcesses() { 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() { 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return std::stoi(value);
      }
    }
  } 
  return 0;
}

string LinuxParser::Command(std::string pid_dir) {
  string command;
  string line;
  std::ifstream stream(kProcDirectory + pid_dir + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line + "   \0";
  }
  return string();
}

string LinuxParser::Ram(std::string pid_dir) { 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + pid_dir + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        long int memory_KB = std::stol(value);
        long int memory_MB = memory_KB / 1000;
        return std::to_string(memory_MB);
      }
    }
  }  
  return string();
}

string LinuxParser::Uid(int pid) { return '/' + std::to_string(pid); }

string LinuxParser::User(std::string pid_dir) { 
  string line{};
  string key{}, value{};
  std::ifstream filestream(kProcDirectory + pid_dir + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return getUserName(value);
      }
    }
  } 
  return string();
}

string LinuxParser::getUserName(string Uid) {
  string line{};
  string key{}, value{}, other{};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::getline(linestream, key, ':');
      std::getline(linestream, other, ':');
      std::getline(linestream, value, ':');

      if (value.compare(Uid) == 0) {
        return key + "     \0";
      }
    }
  } 
  return string();
}

long LinuxParser::UpTime(std::string pid_dir) { 
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + pid_dir + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int token = 0; token < UPTIME_TOKEN; token++) {
      linestream >> uptime;
    }
  }
  long int uptime_number = LinuxParser::UpTime() - (std::stol(uptime)/sysconf(_SC_CLK_TCK));

  return uptime_number;
}

vector<std::string> LinuxParser::CpuUtilization(std::string pid_dir) {
  vector<string> pid_stat_vector{};
  string pid_stat_data;
  string line;
  std::ifstream stream(kProcDirectory + pid_dir + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream) {
      linestream >> pid_stat_data;
      pid_stat_vector.push_back(pid_stat_data);
    }
  }
  return pid_stat_vector;
}