#include "serialReader.h"
#include <fcntl.h>
#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <cerrno>
#include <sstream>


int open_serial(const char* port){
    int connection = open(port, O_RDWR | O_NOCTTY);

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
    return connection;

}

bool read_measurement(int connection, Measurement& result){
    char byte;
    std::string line;

    while(true){
        ssize_t count = read(connection, &byte, 1);

        if(count == -1){
            if(errno ==  EINTR){
                continue;
            }
            perror("Could not read serial port");
            return false;
        }
        if (count == 0){
            return false;
        }
        if (byte == '\n'){
            break;
        }
        if (byte != '\r'){
            line += byte;
        }
    }

    std::istringstream parser(line);
    Measurement parsed {};

    if(!(parser >> parsed.timestamp_ms >> parsed.distance_cm)) {
        return false;
    }

    parser >> std::ws;

    if(!parser.eof()) return false;
    
    result = parsed;
    return true;
}