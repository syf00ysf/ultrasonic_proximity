#include "serialReader.h"
#include <fcntl.h>
#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <cerrno>
#include <sstream>
#include <chrono>
#include <poll.h>
#include <iostream>
#include <iomanip>


int open_serial(const char* port){
    int connection = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);

    termios settings{};

    if(connection == -1){
        perror("Could not open serial port");
        return -1;
    }

    if(tcgetattr(connection, &settings) == -1){
        perror("Could not read serial settings");
        close(connection);
        return -1;
    }

    if(cfsetispeed(&settings, B9600) == -1 || cfsetospeed(&settings, B9600) == -1){
        perror("Could not set serial speed");
        close(connection);
        return -1;
    }

    cfmakeraw(&settings);
    settings.c_cflag |= CLOCAL | CREAD;
    settings.c_cc[VMIN] = 1;
    settings.c_cc[VTIME] = 0;
    settings.c_cflag &= ~(CSTOPB | CRTSCTS);

    if (tcsetattr(connection, TCSANOW, &settings) == -1){
        perror("Could not apply serial settings");
        close(connection);
        return -1;
    }

    // Allow startup to settle, then discard queued input.
sleep(1);

if (tcflush(connection, TCIFLUSH) == -1) {
    perror("Could not flush serial input");
    close(connection);
    return -1;
}

    return connection;

}

ReadStatus read_measurement(int connection, Measurement& result, SerialReaderState& state){
    char byte;
    std::string& line = state.pending_line;

    const auto deadline = std::chrono::steady_clock::now()
                                + std::chrono::milliseconds(250);
    //creating poll
    pollfd port{};
    port.fd = connection;
    port.events = POLLIN;

    while(true){
        const auto now = std::chrono::steady_clock::now();
        if(now >= deadline){
            return ReadStatus::Timeout;
        }
        //Time reamining
        const auto remaining = std::chrono::ceil<std::chrono::milliseconds>( deadline - now );

        int poll_result = poll(
            &port, 
            1,
            static_cast<int>(remaining.count())
        );

        //Poll returns 0 or negative

        if(poll_result == 0){
            return ReadStatus::Timeout;
        }
        if(poll_result == -1){
            if(errno == EINTR){
                continue;
            }
            perror("Could not poll serial port");
            return ReadStatus::Error;
        }
        //Poll returns positive
        if(port.revents & (POLLERR | POLLNVAL)){
            return ReadStatus::Error;
        }
        if(!(port.revents & POLLIN)){
            if(port.revents & POLLHUP){
                return ReadStatus::Disconnected;
            }
            continue;
        }

        ssize_t count = read(connection, &byte, 1);

        if(count == -1){
            if(errno ==  EINTR){
                continue;
            }
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }
            perror("Could not read serial port");
            return ReadStatus::Error;
        }
        if (count == 0){
            return ReadStatus::Disconnected;
        }

        if (byte == '\n'){
            break;
        }
        if (byte != '\r'){
            line += byte;
        }
    }

    std::istringstream parser(line);
    line.clear();
    Measurement parsed {};

    if(!(parser >> parsed.timestamp_ms >> parsed.distance_cm)) {
        std::cerr << "Invalid fields in: "
                  << std::quoted(parser.str()) <<'\n';
        return ReadStatus::InvalidLine;
    }

    parser >> std::ws;

    if(!parser.eof()){ 
        std::cerr << "Unexpected extra text in: "
                  << std::quoted(parser.str()) << '\n';
        return ReadStatus::InvalidLine;
    }

    result = parsed;
    return ReadStatus::MeasurementReady;
}