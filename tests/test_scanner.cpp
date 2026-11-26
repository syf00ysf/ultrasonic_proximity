#include <cassert>
#include "ultrasonicScanner.h"

int main(){
    UltrasonicScanner scanner{5, 25.0f};
//Initial brake state
    assert(scanner.get_brake_state() == false);

//brake activations on 3rd close reading
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == false);
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == false);
    scanner.push_reading(20.0f);
    assert(scanner.get_brake_state() == true);
//Release when fewer than three close readings remain 
    scanner.push_reading(30.0f);
    assert(scanner.get_brake_state() == true);
    scanner.push_reading(30.0f);
    assert(scanner.get_brake_state() == true);
    scanner.push_reading(30.0f);
    assert(scanner.get_brake_state() == false);
    
//Rejection of a zero threshold
    bool rejected_zero = false;

    try {
        UltrasonicScanner scanner{5, 0};
    }catch(const std::invalid_argument&){
        rejected_zero = true;
    }
    assert(rejected_zero);

//Reading exactly equal to the threshold

    UltrasonicScanner scanner1{5, 25.0f};
    scanner1.push_reading(25.0f);
    assert(scanner1.get_brake_state() == false);
    scanner1.push_reading(25.0f);
    assert(scanner1.get_brake_state() == false);
    scanner1.push_reading(25.0f);
    assert(scanner1.get_brake_state() == false);
    scanner1.push_reading(25.0f);
    assert(scanner1.get_brake_state() == false);
    scanner1.push_reading(25.0f);
    assert(scanner1.get_brake_state() == false);

//Reading three close distances that aren't consecutive

    UltrasonicScanner scanner2{5, 25.0f};
    scanner2.push_reading(20.0f);
    assert(scanner2.get_brake_state() == false);
    scanner2.push_reading(30.0f);
    assert(scanner2.get_brake_state() == false);
    scanner2.push_reading(20.0f);
    assert(scanner2.get_brake_state() == false);
    scanner2.push_reading(30.0f);
    assert(scanner2.get_brake_state() == false);
    scanner2.push_reading(20.0f);
    assert(scanner2.get_brake_state() == true);

//reset test

    UltrasonicScanner scanner3{5, 25.0f};
    scanner3.push_reading(20.0f);
    scanner3.push_reading(15.0f);
    scanner3.push_reading(22.0f);
    assert(scanner3.get_brake_state() == true);
    scanner3.reset();
    scanner3.push_reading(20.0f);
    assert(scanner3.get_brake_state() == false);

//Enter failsafe and recovery test 

    scanner3.enter_failsafe();
    assert(scanner3.get_brake_state() == true);
    assert(scanner3.is_in_failsafe() == true);
    scanner3.push_reading(30.0f);
    assert(scanner3.get_brake_state() == true);
    scanner3.push_reading(30.0f);
    assert(scanner3.get_brake_state() == true);
    scanner3.push_reading(30.0f);
    assert(scanner3.get_brake_state() == false);
    assert(scanner3.is_in_failsafe() == false);
    scanner3.reset();
    
    

//Mixed readings

    scanner3.enter_failsafe();
    scanner3.push_reading(20.0f);
    scanner3.push_reading(20.0f);
    scanner3.push_reading(30.0f);
    assert(scanner3.get_brake_state() == true);
    assert(scanner3.is_in_failsafe() == true);
    scanner3.push_reading(30.0f);
    scanner3.push_reading(30.0f);
    assert(scanner3.get_brake_state() == false);
    assert(scanner3.is_in_failsafe() == false);


}