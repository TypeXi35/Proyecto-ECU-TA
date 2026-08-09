#include <iostream>

using std::cin;
using std::cout;
using std::endl;

enum class State
{
    INIT,
    SELF_TEST,
    OPERATIONAL,
    DEGRADED,
    SAFE_STATE,
    SHUTDOWN,
};

enum class AttributeState
{
    OK,
    DEGRADED,
    CRITICAL
};

const int MIN_RPM = 0;
const int MAX_RPM = 8000;

const float MIN_SPEED = 0.0;
const float MAX_SPEED = 250.0;

const float MIN_TEMPERATURE = -40.0;
const float MAX_TEMPERATURE = 150.0;

const float MIN_VOLTAGE = 9.0;
const float MAX_VOLTAGE = 16.0;

const float MIN_THROTTLE = 0.0;
const float MAX_THROTTLE = 100.0;

const float MAX_RPM_JUMP = 4000;
const float MAX_SPEED_JUMP = 100.0;
const float MAX_TEMPERATURE_JUMP = 20.0;
const float MAX_VOLTAGE_JUMP = 2.0;
// No throttle Jump since it is possible to go from 0 to 100 almost instantly

const float DEGRADED_TEMPERATURE = 105.0;
const float DEGRADED_VOLTAGE = 11.5;

const float CRITICAL_TEMPERATURE = 115.0;
const float CRITICAL_VOLTAGE = 10.5;

bool isRpmValid(int rpm)
{
    if (rpm >= MIN_RPM && rpm <= MAX_RPM)
    {
        cout << "Valid RPM" << endl;
        return true;
    }
    return false;
}
bool isSpeedValid(float speed)
{
    if (speed >= MIN_SPEED && speed <= MAX_SPEED)
    {
        cout << "Valid Speed" << endl;
        return true;
    }
    return false;
}

bool isTemperatureValid(int temperature)
{
    if (temperature >= MIN_TEMPERATURE && temperature <= MAX_TEMPERATURE)
    {
        cout << "Valid Temperature" << endl;
        return true;
    }
    return false;
}

bool isVoltageValid(int voltage)
{
    if (voltage >= MIN_VOLTAGE && voltage <= MAX_VOLTAGE)
    {
        cout << "Valid Voltage" << endl;
        return true;
    }
    return false;
}

bool isThrottleValid(int throttle)
{
    if (throttle >= MIN_THROTTLE && throttle <= MAX_THROTTLE)
    {
        cout << "Valid Throttle" << endl;
        return true;
    }
    return false;
}

bool areCriticalValuesMissing(bool valid_voltage, bool valid_temperature)
{
    if (!valid_voltage || !valid_temperature)
    {
        cout << "Critical Values are Missing" << endl;
        return true;
    }
    else
    {
        cout << "Critical Values are valid" << endl;
        return false;
    }
}

