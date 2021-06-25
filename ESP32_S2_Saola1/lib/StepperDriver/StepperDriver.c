#include <StepperDriver.h>
#include <assert.h>

/*
 * calculate the step pulse in microseconds for a given rpm value.
 * 60[s/min] * 1000000[us/s] / microsteps / steps / rpm
 */
#define STEP_PULSE(steps, microsteps, rpm) (60.0*1000000L/steps/microsteps/rpm)
#define HIGH 1
#define LOW 0
#define DEFAULT_ACCELERATION 1000
#define DEFAULT_DECELERATION 1000
#define MIN_YIELD_MICROS 50

inline unsigned getStepPulse(long long steps, short microsteps, short rpm) {
	return 60.0*1000000L/steps/microsteps/rpm;
}

void delayMicros(unsigned delay_us, long long start_us){

	if (delay_us){
		if (!start_us){
			start_us = esp_timer_get_time();
		}
		if (delay_us > MIN_YIELD_MICROS){
			; // pass until figure how to emulate yield in esp-idf

			// yield();
		}
		// See https://www.gammon.com.au/millis
		while (esp_timer_get_time() - start_us < delay_us);
	}
}

/*
 * calculate the interval til the next pulse
 */
void calcStepPulse(StepperHandle_t *StepperHandle){
	// validate pointer
	assert(StepperHandle != NULL);

    if (StepperHandle->steps_remaining <= 0){  // this should not happen, but avoids strange calculations
        return;
    }
    StepperHandle->steps_remaining--;
    StepperHandle->step_count++;

    if (StepperHandle->Config->mode == LINEAR_SPEED){
        switch (getMotorState(StepperHandle)){
        case ACCELERATING:
            if (StepperHandle->step_count < StepperHandle->steps_to_cruise){
                StepperHandle->step_pulse = StepperHandle->step_pulse - (2*StepperHandle->step_pulse+StepperHandle->rest)/(4*StepperHandle->step_count+1);
                StepperHandle->rest = (StepperHandle->step_count < StepperHandle->steps_to_cruise) ? (2*StepperHandle->step_pulse+StepperHandle->rest) % (4*StepperHandle->step_count+1) : 0;
            } else {
                // The series approximates target, set the final value to what it should be instead
                StepperHandle->step_pulse = StepperHandle->cruise_step_pulse;
            }
            break;

        case DECELERATING:
            StepperHandle->step_pulse = StepperHandle->step_pulse - (2*StepperHandle->step_pulse+StepperHandle->rest)/(-4*StepperHandle->steps_remaining+1);
            StepperHandle->rest = (2*StepperHandle->step_pulse+StepperHandle->rest) % (-4*StepperHandle->steps_remaining+1);
            break;

        default:
            break; // no speed changes
        }
    }
}

StepperHandle_t* createStepperHandle(StepperConfig_t *Config) {
	// validate pointer
	assert(Config != NULL);

	/*
		For malloc(), free(), and freeRTOS equivilents see:
		See https://www.freertos.org/a00111.html
	*/

#ifdef INC_FREERTOS_H
	// allocate dynamically so it is not terminated after leaving scope.
    StepperHandle_t *new_stepper = (StepperHandle_t*)pvPortMalloc(sizeof(StepperHandle_t));	
#endif

#ifndef INC_FREERTOS_H
	// allocate dynamically so it is not terminated after leaving scope.
	StepperHandle_t *NewStepper = (StepperHandle_t*)malloc(sizeof(StepperHandle_t));	// allocated dynamically, so it is not terminated when exiting scope.
#endif

	// TODO: Come back here and validate Config properties.
    NewStepper->Config = Config;
	NewStepper->MotorState = STOPPED;
	NewStepper->steps_to_cruise = 0;
	NewStepper->steps_remaining = 0;
	NewStepper->steps_to_brake = 0;
	NewStepper->dir_state = 0;
	NewStepper->cruise_step_pulse = 0;
	NewStepper->step_pulse = 0;
	NewStepper->rest = 0;
	NewStepper->step_count = 0;
	NewStepper->step_high_min = 1;
	NewStepper->step_low_min = 1;
	NewStepper->wakeup_time = 0;
	NewStepper->pulse_next_time = 0;
	NewStepper->pulse_end_time = 0;
	enableStepper(NewStepper);
    return NewStepper;
}

