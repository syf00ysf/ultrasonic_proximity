#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

template <typename T>

class ExponentialMovingAverageFilter{
    private:
    float alpha;
    T estimate{};
    bool initialized{false};

    public:
    explicit ExponentialMovingAverageFilter(float alpha): alpha {alpha}{
        if(alpha <= 0 || alpha > 1){
            throw std::invalid_argument(
                "Alpha must be greater than zero or less than one."
            );
        }
    }

    T update(T reading){
        if (initialized == false){
            estimate = reading;
            initialized = true;
        }
        else{
            estimate = alpha * reading + (1-alpha) * estimate;
        }
        return estimate;

    }

    void reset(){
        estimate = T{};
        initialized = false;
    }

};
