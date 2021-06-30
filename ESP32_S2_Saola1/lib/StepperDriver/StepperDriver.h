#ifndef STEPPERDRIVER_H
#define STEPPERDRIVER_H

// include freeRTOS methods if end-user has the framework defined somewhere in the application.
// This is mainly to perform thread-safe dynamic memory allocation, while allowing freeRTOS to be voided if not used.
#ifdef INC_FREERTOS_H
#include <freertos/FreeRTOS.h>
#endif

#include <esp_timer.h>
#include <driver/gpio.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STEP_PULSE(steps, microsteps, rpm) (60.0*1000000L/steps/microsteps/rpm)
#define HIGH 1
#define LOW 0
#define DEFAULT_ACCELERATION 1000
#define DEFAULT_DECELERATION 1000
#define MIN_YIELD_MICROS 50

typedef enum StepperDriverMode_t{
	CONSTANT_SPEED = 0,
	LINEAR_SPEED,
} StepperDriverMode_t;

typedef enum StepperMotorState_t{
	STOPPED = 0,
	ACCELERATING, 
	CRUISING, 
	DECELERATING,
} StepperMotorState_t;

typedef struct StepperConfig_t {
    gpio_num_t direction_pin;   // pin to control the direction state
    gpio_num_t step_pin;        // pin to send pulse to the stepper driver
    gpio_num_t enable_pin;      // pin to enable the motor control
	short motor_steps;          // number of steps per revolution
    bool enable_active_state;   // logic value for the enable pin to be active
    short microstepping;        // number of microsteps per step
    short rpm;                  // speed (revolutions per minute)
    StepperDriverMode_t mode;   // linear or constant modes
} StepperConfig_t;

typedef struct StepperHandle_t {
    StepperConfig_t *Config;
	StepperMotorState_t MotorState;
    short dir_state;
    unsigned rest;
    unsigned pulse_end_time;
    unsigned pulse_next_time;
    unsigned step_count;
    long long steps_remaining;  // 64 wide for very long travel moves and compatability with esp_get_time().
    unsigned steps_to_cruise;
    unsigned steps_to_brake;
    unsigned step_pulse;
    unsigned cruise_step_pulse;
	short step_high_min;
	short step_low_min;
	short wakeup_time;
} StepperHandle_t;

extern StepperHandle_t* createStepperHandle(StepperConfig_t *Config);

extern void destroyStepperHandle(StepperHandle_t **StepperHandle);

extern unsigned getStepPulse(long long steps, short microsteps, short rpm);

extern void startMove(StepperHandle_t *StepperHandle, long long steps, unsigned time);

extern void delayMicros(unsigned delay_us, long long start_us);

extern void calcStepPulse(StepperHandle_t *StepperHandle);

extern StepperMotorState_t getMotorState(StepperHandle_t *Stepperhandler);

extern void setEnableActiveState(StepperHandle_t *StepperHandle, bool state);

extern void disableStepper(StepperHandle_t *StepperHandle);

extern void enableStepper(StepperHandle_t *StepperHandle);

extern unsigned getStepperDirection(StepperHandle_t *StepperHandle);

extern void setStepperDirection(StepperHandle_t *StepperHandle, short direction);

extern float getStepperRPM(StepperHandle_t *StepperHandle);

extern void setStepperRPM(StepperHandle_t *StepperHandle, short rpm);

extern void setMicrosteps(StepperHandle_t *StepperHandle, short microsteps);

extern unsigned getMicrosteps(StepperHandle_t *StepperHandle);

extern void move(StepperHandle_t *StepperHandle, long long steps);

extern void rotate(StepperHandle_t *StepperHandle, short deg);

extern unsigned nextAction(StepperHandle_t *StepperHandle);

extern void startMove(StepperHandle_t *StepperHandle, long long steps, unsigned time);

extern void startRotate(StepperHandle_t *StepperHandle, short deg);

extern void startBrake(StepperHandle_t *StepperHandle);

extern long long stop(StepperHandle_t *StepperHandle);

extern unsigned getTimeForMove(StepperHandle_t *StepperHandle, long long steps);

extern long long calcStepsForRotation(StepperHandle_t *StepperHandle, short deg);

#ifdef __cplusplus
}
#endif

#endif  // STEPPERDRIVER_H
