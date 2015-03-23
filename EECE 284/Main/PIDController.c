// Test PID Controller by Victa

#include <stdlib.h>
#include <stdio.h>

// Pin Connections
#define SENSOR_LEFT #0x67	// These are arbitrarily decided for now
#define SENSOR_RIGHT #0x68
#define SENSOR_MIDDLE #0x69
#define MOTOR_LEFT #0x70	// These are motor driving pins
#define MOTOR_RIGHT #0x71

// Expectations
#define LEFT_STANDARD 100	// These are arbitrarily decided for now
#define RIGHT_STANDARD 100
#define MIDDLE_STANDARD 100	// not sure if we are gonna need the middle one

#define SMALL_ERROR = 0.0001;
#define TIME_STEP = 100;
#define Kp = 30; 	// P Factor
#define Ki = 5; 	// I Factor
#define Kd = 10;	// D Factor

int main(void) {
	float P, I, D = 0;

	float leftReading, rightReading = 0;
	float hisLeftError, preLeftError, curLeftError = 0;
	float hisRightError, preRightError, curRightError = 0;


	// Output Signals
	float leftVolt = 4;
	float rightVolt = 4;
	driveMotor('Left', leftVolt);	// Assume we have this driveMotor function
	driveMotor('Right', rightVolt);

	while(1){
		// For Left Motor
		leftReading = getVoltage(SENSOR_LEFT); // Assume we have this getVoltage function
		curLeftError = leftReading - LEFT_STANDARD;	// positive means measured volt is smalled than expected, we should turn right

		P = curLeftError;
		I = (curLeftError + preLeftError + hisLeftError) / (3 * TIME_STEP);
		D = (curLeftError - preLeftError) / TIME_STEP;

		hisLeftError = preLeftError;
		preLeftError = curLeftError;

		leftVolt = Kp * P + Ki * I + Kd * D;

		//For Right Motor
		rightReading = getVoltage(SENSOR_RIGHT);
		curRightError = rightReading - RIGHT_STANDARD;
		P = curRightError;
		I = (curRightError + preRightError + hisRightError) / (3 * TIME_STEP);
		D = (curRightError - preRightError) / TIME_STEP;

		hisRightError = preRightError;
		preRightError = curRightError;

		rightVolt = Kp * P + Ki * I + Kd * D;

		// Drive Motor
		driveMotor('Left', leftVolt);
		driveMotor('Right', rightVolt);
	}
}