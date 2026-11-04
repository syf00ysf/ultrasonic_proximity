#include <cassert>
#include "ultrasonicScanner.h"

int main(){
    UltrasonicScanner scanner{5, 25.0f};

    assert(scanner.get_brake_state() == false);
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == false);
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == false);
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == true);
}