// deallocates memory for a stepper handle with either threadsafe pvPortMalloc() or malloc() depending on the framework used
void destroyStepperHandle(StepperHandle_t **StepperHandle) {
	assert(*StepperHandle != NULL);

	/*
		For malloc(), free(), and freeRTOS equivilents see:
		See https://www.freertos.org/a00111.html
	*/

#ifdef INC_FREERTOS_H
	vPortFree(*StepperHandle)
#endif

#ifndef INC_FREERTOS_H
	free(*StepperHandle);
#endif

	*StepperHandle = NULL;

}


StepperMotorState_t getMotorState(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);

	if (StepperHandle->steps_remaining <= 0) {
		StepperHandle->MotorState = STOPPED;
	} else {
		if (StepperHandle->steps_remaining <= StepperHandle->steps_to_brake) {
			StepperHandle->MotorState = DECELERATING;
		} else if (StepperHandle->step_count <= StepperHandle->steps_to_cruise) {
			StepperHandle->MotorState = ACCELERATING;
		} else {
			StepperHandle->MotorState = CRUISING;
		}
	}

	return StepperHandle->MotorState;
}

/*
 * Configure which logic state on ENABLE pin means active
 * when using SLEEP (default) this is active HIGH
 */
void setEnableActiveState(StepperHandle_t *StepperHandle, bool state) {
	// validate pointer
	assert(StepperHandle != NULL);
	StepperHandle->Config->enable_active_state = state;
}

void disableStepper(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);
	// Ensure the pin has been initialized.
	// if (check_connected(StepperHandle->Config->enable_pin)) {
		// Set the active state, specified by the user and their hardware.
		gpio_set_level(StepperHandle->Config->enable_pin, (StepperHandle->Config->enable_active_state == HIGH) ? LOW: HIGH);
	// }
}

void enableStepper(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);
	// Ensure the pin has been initialized.
	// if (check_connected(StepperHandle->Config->enable_pin)) {
		// Set the active state, specified by the user and their hardware.
		gpio_set_level(StepperHandle->Config->enable_pin, (StepperHandle->Config->enable_active_state == HIGH) ? HIGH : LOW);
	// }
}

unsigned getStepperDirection(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);
	return gpio_get_level(StepperHandle->Config->direction_pin);
}

void setStepperDirection(StepperHandle_t *StepperHandle, short direction) {
	// validate pointer
	assert(StepperHandle != NULL);
	StepperHandle->dir_state = direction;
}

float getStepperRPM(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);
	return StepperHandle->Config->rpm;
}

void setStepperRPM(StepperHandle_t *StepperHandle, short rpm) {
	// validate pointer
	assert(StepperHandle != NULL);
	StepperHandle->Config->rpm = rpm;
}

void setMicrosteps(StepperHandle_t *StepperHandle, short microsteps) {
	// validate pointer
	assert(StepperHandle != NULL);
	StepperHandle->Config->microstepping = microsteps;
}

