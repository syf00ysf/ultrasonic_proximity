#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

template <typename T>
class MovingAverageFilter{
    private:
    std::vector<T> readings;
    std::size_t window_size;
    T running_sum{};
    

    public:
    explicit MovingAverageFilter(std::size_t window_size) : window_size{ window_size }{
        if(window_size == 0){
            throw std::invalid_argument(
                "Window size must be greater than zero"
            );
        }
    }

    T update(T reading){
        if(readings.size() >= window_size){
            running_sum -= readings.front();
            readings.erase(readings.begin());
        }
        readings.push_back(reading);
        running_sum += reading;
        return running_sum / static_cast<T>(readings.size());
    }

    void reset(){
        readings.clear();
        running_sum = T{};
    }


};