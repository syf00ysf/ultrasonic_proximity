#pragma once

#include <cstddef>
#include <string>
#include <fstream>
#include <iostream>

template <typename Filter>
void filter_log(
    const std::string& input_path, 
    const std::string& output_path,
    Filter& filter

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

    float timestamp;
    float reading;

    while(input >> timestamp >> reading){
        float filtered_distance = filter.update(reading);

        output << timestamp << ' '
               << filtered_distance <<'\n';
    }
}