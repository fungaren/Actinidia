#pragma once
#include <thread>
#include <chrono>

class Timer
{
    bool canceled;
    std::thread timer;
public:
    Timer() : canceled(true) {};
    void begin(std::chrono::milliseconds interval, std::function<void()> callback)
    {
        if (!canceled)
            end();
        canceled = false;
        timer = std::thread([](const Timer* timer,
            std::chrono::milliseconds interval,
            std::function<void()> callback)
        {
            while (!timer->canceled) {
                std::this_thread::sleep_for(interval);
                callback();
            }
        }, this, interval, callback);
    }
    void end()
    {
        canceled = true;
        if (timer.joinable())
            timer.join();
    }
    ~Timer()
    {
        if (!canceled)
            end();
    }
};
