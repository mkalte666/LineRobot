#include <Wire.h>

#define F_CPU 16000000

//Names.h contains funciton bodys so custom datastructurs can be used as function parameters / returntypes AND the enumerations and definitions
#include "names.h"

int f;

void setup()
{
	//IC2
	Wire.begin();

	//Serial feedback
	Serial.begin(SERIAL_BAUDRATE);

	//Input pins
	pinMode(SENSOR_RIGHT, INPUT);
	pinMode(SENSOR_RIGHT_MIDDLE, INPUT);
	pinMode(SENSOR_LEFT_MIDDLE, INPUT);
	pinMode(SENSOR_LEFT, INPUT);

	//Outout pins
	pinMode(MOTOR_L_SPEED, OUTPUT);
	pinMode(MOTOR_R_SPEED, OUTPUT);
	pinMode(MOTOR_L_DIRECTION, OUTPUT);
	pinMode(MOTOR_R_DIRECTION, OUTPUT);
	pinMode(PIN_UNKNOWN, OUTPUT);

	return;
}

void raw_drive(MOTORSTATE left, MOTORSTATE right, int speedL = SPEED_STANDART, int speedR = SPEED_STANDART)
{

	if(BLOCK_MOVEMENTS) return;

	//Left Motor
	switch (left) {
	case MOTOR_STILL:
		digitalWrite(MOTOR_L_DIRECTION, HIGH);
		analogWrite(MOTOR_L_SPEED, 0);
		break;

	case MOTOR_FORWARD:
		digitalWrite(MOTOR_L_DIRECTION, HIGH);
		analogWrite(MOTOR_L_SPEED, speedL*SPEED_MODIFIER_LEFT);
		break;

	case MOTOR_BACKWARD:
		digitalWrite(MOTOR_L_DIRECTION, LOW);
		analogWrite(MOTOR_L_SPEED, speedL*SPEED_MODIFIER_LEFT);
		break;

	default:
		Serial.println("Warning: Motor state Error");
		break;
	}

	//Right Motor
	switch (right) {
	case MOTOR_STILL:
		digitalWrite(MOTOR_R_DIRECTION, LOW);
		analogWrite(MOTOR_R_SPEED, 0);
		break;

	case MOTOR_FORWARD:
		digitalWrite(MOTOR_R_DIRECTION, LOW);
		analogWrite(MOTOR_R_SPEED, speedR*SPEED_MODIFIER_RIGHT);
		break;

	case MOTOR_BACKWARD:
		digitalWrite(MOTOR_R_DIRECTION, HIGH);
		analogWrite(MOTOR_R_SPEED, speedR*SPEED_MODIFIER_RIGHT);
		break;

	default:
		Serial.println("Warning: Motor state Error");
		break;
	}

	return;
}

void drive(MOVEMENTSTATE movement)
{
	switch (movement)
	{
	case MOVE_FORWARD:
		raw_drive(MOTOR_FORWARD, MOTOR_FORWARD);
		break;
	case MOVE_BACKWARD:
		raw_drive(MOTOR_BACKWARD, MOTOR_BACKWARD);
		break;
	case MOVE_LEFT:
		raw_drive(MOTOR_FORWARD, MOTOR_FORWARD, SPEED_STANDART *0.7, SPEED_STANDART);
		break;
	case MOVE_RIGHT:
		raw_drive(MOTOR_FORWARD, MOTOR_FORWARD, SPEED_STANDART, SPEED_STANDART * 0.7);
		break;
	case MOVE_LEFT_ROTATE:
		raw_drive(MOTOR_BACKWARD, MOTOR_FORWARD, SPEED_STANDART * 0.7, SPEED_STANDART * 0.7);
		break;
	case MOVE_RIGHT_ROTATE:
		raw_drive(MOTOR_FORWARD, MOTOR_BACKWARD, SPEED_STANDART * 0.7, SPEED_STANDART * 0.7);
		break;
	case MOVE_LEFT_ROTATE_FAST:
		raw_drive(MOTOR_BACKWARD, MOTOR_FORWARD);
		break;
	case MOVE_RIGHT_ROTATE_FAST:
		raw_drive(MOTOR_FORWARD, MOTOR_BACKWARD);
		break;
	case MOVE_LAST:
	default:
		Serial.println("Warning: Movement state error");
		break;
	}

	return;
}


unsigned char parse_linesensors(int times)
{
	unsigned char result = 0;
	(LOW == digitalRead(SENSOR_RIGHT)) ? SET_FLAG(result, SENSORFLAG_RIGHT) : RESET_FLAG(result, SENSORFLAG_RIGHT);
	(LOW == digitalRead(SENSOR_RIGHT_MIDDLE)) ? SET_FLAG(result, SENSORFLAG_RIGHT_MIDDLE) : RESET_FLAG(result, SENSORFLAG_RIGHT_MIDDLE);
	(LOW == digitalRead(SENSOR_LEFT_MIDDLE)) ? SET_FLAG(result, SENSORFLAG_LEFT_MIDDLE) : RESET_FLAG(result, SENSORFLAG_LEFT_MIDDLE);
	(LOW == digitalRead(SENSOR_LEFT)) ? SET_FLAG(result, SENSORFLAG_LEFT) : RESET_FLAG(result, SENSORFLAG_LEFT);
	if (times == 0) {
		return result;
	}
	return parse_linesensors(times - 1);
}

