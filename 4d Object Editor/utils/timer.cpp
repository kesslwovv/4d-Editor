//
//  timer.cpp
//  4d projection
//
//  Created by David on 01.11.22.
//

#include "timer.hpp"


std::mutex _timer_mutex;

timer* timer::getByName(const char* name)
{
//        timerLookup.try_emplace(name, timer(name));
    std::scoped_lock<std::mutex> lock(_timer_mutex);
    string sname = string(name);
    if (!timerLookup.count(sname)) {
        timerLookup[sname] = new timer(name);
    }
    return timerLookup[sname];
}

void timer::showAll()
{
    timer* arr[timerLookup.size()];
    int idx = 0;
    for (auto& element : timerLookup) {
        arr[idx] = element.second;
        idx++;
    }
    idx = 0;
    while (idx != timerLookup.size()-1)
    {
        if (idx < 0)
            idx = 0;
        if (arr[idx]->total < arr[idx+1]->total) {
            timer* t = arr[idx];
            arr[idx] = arr[idx+1];
            arr[idx+1] = t;
            idx--;
        } else {
            idx++;
        }
    }
    for (int i = 0; i < timerLookup.size(); i++) {
        arr[i]->printAvg();
    }
}

std::vector<timer*> timer::orderedTimers()
{
    std::vector<timer*> arr(timerLookup.size());
    int idx = 0;
    for (auto& element : timerLookup) {
        arr[idx] = element.second;
        idx++;
    }
    idx = 0;
    while (idx != timerLookup.size()-1)
    {
        if (idx < 0)
            idx = 0;
        if (arr[idx]->total < arr[idx+1]->total) {
            timer* t = arr[idx];
            arr[idx] = arr[idx+1];
            arr[idx+1] = t;
            idx--;
        } else {
            idx++;
        }
    }
    return arr;
}

void timer::resetAll()
{
    for (auto& element: timerLookup) {
        element.second->reset();
    }
}

uint64_t timer::getCount()
{
    return count;
}

uint64_t timer::getTotal()
{
    return total;
}

const char* timer::getName()
{
    return name;
}

timer::timer(const char* timerName)
{
    name = timerName;
//        timerLookup[string(name)] = this;
}
void timer::start()
{
//    printf("started: %s\n", name);
    lastTime = clock();
}
uint64_t timer::end()
{
    time_t now = clock();
    uint64_t dif = now - lastTime;
    total += dif;
    count++;
    return dif;
}
double timer::update()
{
    time_t now = clock();
    uint64_t dif = now - lastTime;
    total += dif;
    count++;
    lastTime = now;
    return (double)dif/CLOCKS_PER_SEC;
}
void timer::reset()
{
    total = 0;
    count = 0;
}
void timer::printAvg()
{
//        print(name, " calls: ", count, "average time:", double(total) / count / CLOCKS_PER_SEC, "total time:", double(total) / CLOCKS_PER_SEC);
    printf("%-20s calls: %10llu average time: %10e total time: %10f\n", name, (uint64_t)count, double(total) / count / CLOCKS_PER_SEC, double(total) / CLOCKS_PER_SEC);
}

#define ALLOW_TIMING 1

#if ALLOW_TIMING
#define START_TIMER(name) timer::getByName(name)->start()
#define END_TIMER(name) timer::getByName(name)->end()
#define SHOW_TIMER(name) timer::getByName(name)->printAvg()
#else
#define START_TIMER(name)
#define END_TIMER(name)
#define SHOW_TIMER(name)
#endif

