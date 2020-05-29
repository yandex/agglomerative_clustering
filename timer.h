#pragma once

#include <chrono>
#include <string>
#include <iostream>

class TTimer {
private:
    using TTimePoint = std::chrono::high_resolution_clock::time_point;

    std::string FrameName;
    TTimePoint FrameStart;

    std::ostream& Out;
public:
    TTimer(std::ostream& out)
        : Out(out)
    {
    }

    void Reset(const std::string& name) {
        FrameName = name;
        FrameStart = std::chrono::high_resolution_clock::now();
    }

    double Duration() const {
        const TTimePoint now = std::chrono::high_resolution_clock::now();
        const size_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - FrameStart).count();
        return (double) milliseconds / 1000;
    }

    void Report() {
        Out << FrameName << ": finished in " << Duration() << "s" << std::endl;
    }

    void ReportAndReset(const std::string& newName) {
        Report();
        Reset(newName);
    }
};
