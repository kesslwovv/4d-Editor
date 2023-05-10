//
//  timer.hpp
//  4d projection
//
//  Created by David on 01.11.22.
//

#ifndef timer_hpp
#define timer_hpp

#include <time.h>
#include <string>
#include <vector>
#include <unordered_map>

//#include <thread>
#include <mutex>
#include <atomic>

using std::string;

#ifndef ALLOW_TIMING
#define ALLOW_TIMING 1
#endif

#if ALLOW_TIMING
#define START_TIMER(name) timer::getByName(name)->start()
#define END_TIMER(name) timer::getByName(name)->end()
#define SHOW_TIMER(name) timer::getByName(name)->printAvg()
#else
#define START_TIMER(name)
#define END_TIMER(name)
#define SHOW_TIMER(name)
#endif


class timer
{
public:
    static timer* getByName(const char* name);
    static void showAll();
    static void resetAll();
//    static string getToShow();
    static std::vector<timer*> orderedTimers();
    
    
    timer(const char* timerName);
    void printAvg();
    uint64_t getCount();
    uint64_t getTotal();
    const char* getName();
    void start();
    uint64_t end();
    void reset();
    double update();
private:
    std::atomic<uint64_t> total = 0;
    std::atomic<uint64_t>  count = 0;
    std::atomic<time_t> lastTime;
    const char* name;
};

// lookup for timers invoked by macros defined above
static std::unordered_map<string, timer*> timerLookup;



#endif /* timer_hpp */
