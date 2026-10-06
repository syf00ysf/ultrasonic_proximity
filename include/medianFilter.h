#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <algorithm>

template<typename T>
class MedianFilter{
    private:
    std::vector<T> readings;
    std::size_t window_size;

    public:
    explicit MedianFilter(std::size_t window_size) : window_size { window_size }{
        if (window_size == 0){
            throw std::invalid_argument(
                "Window size must be greater than zero"
            );
        }
    }

    T update(T reading){
        if (readings.size() >= window_size){
            readings.erase(readings.begin());
        }
        readings.push_back(reading);
        std::vector<T> readings_copy = readings;
        std::sort(readings_copy.begin(), readings_copy.end());

        std::size_t mid = readings_copy.size() / 2;
        if(readings_copy.size() % 2 == 0){
            return (readings_copy[mid-1] + readings_copy[mid] ) / 2;
        }
        else{
            return readings_copy[mid];
        }
    }

    void reset(){
        readings.clear();
    }


};