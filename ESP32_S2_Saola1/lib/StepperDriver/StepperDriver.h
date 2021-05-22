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
	CONSTANT_SPEED,
	LINEAR_SPEED,
} StepperDriverMode_t;

typedef enum StepperMotorState_t{
	STOPPED, 
	ACCELERATING, 
	CRUISING, 
	DECELERATING,
} StepperMotorState_t;

typedef struct StepperConfig_t {
    gpio_num_t direction_pin;
    gpio_num_t step_pin;
    gpio_num_t enable_pin;
	int motor_steps;
    int enable_active_state;
    int microstepping;
    float rpm;
    StepperDriverMode_t mode;
} StepperConfig_t;

typedef struct StepperHandler_t {
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
} StepperHandler_t;

extern long getStepPulse(long steps, short microsteps, float rpm);

extern void startMove(StepperHandler_t *stepper_handler, long steps, long time);

extern void delayMicros(unsigned long delay_us, unsigned long start_us);

extern void calcStepPulse(StepperHandler_t *stepper_handler);

extern StepperHandler_t createStepperHandler(StepperConfig_t *configuration);

extern StepperMotorState_t getMotorState(StepperHandler_t *Stepperhandler);

extern void setEnableActiveState(StepperHandler_t *stepper_handler, bool state);

extern void disableStepper(StepperHandler_t *stepper_handler);

extern void enableStepper(StepperHandler_t *stepper_handler);

extern unsigned getStepperDirection(StepperHandler_t *stepper_handler);

extern void setStepperDirection(StepperHandler_t *stepper_handler, unsigned direction);

extern float getStepperRPM(StepperHandler_t *stepper_handler);

extern void setStepperRPM(StepperHandler_t *stepper_handler, unsigned rpm);

extern void setMicrosteps(StepperHandler_t *stepper_handler, short microsteps);

extern unsigned getMicrosteps(StepperHandler_t *stepper_handler);

extern void move(StepperHandler_t *stepper_handler, long steps);

extern void rotate(StepperHandler_t *stepper_handler, double deg);

extern long nextAction(StepperHandler_t *stepper_handler);

extern void startMove(StepperHandler_t *stepper_handler, long steps, long time);

extern void startRotate(StepperHandler_t *stepper_handler, long deg);

extern void startBrake(StepperHandler_t *stepper_handler);

extern long stop(StepperHandler_t *stepper_handler);

extern long getTimeForMove(StepperHandler_t *stepper_handler, long steps);

extern long calcStepsForRotation(StepperHandler_t *stepper_motor, double deg);

#endif
