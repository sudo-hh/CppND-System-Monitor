#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int Pid) : cpu_utilization{0.0} { this->setPid(Pid); }

// Return this process's ID
int Process::Pid() const { return this->pid; }

// Sets the PID number and the PID string
void Process::setPid(int Pid) { 
    this->pid = Pid;
    this->pid_dir = LinuxParser::Uid(Pid);
}

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    std::vector<std::string> pid_stat = LinuxParser::CpuUtilization(this->pid_dir);
    int index{13};
    try {
        long utime_number = std::stol(pid_stat[index++]);
        long stime_number = std::stol(pid_stat[index++]);
        long cutime_number = std::stol(pid_stat[index++]);
        long cstime_number = std::stol(pid_stat[index++]);
        long starttime_number = std::stol(pid_stat[21]);

        float total_pid_time = (utime_number + stime_number + cutime_number + cstime_number) / sysconf(_SC_CLK_TCK);
        float seconds = LinuxParser::UpTime() - (starttime_number / sysconf(_SC_CLK_TCK));

        this->cpu_utilization = (total_pid_time / seconds);

        return this->cpu_utilization;

    } catch (...) {
        this->cpu_utilization = 0.0;
        return this->cpu_utilization;
    }
}

string Process::Command() { return LinuxParser::Command(this->pid_dir); }

string Process::Ram() { return LinuxParser::Ram(this->pid_dir); }

string Process::User() { return LinuxParser::User(this->pid_dir); }

long int Process::UpTime() { return LinuxParser::UpTime(this->pid_dir); }

bool Process::operator>(Process const& a) const { 
    return (this->cpu_utilization -  a.cpu_utilization) > 0.0005; // Bigger by ~0.05 %
}