bool inconsistentMotion(int rpm, float speed)
{
    if (speed > 0.0 && rpm == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool inconsistentThrottle(int rpm, float throttle)
{
    if (throttle > 80.0 && rpm < 500)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isInconsistentMotionPossible(bool valid_rpm, bool valid_speed)
{
    if (valid_rpm && valid_speed)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isInconsistentThrottlePossible(bool valid_rpm, bool valid_throttle)
{
    if (valid_rpm && valid_throttle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int areNormalValuesMissing(bool valid_rpm, bool valid_speed, bool valid_throttle)
{
    int invalid_count = 0;
    if (!valid_rpm)
    {
        invalid_count++;
    }
    if (!valid_speed)
    {
        invalid_count++;
    }
    if (!valid_throttle)
    {
        invalid_count++;
    }
    cout << invalid_count << " Values are invalid" << endl;
    return invalid_count;
}

AttributeState currentTemperatureState(float temperature)
{
    if (temperature <= DEGRADED_TEMPERATURE)
    {
        return AttributeState::OK;
    }
    else if (temperature <= CRITICAL_TEMPERATURE)
    {
        return AttributeState::DEGRADED;
    }
    else
    {
        return AttributeState::CRITICAL;
    }
}

AttributeState currentVoltageState(float voltage)
{
    if (voltage >= DEGRADED_VOLTAGE)
    {
        return AttributeState::OK;
    }
    else if (voltage >= CRITICAL_VOLTAGE)
    {
        return AttributeState::DEGRADED;
    }
    else
    {
        return AttributeState::CRITICAL;
    }
}

void printCurrentState(State current_state)
{
    switch (current_state)
    {
    case State::INIT:
        cout << "Your Current State is INIT" << endl;
        break;
    case State::SELF_TEST:
        cout << "Your Current State is SELF_TEST" << endl;
        break;
    case State::OPERATIONAL:
        cout << "Your Current State is OPERATIONAL" << endl;
        break;
    case State::DEGRADED:
        cout << "Your Current State is DEGRADED" << endl;
        break;
    case State::SAFE_STATE:
        cout << "Your Current State is SAFE_STATE" << endl;
        break;
    case State::SHUTDOWN:
        cout << "Your Current State is SHUTDOWN" << endl;
        break;
    }
}

int main()
{
    int rpm, runSystem;
    float speed, temperature, voltage, throttle;
    bool valid_rpm, valid_speed, valid_temperature, valid_voltage, valid_throttle;
    AttributeState voltage_state, temperature_state;

    State current_state = State::INIT;

    runSystem = 1;
    while (runSystem)
    {
        switch (current_state)
        {
        case State::INIT:
            printCurrentState(current_state);
            cout << "Initializing system configuration..." << endl;

            rpm = 1200;
            speed = 0.0;
            temperature = 25.0;
            voltage = 12.0;
            throttle = 0.0;

            cout << "Configured system!" << endl;
            current_state = State::SELF_TEST;
            break;

        case State::SELF_TEST:
            printCurrentState(current_state);
            cout << "Running initial Self-Test" << endl;

            valid_temperature = isTemperatureValid(temperature);
            valid_voltage = isVoltageValid(voltage);

            if (areCriticalValuesMissing(valid_voltage, valid_temperature))
            {
                current_state = State::SAFE_STATE;
                cout << "Critical Values missing, moving to SAFE_STATE" << endl;
                break;
            }

            valid_rpm = isRpmValid(rpm);
            valid_speed = isSpeedValid(speed);
            valid_throttle = isThrottleValid(throttle);

            if (areNormalValuesMissing(valid_rpm, valid_speed, valid_throttle) >= 2)
            {
                current_state = State::SAFE_STATE;
                cout << "2 or more values are missing, moving to SAFE_STATE" << endl;
                break;
            }

            temperature_state = currentTemperatureState(temperature);

            if (temperature_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Temperature State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            voltage_state = currentVoltageState(voltage);

            if (voltage_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Voltage State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            if (temperature_state == AttributeState::DEGRADED)
            {
                current_state = State::DEGRADED;
                cout << "Temperature State is DEGRADED, moving to DEGRADED" << endl;
                break;
            }

            if (voltage_state == AttributeState::DEGRADED)
            {
                current_state = State::DEGRADED;
                cout << "Voltage State is DEGRADED, moving to DEGRADED" << endl;
                break;
            }

            if (isInconsistentMotionPossible(valid_rpm, valid_speed))
            {
                if (inconsistentMotion(rpm, speed))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Motion Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent motion check." << endl;
            }

            if (isInconsistentThrottlePossible(valid_rpm, valid_throttle))
            {
                if (inconsistentThrottle(rpm, throttle))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Throttle Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent throttle check." << endl;
            }

            current_state = State::OPERATIONAL;
            cout << "Self_test completed without errors." << endl;
            cout << "Everything under normal operational status, moving to OPERATIONAL" << endl;
            break;

        case State::OPERATIONAL:
            printCurrentState(current_state);

            cout << "Enter new values:" << endl;
            cout << "RPM: ";
            cin >> rpm;

            cout << "Speed: ";
            cin >> speed;

            cout << "Temperature: ";
            cin >> temperature;

            cout << "Voltage: ";
            cin >> voltage;

            cout << "Throttle: ";
            cin >> throttle;

            cout << "Updated values." << endl;
            cout << "Validating new values..." << endl;

            valid_temperature = isTemperatureValid(temperature);
            valid_voltage = isVoltageValid(voltage);

            if (areCriticalValuesMissing(valid_voltage, valid_temperature))
            {
                current_state = State::SAFE_STATE;
                cout << "Critical Values missing, moving to SAFE_STATE" << endl;
                break;
            }

            valid_rpm = isRpmValid(rpm);
            valid_speed = isSpeedValid(speed);
            valid_throttle = isThrottleValid(throttle);

            if (areNormalValuesMissing(valid_rpm, valid_speed, valid_throttle) >= 2)
            {
                current_state = State::SAFE_STATE;
                cout << "2 or more values are missing, moving to SAFE_STATE" << endl;
                break;
            }

            temperature_state = currentTemperatureState(temperature);

            if (temperature_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Temperature State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            voltage_state = currentVoltageState(voltage);

            if (voltage_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Voltage State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            if (temperature_state == AttributeState::DEGRADED)
            {
                current_state = State::DEGRADED;
                cout << "Temperature State is DEGRADED, moving to DEGRADED" << endl;
                break;
            }

            if (voltage_state == AttributeState::DEGRADED)
            {
                current_state = State::DEGRADED;
                cout << "Voltage State is DEGRADED, moving to DEGRADED" << endl;
                break;
            }

            if (isInconsistentMotionPossible(valid_rpm, valid_speed))
            {
                if (inconsistentMotion(rpm, speed))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Motion Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent motion check." << endl;
            }

            if (isInconsistentThrottlePossible(valid_rpm, valid_throttle))
            {
                if (inconsistentThrottle(rpm, throttle))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Throttle Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent throttle check." << endl;
            }

            printCurrentState(current_state);
            cout << "Everything under normal operational status, keep it OPERATIONAL" << endl;

            break;

        case State::DEGRADED:
            printCurrentState(current_state);

            cout << "System operating with limitations..." << endl;
            cout << "Enter new values:" << endl;
            cout << "RPM: ";
            cin >> rpm;

            cout << "Speed: ";
            cin >> speed;

            cout << "Temperature: ";
            cin >> temperature;

            cout << "Voltage: ";
            cin >> voltage;

            cout << "Throttle: ";
            cin >> throttle;

            cout << "Updated values." << endl;
            cout << "Validating new values..." << endl;

            valid_temperature = isTemperatureValid(temperature);
            valid_voltage = isVoltageValid(voltage);

            if (areCriticalValuesMissing(valid_voltage, valid_temperature))
            {
                current_state = State::SAFE_STATE;
                cout << "Critical Values missing, moving to SAFE_STATE" << endl;
                break;
            }

            valid_rpm = isRpmValid(rpm);
            valid_speed = isSpeedValid(speed);
            valid_throttle = isThrottleValid(throttle);

            if (areNormalValuesMissing(valid_rpm, valid_speed, valid_throttle) >= 2)
            {
                current_state = State::SAFE_STATE;
                cout << "2 or more values are missing, moving to SAFE_STATE" << endl;
                break;
            }

            temperature_state = currentTemperatureState(temperature);

            if (temperature_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Temperature State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            voltage_state = currentVoltageState(voltage);

            if (voltage_state == AttributeState::CRITICAL)
            {
                current_state = State::SAFE_STATE;
                cout << "Voltage State is CRITICAL, moving to SAFE_STATE" << endl;
                break;
            }

            if (temperature_state == AttributeState::DEGRADED)
            {
                cout << "Temperature State is DEGRADED, keep it DEGRADED" << endl;
                break;
            }

            if (voltage_state == AttributeState::DEGRADED)
            {
                cout << "Voltage State is DEGRADED, keep it DEGRADED" << endl;
                break;
            }

            if (isInconsistentMotionPossible(valid_rpm, valid_speed))
            {
                if (inconsistentMotion(rpm, speed))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Motion Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent motion check." << endl;
            }

            if (isInconsistentThrottlePossible(valid_rpm, valid_throttle))
            {
                if (inconsistentThrottle(rpm, throttle))
                {
                    current_state = State::DEGRADED;
                    cout << "Inconsistent Throttle Detected, moving to DEGRADED" << endl;
                    break;
                }
            }
            else
            {
                cout << "One of the values is invalid. We cant make a reliable inconsistent throttle check." << endl;
            }

            current_state = State::OPERATIONAL;
            cout << "Conditions returned to the normal range, moving to OPERATIONAL" << endl;

            break;

        case State::SAFE_STATE:
            printCurrentState(current_state);
            cout << "The system is in a critical condition." << endl;
            cout << "Deactivating system functionalities..." << endl;

            rpm = 0;
            speed = 0.0;
            temperature = 0.0;
            voltage = 0.0;
            throttle = 0.0;

            current_state = State::SHUTDOWN;
            break;

        case State::SHUTDOWN:
            printCurrentState(current_state);

            cout << "Shutting down system..." << endl;
            runSystem = 0;
            break;
        }
    }
}