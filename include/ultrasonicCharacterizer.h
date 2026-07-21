#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>


struct CharacterizationStats {
    int max_low_run = 0;
    int max_spike_run = 0;
    float corrupted_percentage = 0.0f;
    float jitter_std = 0.0f;
    float delta_std = 0.0f;
    float reading_std = 0.0f;
    float reading_mean = 0.0f;
};

class UltrasonicCharacterizer{
    private:
    static constexpr int SENSOR_TIMEOUT_VALUE = 798;
    static constexpr int SENSOR_LOW_VALUE = 10;

    public:

    CharacterizationStats analyze_file(const std::string& path){
        CharacterizationStats stats;

        std::ifstream logFile(path);
        if(!logFile.is_open()){
            std::cerr << "Error: could not open the log file." <<std::endl;
            return stats;
        }

        float timestamp;
        float timestamp_next;
        int reading = 0;
        int reading_next = 0;
        int total_reading = 0;
        int spike_total = 0;
        int current_low_run = 0;
        int max_low_run = 0;
        int current_spike_run = 0;
        int max_spike_run = 0;
        float sum_timestamp_diff = 0.0f;
        float sum_reading_diff = 0.0f;
        int sum_reading = 0;
        float mean_reading = 0.0f;
        float mean_timestamp = 0.0f;
        float delta_std = 0.0f;
        float sum_dist_vect_diff = 0.0f;


        std::vector<int> distance_diff;
        std::vector<float> time_diff;
        std::vector<int> dist_vect;

        if (!(logFile >> timestamp >> reading)) return stats;
        total_reading++;
        

        if(reading >= SENSOR_TIMEOUT_VALUE ){
            spike_total++;
            current_spike_run++;
        } 
        else{
            dist_vect.push_back(reading);
            sum_reading += reading;
        }
        if(reading < SENSOR_LOW_VALUE) {
            current_low_run++;
        }

        while (logFile >> timestamp_next >> reading_next){
            total_reading++;

            if(reading_next >= SENSOR_TIMEOUT_VALUE ) spike_total++;
            if(reading_next < SENSOR_LOW_VALUE) current_low_run++;
            if(reading_next >= SENSOR_LOW_VALUE){
                if(current_low_run > max_low_run){
                    max_low_run = current_low_run;
                }
                current_low_run = 0;
            }
            if(reading_next >= SENSOR_TIMEOUT_VALUE) current_spike_run++;
            if(reading_next < SENSOR_TIMEOUT_VALUE){
                if(current_spike_run > max_spike_run){
                    max_spike_run = current_spike_run;
                }
                current_spike_run = 0;
            }

            if(reading_next < SENSOR_TIMEOUT_VALUE){
                dist_vect.push_back(reading_next);
                sum_reading += reading_next;
            }
            
            if(reading_next < SENSOR_TIMEOUT_VALUE && reading < SENSOR_TIMEOUT_VALUE){
                distance_diff.push_back(reading_next - reading);
                sum_reading_diff += (reading_next - reading);
            }
            time_diff.push_back(timestamp_next - timestamp);
            sum_timestamp_diff += (timestamp_next - timestamp);


            reading = reading_next;
            timestamp =  timestamp_next;

            
        }

        if(current_low_run > max_low_run){
            max_low_run = current_low_run;
        }
        if(current_spike_run > max_spike_run){
            max_spike_run = current_spike_run;
        }

        stats.max_low_run   = max_low_run;
        stats.max_spike_run = max_spike_run;

        if (total_reading == 0){
            return stats;
        }
        stats.corrupted_percentage = (1.0f * spike_total / total_reading) * 100;

        //
        if(!dist_vect.empty()){
            stats.reading_mean = 1.0*sum_reading / dist_vect.size();

            for(size_t i = 0; i < dist_vect.size(); i++){
            sum_dist_vect_diff += std::pow(dist_vect[i] - stats.reading_mean, 2);
        }
            stats.reading_std = std::sqrt(sum_dist_vect_diff/dist_vect.size());
        }
        
        
        //
        if (distance_diff.size() == 0){
            return stats;
        }
        mean_reading = sum_reading_diff / distance_diff.size();
        float sum_mean_reading = 0.0f;

        for(size_t i = 0; i < distance_diff.size(); i++){
            sum_mean_reading += std::pow((distance_diff[i] - mean_reading), 2); 
        }

        delta_std = std::sqrt(sum_mean_reading/distance_diff.size());
        stats.delta_std = delta_std;

        //
        if (time_diff.size() == 0) return stats;
        mean_timestamp = sum_timestamp_diff / time_diff.size();
        float sum_mean_timestamp = 0.0f;

        for(size_t i = 0; i < time_diff.size(); i++){
            sum_mean_timestamp += std::pow(time_diff[i] - mean_timestamp, 2);
        }
        stats.jitter_std = std::sqrt(sum_mean_timestamp/time_diff.size());

        return stats;

    }
};