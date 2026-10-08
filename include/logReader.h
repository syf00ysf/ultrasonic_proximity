#pragma once

#include <cstddef>
#include <string>
#include <fstream>
#include <iostream>
#include <ultrasonicScanner.h>


inline void log_reader(
    const std::string& input_path,
    const std::string& output_path,
    UltrasonicScanner& scanner
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
    float distance_cm;

    while(input >> timestamp >> distance_cm){
        scanner.push_reading(distance_cm);

        output  << timestamp << ' '
                << distance_cm  << ' '
                << scanner.get_brake_state() <<'\n';
    }

}