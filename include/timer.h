#ifndef FLASHTOKENIZER_TIMER_H
#define FLASHTOKENIZER_TIMER_H

#include <chrono>
#include <string>
#include <unordered_map>

struct TimerData {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    double accu = 0.0;
};

class Timer {
private:
    static std::unordered_map<std::string, TimerData> timers;

public:
    static void Tick(const std::string& name) {
        timers[name].start = std::chrono::high_resolution_clock::now();
    }

    static void Tock(const std::string& name) {
        timers[name].end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = timers[name].end - timers[name].start;
        timers[name].accu += diff.count();
    }

    static TimerData& Watch(const std::string& name) {
        return timers[name];
    }
};

std::unordered_map<std::string, TimerData> Timer::timers;

#endif // FLASHTOKENIZER_TIMER_H
