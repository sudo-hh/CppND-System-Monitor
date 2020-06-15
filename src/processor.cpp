#include "processor.h"
#include "linux_parser.h"

Processor::Processor() : Idle(0.0), NonIdle(0.0), Total(0.0), totald(0.0), idled(0.0), CPU_Percentage(0.0) {}

// File Format: user  nice  system  idle  iowait  irq  softirq  steal  guest  guest_nice
float Processor::Utilization() { 
    std::vector<std::string> cpu_data = LinuxParser::CpuUtilization();

    int index{1};
    float cpu_user = std::stof(cpu_data[index++]);
    float cpu_nice = std::stof(cpu_data[index++]);
    float cpu_system = std::stof(cpu_data[index++]);
    float cpu_idle = std::stof(cpu_data[index++]);
    float cpu_iowait = std::stof(cpu_data[index++]);
    float cpu_irq = std::stof(cpu_data[index++]);
    float cpu_softirq = std::stof(cpu_data[index++]);
    float cpu_steal = std::stof(cpu_data[index++]);
    float cpu_guest = std::stof(cpu_data[index++]);
    float cpu_guest_nice = std::stof(cpu_data[index++]);

    // Update user and nice time
    cpu_user -= cpu_guest;
    cpu_nice -= cpu_guest_nice;

    // Calculate Idle, NonIdle and Total times
    this->Idle = cpu_idle + cpu_iowait;
    this->NonIdle = cpu_user + cpu_nice + cpu_system + cpu_irq + cpu_softirq +cpu_steal;
    this->Total = Idle + NonIdle;

    // Calculate the change
    this->totald = Total - PrevTotal;
    this->idled = Idle - PrevIdle;

    // Final CPU percentage
    this->CPU_Percentage = (totald - idled) / totald;

    // Update the static member variables
    PrevIdle = Idle;
    PrevNonIdle = NonIdle;
    PrevTotal = PrevIdle + PrevNonIdle;

    return this->CPU_Percentage;
    
}

// Static member variables initialization 
float Processor::PrevIdle{0.0};
float Processor::PrevNonIdle{0.0};
float Processor::PrevTotal{0.0};