#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
    Processor();
    float Utilization();
    
 private:
    static float PrevIdle, PrevNonIdle, PrevTotal;
    float Idle, NonIdle, Total;
    float totald, idled;
    float CPU_Percentage;
};


#endif