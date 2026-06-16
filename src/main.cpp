#include <chrono>
#include <cstdio>
#include "ultrasonicScanner.h"
#include <fstream>
#include <iostream>



int main(){
    std::ifstream logFile("./Data/readings01.log");
    if (!logFile.is_open()){
        std::cerr << "Error: could not open the log file." <<std::endl;
        return 1;
    }
    int greater = 0;
    int current = 0;
    while (logFile >> current) {
        // Process the line (e.g., print it, parse it, filter for errors)
        if (current > 400) greater++;

    }
    logFile.close();
    printf("Greater than 400 is: %d\n", greater);


    printf("Let's go");
    printf("\n");

 
    UltrasonicScanner a_scanner{ 5 };

    auto starting_time = std::chrono::high_resolution_clock::now();

    bool state = a_scanner.get_brake_state();
    for(int i = 0; i < 1000000; i++){
        a_scanner.push_reading(3.2f);
        if (i == 900000){
            a_scanner.push_reading(1.2f);
            a_scanner.push_reading(1.2f);
            a_scanner.push_reading(1.2f);
            state = a_scanner.get_brake_state();
            if(state){
                printf("Object stopped!\n");
            }
            else{
                printf("Object still running!\n");
            }
        }
    }

    state = a_scanner.get_brake_state();
    if(state){
        printf("Object stopped!\n");
        }
    else{
        printf("Object still running!\n");
        }


    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end_time - starting_time;
    printf("It took : %.2fms\n", duration.count());
    
    
    

    
}