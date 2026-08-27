#include "logFilter.h"
#include "movingAverageFilter.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

void filter_log(
    const std::string& input_path, 
    const std::string& output_path,
    std::size_t window_size
){
    std::ifstream input(input_path);
    std::ofstream output(output_path);

    if(!input.is_open()){
        std::cerr << "Could not open input file\n";
        return;
    }
    if(!output.is_open()){
        std::cerr << "Could not create output file\n";
        return;
    }

    MovingAverageFilter<float> filter{window_size};
    float timestamp;
    float reading;

    output << "Timestamp raw_distance filtered_distance\n";

    while(input >> timestamp >> reading){
        float filtered_distance = filter.update(reading);

        output << timestamp << ' '
               << reading   << ' '
               << filtered_distance <<'\n';
    }
}