void srf02()
{
	Wire.beginTransmission(112);    //entspricht Adresse 0xE0
	Wire.write(byte(0x00));         //Befehlsregister 0x00
	Wire.write(byte(0x51));         //Werte in cm
	Wire.endTransmission();
	delay(70);
	Wire.beginTransmission(112);    //entspricht Adresse 0xE0
	Wire.write(byte(0x02));         //Befehlsregister 0x00
	Wire.endTransmission();
	Wire.requestFrom(112, 2);        //2 bytes von Adresse 112
	if (2 <= Wire.available())
	{
		f = Wire.read();
		f = f << 8;
		f |= Wire.read();
	}
}

void loop()
{
	static TASKSTAKE task = TASK_MOVE;
	static long taskTimer = 0;
	static long time = millis();
	static EDGESTATE edgeHelper = EDGE_NONE;
	unsigned char flags = parse_linesensors();
	srf02();
	
	
	flags = parse_linesensors(1);
	switch (task)
	{
	case TASK_DONE:
		taskTimer = 0;
		break;
	case TASK_MOVE:
		
		drive(MOVE_FORWARD);
		if (flags != 0) {
			Serial.println("Entering Correction mode");
			if (CHECK_FLAG(flags, SENSORFLAG_LEFT) || CHECK_FLAG(flags, SENSORFLAG_RIGHT)) {
				task = TASK_EDGE_CORRECTION;
				taskTimer = 0;
				Serial.println("Correction mode edge");
				Serial.println(flags);
			}
			else {
				task = TASK_LINE_CORRECTION;
				taskTimer = 0;
				Serial.println("Correction mode line");
				Serial.println(flags);
			}
		}

		break;
	case TASK_LINE_CORRECTION:
		

		if (CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			task = TASK_EDGE_CORRECTION;
			taskTimer = 0;
			break;
		}
		else if (CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && !CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			drive(MOVE_LEFT);
		}
		else if (!CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			drive(MOVE_RIGHT);
		}
		else {
			task = TASK_MOVE;
			taskTimer = 0;
		}

		if (taskTimer >= LINE_CORRECTION_MAX_TIME) {
			task = TASK_LINE_CORRECTION_CRITICAL;
			taskTimer = 0;
			break;
		}

		break;
	case TASK_LINE_CORRECTION_CRITICAL:

		if (CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			taskTimer = 0;
			break;
		}
		else if (CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && !CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			(LINE_CORRECTION_CRITICAL_MAX_TIME >= taskTimer) ? drive(MOVE_LEFT_ROTATE) : drive(MOVE_LEFT_ROTATE_FAST);
		}
		else if (!CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) && CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
			(LINE_CORRECTION_CRITICAL_MAX_TIME >= taskTimer) ? drive(MOVE_RIGHT_ROTATE) : drive(MOVE_RIGHT_ROTATE_FAST);
		}
		else {
			task = TASK_LINE_CORRECTION; //From here we only fall back to line-correction-mode. 
			taskTimer = 0;
		}
		

		break;
	case TASK_EDGE_CORRECTION:
		task = TASK_EDGE_CORRECTION;
		if (CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE) || CHECK_FLAG(flags, SENSORFLAG_LEFT)) {
			edgeHelper = EDGE_LEFT;
		}
		else if (CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE) || CHECK_FLAG(flags, SENSORFLAG_RIGHT)) {
			edgeHelper = EDGE_RIGHT;
		}

		task = TASK_EDGE_CORRECTION_RUNNING;
		taskTimer = 0;
		break;
	case TASK_EDGE_CORRECTION_RUNNING:
		drive(MOVE_FORWARD);
		if (flags == 0) {
			task = TASK_EDGE_CORRECTION_FINALISE;
			taskTimer == 0;
		}
		break;
	case TASK_EDGE_CORRECTION_FINALISE:
		switch (edgeHelper)
		{
		case EDGE_LEFT:
			if (CHECK_FLAG(flags, SENSORFLAG_RIGHT) || CHECK_FLAG(flags, SENSORFLAG_RIGHT_MIDDLE)) {
				task = TASK_MOVE;
				break;
			}
			else {
				drive(MOVE_LEFT_ROTATE);
			}
			break;
		case EDGE_RIGHT:
			if (CHECK_FLAG(flags, SENSORFLAG_LEFT) || CHECK_FLAG(flags, SENSORFLAG_LEFT_MIDDLE)) {
				task = TASK_MOVE;
				break;
			}
			else {
				drive(MOVE_RIGHT_ROTATE);
			}
			break;

		case EDGE_NONE:
		default:
			break;
		}

		break;

	case TASK_OBSTACLE:
		taskTimer = 0;
		break;
	case TASK_LINE_DECISION:
		taskTimer = 0;
		task = TASK_MOVE;
		break;
	case TASK_LAST:
		break;
	default:
		break;
	}

	//Update Task timer:
	taskTimer += millis() - time;
	time = millis();

	//Parse debug inputs
	if (Serial.available() != 0) {
		String input = Serial.readString();
		if (input == "Reset") {
			Serial.println("Resetting...");
			task = TASK_MOVE;
			taskTimer = 0;
		}
	}
}


