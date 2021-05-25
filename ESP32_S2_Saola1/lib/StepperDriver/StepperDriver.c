#include <StepperDriver.h>

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

inline long getStepPulse(long steps, short microsteps, short rpm) {
	return 60.0*1000000L/steps/microsteps/rpm;
}

inline void delayMicros(unsigned long delay_us, unsigned long start_us){
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
inline void calcStepPulse(StepperHandle_t *stepper_handler){
    if (stepper_handler->steps_remaining <= 0){  // this should not happen, but avoids strange calculations
        return;
    }
    stepper_handler->steps_remaining--;
    stepper_handler->step_count++;

    if (stepper_handler->configuration->mode == LINEAR_SPEED){
        switch (getMotorState(stepper_handler)){
        case ACCELERATING:
            if (stepper_handler->step_count < stepper_handler->steps_to_cruise){
                stepper_handler->step_pulse = stepper_handler->step_pulse - (2*stepper_handler->step_pulse+stepper_handler->rest)/(4*stepper_handler->step_count+1);
                stepper_handler->rest = (stepper_handler->step_count < stepper_handler->steps_to_cruise) ? (2*stepper_handler->step_pulse+stepper_handler->rest) % (4*stepper_handler->step_count+1) : 0;
            } else {
                // The series approximates target, set the final value to what it should be instead
                stepper_handler->step_pulse = stepper_handler->cruise_step_pulse;
            }
            break;

        case DECELERATING:
            stepper_handler->step_pulse = stepper_handler->step_pulse - (2*stepper_handler->step_pulse+stepper_handler->rest)/(-4*stepper_handler->steps_remaining+1);
            stepper_handler->rest = (2*stepper_handler->step_pulse+stepper_handler->rest) % (-4*stepper_handler->steps_remaining+1);
            break;

        default:
            break; // no speed changes
        }
    }
}

inline StepperHandle_t createStepperHandler(StepperConfig_t *configuration) {
	// malloc syntax:
	// ptr = (cast-type*) malloc(byte-size)
    StepperHandle_t *new_stepper = (StepperHandle_t*)malloc(sizeof(StepperHandle_t));	// allocated dynamically, so it is not terminated when exiting scope.
	// TODO: Come back here and validate configuration properties.
    new_stepper->configuration = configuration;
	new_stepper->motor_state = STOPPED;
	new_stepper->steps_to_cruise = 0;
	new_stepper->steps_remaining = 0;
	new_stepper->steps_to_brake = 0;
	new_stepper->direction_state = 0;
	new_stepper->cruise_step_pulse = 0;
	new_stepper->step_pulse = 0;
	new_stepper->rest = 0;
	new_stepper->step_count = 0;
	new_stepper->step_high_min = 1;
	new_stepper->step_low_min = 1;
	new_stepper->wakeup_time = 0;
	new_stepper->next_action_interval = 0;
	new_stepper->last_action_end = 0;
	enableStepper(new_stepper);
    return *new_stepper;
}

inline StepperMotorState_t getMotorState(StepperHandle_t *StepperHandler) {

	if (StepperHandler->steps_remaining <= 0) {
		StepperHandler->motor_state = STOPPED;
	} else {
		if (StepperHandler->steps_remaining <= StepperHandler->steps_to_brake) {
			StepperHandler->motor_state = DECELERATING;
		} else if (StepperHandler->step_count <= StepperHandler->steps_to_cruise) {
			StepperHandler->motor_state = ACCELERATING;
		} else {
			StepperHandler->motor_state = CRUISING;
		}
	}

	return StepperHandler->motor_state;
}

/*
 * Configure which logic state on ENABLE pin means active
 * when using SLEEP (default) this is active HIGH
 */
inline void setEnableActiveState(StepperHandle_t *StepperHandler, bool state) {
	StepperHandler->configuration->enable_active_state = state;
}

inline void disableStepper(StepperHandle_t *StepperHandler) {
	// Ensure the pin has been initialized.
	// if (check_connected(StepperHandler->configuration->enable_pin)) {
		// Set the active state, specified by the user and their hardware.
		gpio_set_level(StepperHandler->configuration->enable_pin, (StepperHandler->configuration->enable_active_state == HIGH) ? LOW: HIGH);
	// }
}

inline void enableStepper(StepperHandle_t *StepperHandler) {
	// Ensure the pin has been initialized.
	// if (check_connected(StepperHandler->configuration->enable_pin)) {
		// Set the active state, specified by the user and their hardware.
		gpio_set_level(StepperHandler->configuration->enable_pin, (StepperHandler->configuration->enable_active_state == HIGH) ? HIGH : LOW);
	// }
}

inline unsigned getStepperDirection(StepperHandle_t *stepper_handler) {
	return gpio_get_level(stepper_handler->configuration->direction_pin);
}

inline void setStepperDirection(StepperHandle_t *stepper_handler, unsigned direction) {
	stepper_handler->direction_state = direction;
}

inline float getStepperRPM(StepperHandle_t *stepper_handler) {
	return stepper_handler->configuration->rpm;
}

inline void setStepperRPM(StepperHandle_t *stepper_handler, short rpm) {
	stepper_handler->configuration->rpm = rpm;
}

inline void setMicrosteps(StepperHandle_t *stepper_handler, short microsteps) {
	stepper_handler->configuration->microstepping = microsteps;
}

inline unsigned getMicrosteps(StepperHandle_t *stepper_handler) {
	return stepper_handler->configuration->microstepping;
}


/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
inline void move(StepperHandle_t *stepper_handler, long steps){
	
    startMove(stepper_handler, steps, 0);
    while (nextAction(stepper_handler));
}

/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
inline void rotate(StepperHandle_t *stepper_handler, short deg){
    move(stepper_handler, calcStepsForRotation(stepper_handler, deg));
}

/*
 * Yield to step control
 * Toggle step and return time until next change is needed (micros)
 */
inline long nextAction(StepperHandle_t *stepper_handler){
	if (stepper_handler->steps_remaining > 0){
        delayMicros(stepper_handler->next_action_interval, stepper_handler->last_action_end);
        /*
         * DIR pin is sampled on rising STEP edge, so it is set first
         */
		gpio_set_level(stepper_handler->configuration->direction_pin, stepper_handler->direction_state);
		gpio_set_level(stepper_handler->configuration->step_pin, HIGH);
        unsigned duty_time = esp_timer_get_time();
        unsigned long pulse = stepper_handler->step_pulse; // save value because calcStepPulse() will overwrite it
        calcStepPulse(stepper_handler);
        // We should pull HIGH for at least 1-2us (step_high_min)
        delayMicros(stepper_handler->step_high_min, 0);
		gpio_set_level(stepper_handler->configuration->step_pin, LOW);
        // account for calcStepPulse() execution time; sets ceiling for max rpm on slower MCUs
        stepper_handler->last_action_end = esp_timer_get_time();
        duty_time = stepper_handler->last_action_end - duty_time;
		// Actual time it took to execute.
        stepper_handler->next_action_interval = (pulse > duty_time) ? pulse - duty_time : 1;
    } else {
        // end of move
        stepper_handler->last_action_end = 0;
        stepper_handler->next_action_interval = 0;
    }
    return stepper_handler->next_action_interval;
}

/*
 * Set up a new move (calculate and save the parameters)
 */
inline void startMove(StepperHandle_t *stepper_handler, long steps, long time){
	float speed;
    // set up new move
    stepper_handler->direction_state = (steps >= 0) ? HIGH : LOW;
    stepper_handler->last_action_end = 0;
    stepper_handler->steps_remaining = labs(steps);
    stepper_handler->step_count = 0;
    stepper_handler->rest = 0;
    switch (stepper_handler->configuration->mode){

		case LINEAR_SPEED:
			// speed is in [steps/s]
			speed = stepper_handler->configuration->rpm * stepper_handler->configuration->motor_steps / 60;
			if (time > 0){
				// Calculate a new speed to finish in the time requested
				float t = time / (1e+6);                  // convert to seconds
				float d = stepper_handler->steps_remaining / stepper_handler->configuration->microstepping;   // convert to full steps
				float a2 = 1.0 / DEFAULT_ACCELERATION + 1.0 / DEFAULT_DECELERATION;
				float sqrt_candidate = t*t - 2 * a2 * d;  // in √b^2-4ac
				if (sqrt_candidate >= 0){
					speed = fmin(speed, (t - (float)sqrt(sqrt_candidate)) / a2);
				};
			}
			// how many microsteps from 0 to target speed
			stepper_handler->steps_to_cruise = stepper_handler->configuration->microstepping * (speed * speed / (2 * DEFAULT_ACCELERATION));
			// how many microsteps are needed from cruise speed to a full stop
			stepper_handler->steps_to_brake = stepper_handler->steps_to_cruise * DEFAULT_ACCELERATION / DEFAULT_DECELERATION;
			if (stepper_handler->steps_remaining < stepper_handler->steps_to_cruise + stepper_handler->steps_to_brake){
				// cannot reach max speed, will need to brake early
				stepper_handler->steps_to_cruise = stepper_handler->steps_remaining * DEFAULT_DECELERATION / (DEFAULT_ACCELERATION + DEFAULT_DECELERATION);
				stepper_handler->steps_to_brake = stepper_handler->steps_remaining - stepper_handler->steps_to_cruise;
			}
			// Initial pulse (c0) including error correction factor 0.676 [us]
			stepper_handler->step_pulse = (1e+6)*0.676*sqrt(2.0f/DEFAULT_ACCELERATION/stepper_handler->configuration->microstepping);
			// Save cruise timing since we will no longer have the calculated target speed later
			stepper_handler->cruise_step_pulse = 1e+6 / speed / stepper_handler->configuration->microstepping;
			break;

		case CONSTANT_SPEED:
		default:
			stepper_handler->steps_to_cruise = 0;
			stepper_handler->steps_to_brake = 0;
			stepper_handler->step_pulse = stepper_handler->cruise_step_pulse = getStepPulse(stepper_handler->configuration->motor_steps, stepper_handler->configuration->microstepping, stepper_handler->configuration->rpm);

			if (time > stepper_handler->steps_remaining * stepper_handler->step_pulse){
				stepper_handler->step_pulse = (float)time / stepper_handler->steps_remaining;
			}

		}
}

/*
 * Move the motor an integer number of degrees (360 = full rotation)
 * This has poor precision for small amounts, since step is usually 1.8deg
 */
inline void startRotate(StepperHandle_t *stepper_handler, short deg){
    startMove(stepper_handler, calcStepsForRotation(stepper_handler, deg), 0L);
}

/*
 * Brake early.
 */
inline void startBrake(StepperHandle_t *stepper_handler){
    switch (getMotorState(stepper_handler)){
    case CRUISING:  // this applies to both CONSTANT_SPEED and LINEAR_SPEED modes
        stepper_handler->steps_remaining = stepper_handler->steps_to_brake;
        break;

    case ACCELERATING:
        stepper_handler->steps_remaining = stepper_handler->step_count * DEFAULT_ACCELERATION / DEFAULT_DECELERATION;
        break;

    default:
        break; // nothing to do if already stopped or braking
    }
}

/*
 * Stop movement immediately and return remaining steps.
 */
inline long stop(StepperHandle_t *stepper_handler){
    long retval = stepper_handler->steps_remaining;
    stepper_handler->steps_remaining = 0;
    return retval;
}

/*
 * Return calculated time to complete the given move
 */
inline long getTimeForMove(StepperHandle_t *stepper_handler, long steps){
    float t;
    long cruise_steps;
    float speed;
    if (steps == 0){
        return 0;
    }
    switch (stepper_handler->configuration->mode){
        case LINEAR_SPEED:
            startMove(stepper_handler, steps, 0L);
            cruise_steps = stepper_handler->steps_remaining - stepper_handler->steps_to_cruise - stepper_handler->steps_to_brake;
            speed = stepper_handler->configuration->rpm * stepper_handler->configuration->motor_steps / 60;   // full steps/s
            t = (cruise_steps / (stepper_handler->configuration->microstepping * speed)) + 
                sqrt(2.0 * stepper_handler->steps_to_cruise / DEFAULT_ACCELERATION / stepper_handler->configuration->microstepping) +
                sqrt(2.0 * stepper_handler->steps_to_brake / DEFAULT_DECELERATION / stepper_handler->configuration->microstepping);
            t *= (1e+6); // seconds -> micros
            break;
        case CONSTANT_SPEED:
        default:
			t = steps * getStepPulse(stepper_handler->configuration->motor_steps, stepper_handler->configuration->microstepping, stepper_handler->configuration->rpm);
    }
    return round(t);
}

inline long calcStepsForRotation(StepperHandle_t *stepper_handler, short deg){
	return deg * stepper_handler->configuration->motor_steps * stepper_handler->configuration->microstepping / 360;
}
