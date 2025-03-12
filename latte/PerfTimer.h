#pragma once
#include <chrono>
#include <iostream>
#include <string>

class PerfTimer {
    std::string mName;
    std::chrono::time_point<std::chrono::high_resolution_clock> mStart;



public:

    PerfTimer(std::string name) : mName(name) {
        mStart = std::chrono::high_resolution_clock::now();
    }

    ~PerfTimer() {
        std::chrono::duration<double, std::milli> duration = std::chrono::high_resolution_clock::now() - mStart;
        std::cout << "PerfTimer [" << mName << "] " << duration.count() << "ms" << std::endl;


    }


};
