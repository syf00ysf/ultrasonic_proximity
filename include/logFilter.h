#pragma once

#include <cstddef>
#include <string>

void filter_log(
    const std::string& input_path, 
    const std::string& output_path,
    std::size_t window_size
);