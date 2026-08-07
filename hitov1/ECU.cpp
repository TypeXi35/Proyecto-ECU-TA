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

int main()
{  
    State current_state = State::INIT;
    switch (current_state)
    {
    case State::INIT:
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