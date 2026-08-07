#include <iostream>

enum class State
{
    INIT,
    SELF_TEST,
    OPERATIONAL,
    DEGRADED,
    SAFE_STATE,
    SHUTDOWN,
};

int rpm;
float speed, temperature, voltage, throttle;

int main()
{  
    State current_state = State::INIT;
    switch (current_state)
    {
    case State::INIT:
        rpm = 0;
        speed = 0.0;
        temperature = 0.0;
        voltage = 0.0;
        throttle = 0.0;

        break;
    case State::SELF_TEST:
        break;
    case State::OPERATIONAL:
        break;
    case State::DEGRADED:
        break;
    case State::SAFE_STATE:
        break;
    case State::SHUTDOWN:   
        break;
    }
}