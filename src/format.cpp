#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

#define HOUR 3600
#define MINUTE 60

using std::string;

string Format::ElapsedTime(long seconds) {
    
    int hours = seconds / HOUR;
    int sec = seconds % HOUR;
    int minutes = sec / MINUTE;
    sec = sec % MINUTE;
    
    std::stringstream HH, MM, SS;
    HH << std::setfill('0') << std::setw(2) << std::to_string(hours);
    MM << std::setfill('0') << std::setw(2) << std::to_string(minutes);
    SS << std::setfill('0') << std::setw(2) << std::to_string(sec);
    string time =  HH.str() + ":" + MM.str() + ":" + SS.str();

    return time;
    
}