unsigned getMicrosteps(StepperHandle_t *StepperHandle) {
	// validate pointer
	assert(StepperHandle != NULL);
	return StepperHandle->Config->microstepping;
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
void move(StepperHandle_t *StepperHandle, long long steps){
	// validate pointer
	assert(StepperHandle != NULL);
    startMove(StepperHandle, steps, 0);
    while (nextAction(StepperHandle));
}

/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
void rotate(StepperHandle_t *StepperHandle, short deg){
	// validate pointer
	assert(StepperHandle != NULL);
	move(StepperHandle, calcStepsForRotation(StepperHandle, deg));
}

/*
 * Yield to step control
 * Toggle step and return time until next change is needed (micros)
 */
unsigned nextAction(StepperHandle_t *StepperHandle){
	// validate pointer
	assert(StepperHandle != NULL);

	if (StepperHandle->steps_remaining > 0){
        delayMicros(StepperHandle->pulse_next_time, StepperHandle->pulse_end_time);
        /*
         * DIR pin is sampled on rising STEP edge, so it is set first
         */
		gpio_set_level(StepperHandle->Config->direction_pin, StepperHandle->dir_state);
		gpio_set_level(StepperHandle->Config->step_pin, HIGH);
        long long pulse_start_time = esp_timer_get_time();
        unsigned pulse = StepperHandle->step_pulse; // save value because calcStepPulse() will overwrite it
        calcStepPulse(StepperHandle);
        // We should pull HIGH for at least 1-2us (step_high_min)
        delayMicros(StepperHandle->step_high_min, 0);
		gpio_set_level(StepperHandle->Config->step_pin, LOW);
        // account for calcStepPulse() execution time; sets ceiling for max rpm on slower MCUs
        long long pulse_end_time = esp_timer_get_time();
		// find executation time duration
        pulse_start_time = pulse_end_time - pulse_start_time;
		// determine if we need to wait a number of tics before next pulse, or if we do it as soon as possible (next cycle)
        StepperHandle->pulse_next_time = (pulse > pulse_start_time) ? pulse - pulse_start_time : 1;
    } else {
        // end of move
        StepperHandle->pulse_end_time = 0;
        StepperHandle->pulse_next_time = 0;
    }
    return StepperHandle->pulse_next_time;
}

/*
 * Set up a new move (calculate and save the parameters)
 */
void startMove(StepperHandle_t *StepperHandle, long long steps, unsigned time){
	// validate pointer
	assert(StepperHandle != NULL);
	float lin_speed;
    // initialize a new move
    StepperHandle->dir_state = (steps >= 0) ? HIGH : LOW;
    StepperHandle->pulse_end_time = 0;
    StepperHandle->steps_remaining = llabs(steps);
    StepperHandle->step_count = 0;
    StepperHandle->rest = 0;
    switch (StepperHandle->Config->mode){

		case LINEAR_SPEED:
			// speed is in [steps/s]
			lin_speed = StepperHandle->Config->rpm * StepperHandle->Config->motor_steps / 60;
			if (time > 0){
				// Calculate a new speed to finish in the time requested
				float t = time / (1e+6);                  // convert to seconds
				float d = StepperHandle->steps_remaining / StepperHandle->Config->microstepping;   // convert to full steps
				float a2 = 1.0 / DEFAULT_ACCELERATION + 1.0 / DEFAULT_DECELERATION;
				float sqrt_candidate = t*t - 2 * a2 * d;  // in √b^2-4ac
				if (sqrt_candidate >= 0){
					lin_speed = fmin(lin_speed, (t - (float)sqrt(sqrt_candidate)) / a2);
				};
			}
			// how many microsteps from 0 to target speed
			StepperHandle->steps_to_cruise = StepperHandle->Config->microstepping * (lin_speed * lin_speed / (2 * DEFAULT_ACCELERATION));
			// how many microsteps are needed from cruise speed to a full stop
			StepperHandle->steps_to_brake = StepperHandle->steps_to_cruise * DEFAULT_ACCELERATION / DEFAULT_DECELERATION;
			if (StepperHandle->steps_remaining < StepperHandle->steps_to_cruise + StepperHandle->steps_to_brake){
				// cannot reach max speed, will need to brake early
				StepperHandle->steps_to_cruise = StepperHandle->steps_remaining * DEFAULT_DECELERATION / (DEFAULT_ACCELERATION + DEFAULT_DECELERATION);
				StepperHandle->steps_to_brake = StepperHandle->steps_remaining - StepperHandle->steps_to_cruise;
			}
			// Initial pulse (c0) including error correction factor 0.676 [us]
			StepperHandle->step_pulse = (1e+6)*0.676*sqrt(2.0f/DEFAULT_ACCELERATION/StepperHandle->Config->microstepping);
			// Save cruise timing since we will no longer have the calculated target speed later
			StepperHandle->cruise_step_pulse = 1e+6 / lin_speed / StepperHandle->Config->microstepping;
			break;

		case CONSTANT_SPEED:
		default:
			StepperHandle->steps_to_cruise = 0;
			StepperHandle->steps_to_brake = 0;
			StepperHandle->step_pulse = StepperHandle->cruise_step_pulse = getStepPulse(StepperHandle->Config->motor_steps, StepperHandle->Config->microstepping, StepperHandle->Config->rpm);

			if (time > StepperHandle->steps_remaining * StepperHandle->step_pulse){
				StepperHandle->step_pulse = time / StepperHandle->steps_remaining;
			}

		}
}

/*
 * Move the motor an integer number of degrees (360 = full rotation)
 * This has poor precision for small amounts, since step is usually 1.8deg
 */
inline void startRotate(StepperHandle_t *StepperHandle, short deg){
	// validate pointer
	assert(StepperHandle != NULL);
    startMove(StepperHandle, calcStepsForRotation(StepperHandle, deg), 0L);
}

/*
 * Brake early.
 */
void startBrake(StepperHandle_t *StepperHandle){
	// validate pointer
	assert(StepperHandle != NULL);

    switch (getMotorState(StepperHandle)){
    case CRUISING:  // this applies to both CONSTANT_SPEED and LINEAR_SPEED modes
        StepperHandle->steps_remaining = StepperHandle->steps_to_brake;
        break;

    case ACCELERATING:
        StepperHandle->steps_remaining = StepperHandle->step_count * DEFAULT_ACCELERATION / DEFAULT_DECELERATION;
        break;

    default:
        break; // nothing to do if already stopped or braking
    }
}

/*
 * Stop movement immediately and return remaining steps.
 */
long long stop(StepperHandle_t *StepperHandle){
	// validate pointer
	assert(StepperHandle != NULL);
    long long v = StepperHandle->steps_remaining;
    StepperHandle->steps_remaining = 0;
    return v;
}

/*
 * Return calculated time to complete the given number of steps (in microseconds)
 */
unsigned getTimeForMove(StepperHandle_t *StepperHandle, long long steps){
    float t;
	float speed;
    unsigned cruise_steps;
    
    if (steps == 0){
        return 0;
    }
    switch (StepperHandle->Config->mode){
        case LINEAR_SPEED:
            startMove(StepperHandle, steps, 0L);
            cruise_steps = StepperHandle->steps_remaining - StepperHandle->steps_to_cruise - StepperHandle->steps_to_brake;
            speed = StepperHandle->Config->rpm * StepperHandle->Config->motor_steps / 60;   // full steps/s
            t = (cruise_steps / (StepperHandle->Config->microstepping * speed)) + 
                sqrt(2.0 * StepperHandle->steps_to_cruise / DEFAULT_ACCELERATION / StepperHandle->Config->microstepping) +
                sqrt(2.0 * StepperHandle->steps_to_brake / DEFAULT_DECELERATION / StepperHandle->Config->microstepping);
            t *= (1e+6); // seconds -> micros
            break;
        case CONSTANT_SPEED:
        default:
			t = steps * getStepPulse(StepperHandle->Config->motor_steps, StepperHandle->Config->microstepping, StepperHandle->Config->rpm);
    }
    return round(t);
}

inline long long calcStepsForRotation(StepperHandle_t *StepperHandle, short deg){
	// validate pointer
	assert(StepperHandle != NULL);
	// use 64 width in case of large movements
	return deg * StepperHandle->Config->motor_steps * StepperHandle->Config->microstepping / 360;
}
