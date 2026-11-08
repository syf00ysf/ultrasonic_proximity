#include <chrono>
#include <cstdio>
#include "ultrasonicScanner.h"
#include "ultrasonicCharacterizer.h"
#include "movingAverageFilter.h"
#include "exponentialMovingAverageFilter.h"
#include "medianFilter.h"
#include "logFilter.h"
#include "logReader.h"
#include "serialReader.h"
#include <iostream>
#include <unistd.h>


int main(){
    printf("Let's go");
    printf("\n");

 
    // UltrasonicScanner a_scanner{ 5, 25.0f };
    // UltrasonicCharacterizer sonicCharacterizer;
    // CharacterizationStats stats = sonicCharacterizer.analyze_file("./Data/readings02_moving.log");
    
    // printf("Jitter_std is = %.2f\n", stats.jitter_std);
    // printf("Delta std = %.2f\n", stats.delta_std);
    // printf("Corrupted = %.2f\n", stats.corrupted_percentage);
    // printf("Max low run = %d\n", stats.max_low_run);
    // printf("Max spike run = %d\n", stats.max_spike_run);
    // printf("Reading mean = %.2f\n", stats.reading_mean);
    // printf("Reading std = %.2f\n", stats.reading_std);

    // // New file from filter
    // MovingAverageFilter<float> moving_average{10};
    // ExponentialMovingAverageFilter<float> ema{0.5f};
    // MedianFilter<float> median_filter{5};

    // filter_log(
    //     "./Data/readings02_moving.log",
    //     "./Data/readings02_moving_filtered_w10.log",
    //     moving_average
    // );
    // filter_log(
    //     "./Data/readings02_moving.log",
    //     "./Data/readings02_moving_ema_filtered_0_5.log",
    //     ema
    // );
    // ema.reset();
    // filter_log(
    //     "./Data/readings03_static50.log",
    //     "./Data/readings03_static50_ema_filtered_0_5.log",
    //     ema
    // );
    // filter_log(
    //     "./Data/readings02_moving.log",
    //     "./Data/readings02_moving_median_filtered_w5.log",
    //     median_filter
    // );
    // median_filter.reset();
    // filter_log(
    //     "./Data/readings03_static50.log",
    //     "./Data/readings03_static50_median_filtered_w5.log",
    //     median_filter
    // );

    // // synthetic data process
    // moving_average.reset();
    // ema.reset();
    // median_filter.reset();

    // filter_log(
    //     "./Data/synthetic_spike_step_100.log",
    //     "./Data/synthetic_spike_step_100_median_filtered_w5.log",
    //     median_filter
    // );
    // filter_log(
    //     "./Data/synthetic_spike_step_100.log",
    //      "./Data/synthetic_spike_step_100_moving_average_w10.log",
    //      moving_average
    // );
    // filter_log(
    //     "./Data/synthetic_spike_step_100.log",
    //     "./Data/synthetic_spike_step_100_ema_0_5.log",
    //     ema
    // );

    // //Test scanner

    // UltrasonicScanner b_scanner{ 5, 25.0f };
    // log_reader(
    //     "./Data/synthetic_spike_step_100_median_filtered_w5.log",
    //     "./Data/synthetic_scanner_decision_median_filtered_w5.log",
    //     b_scanner
    // );

    //live connection

    MedianFilter<float> live_median_filter{5};

    Measurement reading{};
    int connection = open_serial("/dev/cu.usbmodem21401");
    int i = 0;
    if(connection == -1){
            printf("no connection ");
            return 1;
        } 
    
    SerialReaderState reader_state{}; 
    auto last_valid_received = std::chrono::steady_clock::now();
    bool has_valid_reading = false;
    const auto stale_after = std::chrono::seconds(3);
    bool is_stale_state = false;

    while (i < 15){
        auto elapsed = std::chrono::steady_clock::now() - last_valid_received;
        if(elapsed >= stale_after && is_stale_state == false){
                is_stale_state = true;
                if(has_valid_reading == false){
                    std::cout << "still waiting for the first valid reading... \n";
                }else{
                    std::cout << "Too long without a valid reading... \n";
                }
            }

        ReadStatus status = read_measurement(connection, reading, reader_state);
        if (status == ReadStatus::MeasurementReady){
        
            if(reading.distance_cm > 0) {
                if(is_stale_state == true){
                    if(has_valid_reading == false){
                        std::cout << "First valid reading received... \n";
                    }
                    else{
                        live_median_filter.reset();
                        std::cout << "Valid readings resumed... \n";
                    }
                    is_stale_state = false;
                }
                
                last_valid_received = std::chrono::steady_clock::now();
                has_valid_reading = true;
                float filtered = live_median_filter.update(reading.distance_cm);
                std::cout   << reading.timestamp_ms
                            << " ms | raw: " << reading.distance_cm
                            << " cm | median: " << filtered << " cm\n";
            }else{
                std::cout << reading.timestamp_ms
                            << "ms | invalid reading: "
                            << reading.distance_cm << "\n";
            }
       
        }
        else if(status == ReadStatus::Timeout){
            continue;
        }
        else {
            if(status == ReadStatus::InvalidLine){
                std::cerr << "Serial message could not be parsed\n";
                continue;
            }else if(status == ReadStatus::Disconnected){
                std::cerr << "Serial connection closed\n";
            }else if(status == ReadStatus::Error){
                std::cerr << "Serial port read or poll error\n";
            }
            break;
        }
        i++;
    }
    close(connection);

    // auto starting_time = std::chrono::high_resolution_clock::now();

    // bool state = a_scanner.get_brake_state();
    // for(int i = 0; i < 1000000; i++){
    //     a_scanner.push_reading(50.0f);
    //     if (i == 900000){
    //         a_scanner.push_reading(24.7f);
    //         a_scanner.push_reading(23.0f);
    //         a_scanner.push_reading(20.5f);
    //         state = a_scanner.get_brake_state();
    //         if(state){
    //             printf("Object stopped!\n");
    //         }
    //         else{
    //             printf("Object still running!\n");
    //         }
    //     }
    // }

    // state = a_scanner.get_brake_state();
    // if(state){
    //     printf("Object stopped!\n");
    //     }
    // else{
    //     printf("Object still running!\n");
    //     }


    // auto end_time = std::chrono::high_resolution_clock::now();

    // std::chrono::duration<double, std::milli> duration = end_time - starting_time;
    // printf("It took : %.2fms\n", duration.count());
    
    
    

    
}