#pragma once
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

class Timer
{
    bool canceled;
    std::thread timer;
public:
    Timer() : canceled(true) {};
    /**
     * @param interval The interval (milliseconds) of callback to invoke. 
     * @param callback The callback function will be called continuously.
     * @param delay Delay some milliseconds then start the timer.
     */
    void begin(
        std::chrono::milliseconds interval,
        std::function<void()> callback,
        std::chrono::milliseconds delay=0ms
        )
    {
        if (!canceled)
            end();
        canceled = false;
        std::this_thread::sleep_for(delay);
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
    /**
     * Stop the timer.
     */
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
