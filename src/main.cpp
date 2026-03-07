#include <chrono>
#include <cstdio>
#include "lidarScanner.h"



int main(){
    printf("Let's go");
    printf("\n");


    LidarScanner a_scanner{ 5 };

    auto starting_time = std::chrono::high_resolution_clock::now();

    bool state = a_scanner.get_brake_state();
    for(int i = 0; i < 1000000; i++){
        a_scanner.lidar_reading(3.2f);
        if (i == 900000){
            a_scanner.lidar_reading(1.2f);
            a_scanner.lidar_reading(1.2f);
            a_scanner.lidar_reading(1.2f);
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