#ifndef NAMES_HEADER
#define NAMES_HEADER

//Pin definitions
#define MOTOR_L_DIRECTION	8	//Direktion of the left motor
#define MOTOR_L_SPEED		6	//Speed of the right motor
#define MOTOR_R_DIRECTION	9	//Direktion of the right motor
#define MOTOR_R_SPEED		7	//Speed of the right motor

#define SENSOR_LEFT			27	//Left	Linesensor
#define SENSOR_LEFT_MIDDLE	26	//Sensor right from the left sensor (left-middle)
#define SENSOR_RIGHT_MIDDLE	25	//Sensor left from the right sensor (right-middle)
#define SENSOR_RIGHT		24	//Right Linesensor

#define PIN_UNKNOWN			14	//Unknown funktion

//Sets flag f in x
#define SET_FLAG( x, f )  ( x |= (1<<f) )
//Delets flag f in x
#define RESET_FLAG( x, f ) ( x &= ~(1<<f) )
//Checks flag f in x
#define CHECK_FLAG( x, f ) ( x & (1<<f) )



//Serial Feedback
#define SERIAL_BAUDRATE		9600

//Driving

//Motor Directions
enum MOTORSTATE
{
	MOTOR_STILL = 0x0,
	MOTOR_FORWARD,
	MOTOR_BACKWARD,
	MOTOR_LAST
};

#define SPEED_STANDART			65		//Normal motorspeed
#define SPEED_MODIFIER_LEFT		1.0f	//Left Speedmodifier
#define SPEED_MODIFIER_RIGHT	1.0f	//Right Speedmodifier
#define BLOCK_MOVEMENTS		false

//Movement types
enum MOVEMENTSTATE
{
	MOVE_FORWARD		= 0x0,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_LEFT_ROTATE,
	MOVE_RIGHT_ROTATE,
	MOVE_LEFT_ROTATE_FAST,
	MOVE_RIGHT_ROTATE_FAST,
	MOVE_LAST
};

//Sensorflags
#define SENSORFLAG_LEFT			0
#define SENSORFLAG_LEFT_MIDDLE	1
#define SENSORFLAG_RIGHT_MIDDLE 2
#define SENSORFLAG_RIGHT		3
#define SENSORFLAG_INVALID		7

//Task to do. Like move forward, correct line, ...
enum TASKSTAKE
{
	TASK_DONE = 0x0,
	TASK_MOVE,
	TASK_LINE_CORRECTION,
	TASK_LINE_CORRECTION_CRITICAL,
	TASK_EDGE_CORRECTION,
	TASK_EDGE_CORRECTION_RUNNING,
	TASK_EDGE_CORRECTION_FINALISE,
	TASK_OBSTACLE,
	TASK_LINE_DECISION,

	TASK_LAST
};

enum EDGESTATE
{
	EDGE_LEFT = 0x0,
	EDGE_RIGHT,
	EDGE_NONE
};
//Line correction constant values
#define LINE_CORRECTION_MAX_TIME			40
#define LINE_CORRECTION_CRITICAL_MAX_TIME	1200
#define EDGE_CORRECTION_MAX_TIME			300	

void raw_drive(MOTORSTATE left, MOTORSTATE right, int sppedL, int speedR);
void drive(MOVEMENTSTATE movement);
unsigned char parse_linesensors(int times = 0);

#endif //NAMES_HEADER