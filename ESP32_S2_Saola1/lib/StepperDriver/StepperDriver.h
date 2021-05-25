#ifndef STEPPERDRIVER_H
#define STEPPERDRIVER_H

#include <esp_timer.h>
#include <driver/gpio.h>
#include <math.h>

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
    StepperConfig_t *configuration;
	StepperMotorState_t motor_state;
    short direction_state; // dir pin state.
    long rest;
    long last_action_end;
    long next_action_interval;
    long step_count;
    long steps_remaining;
    long steps_to_cruise;
    long steps_to_brake;
    long step_pulse;
    long cruise_step_pulse;
	int step_high_min;
	int step_low_min;
	int wakeup_time;
} StepperHandle_t;

extern long getStepPulse(long steps, short microsteps, short rpm);

extern void startMove(StepperHandle_t *stepper_handler, long steps, long time);

extern void delayMicros(unsigned long delay_us, unsigned long start_us);

extern void calcStepPulse(StepperHandle_t *stepper_handler);

extern StepperHandle_t createStepperHandler(StepperConfig_t *configuration);

extern StepperMotorState_t getMotorState(StepperHandle_t *Stepperhandler);

extern void setEnableActiveState(StepperHandle_t *stepper_handler, bool state);

extern void disableStepper(StepperHandle_t *stepper_handler);

extern void enableStepper(StepperHandle_t *stepper_handler);

extern unsigned getStepperDirection(StepperHandle_t *stepper_handler);

extern void setStepperDirection(StepperHandle_t *stepper_handler, unsigned direction);

extern float getStepperRPM(StepperHandle_t *stepper_handler);

extern void setStepperRPM(StepperHandle_t *stepper_handler, short rpm);

extern void setMicrosteps(StepperHandle_t *stepper_handler, short microsteps);

extern unsigned getMicrosteps(StepperHandle_t *stepper_handler);

extern void move(StepperHandle_t *stepper_handler, long steps);

extern void rotate(StepperHandle_t *stepper_handler, short deg);

extern long nextAction(StepperHandle_t *stepper_handler);

extern void startMove(StepperHandle_t *stepper_handler, long steps, long time);

extern void startRotate(StepperHandle_t *stepper_handler, short deg);

extern void startBrake(StepperHandle_t *stepper_handler);

extern long stop(StepperHandle_t *stepper_handler);

extern long getTimeForMove(StepperHandle_t *stepper_handler, long steps);

extern long calcStepsForRotation(StepperHandle_t *stepper_motor, short deg);

#endif
