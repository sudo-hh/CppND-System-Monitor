#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int Pid);
  int Pid() const;                         
  std::string User();                      
  std::string Command();                  
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator>(Process const& a) const;  

 private:
  void setPid(int Pid);
  int pid;
  float cpu_utilization;
  std::string pid_dir;
};

#endif