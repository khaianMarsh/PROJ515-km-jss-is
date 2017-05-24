/* 
 *  return codes
 *  0=no return
 *  1=command completed
 *  101=system ready
 *  254=limit switch hit/system locked
 *  253=limit switch already depressed,home command failed
 *  252=pitch out of range
 *  251=lin out of range
 *  250=yaw out of range
 *  249=command not found
 *  248=attempt to set invalid timeing
*/

/*arm weight 1.57kg
*. 32M from center
* 5.02nm
* lin motor 1.1kg
* 0.09m
* 0.99nm
*/

//direction defs
#define pitchUp true
#define pitchDown false
#define linForwards false
#define linBackwards true
#define yawRight false
#define yawLeft true

#define pitchOnly true,false,false
#define linOnly false,true,false
#define yawOnly false,false,true

//pin definitions-------------------------------------------------------------------------------------------------
#define pitch_ena 4
#define pitch_pul 10
#define pitch_dir 2
#define RELAY_PIN A3
#define yaw_ena 12
#define yaw_pul 11
#define yaw_dir 13

#define lin_ena 7
#define lin_pul 8
#define lin_dir 9

#define limitSwitch 6
//pins 3, 5 are n/c

#define linearMinium 0 //mm from pitch pivot
#define linearMaximum 670 //mm from the pitch pivot
#define linearOffset 168//mm offset of the linear 0 to the pitch pivot
#define carteasianHome 500, 0, 440

#define Z_OFFSET 440 //mm from the height of the pitch pivot to the bottom of the base
#define CARTEASIAN_STOP_FLAG 1
#define SERVO_DRIVER_ADDRESS 0x40

#define PITCH_SERVO_MIN 148
#define PITCH_SERVO_MAX 611

#define YAW_SERVO_MIN 138
#define YAW_SERVO_MAX 598

#define TOP_EXTENSION_SERVO_MIN 150
#define TOP_EXTENSION_SERVO_MAX 550

#define BOTTOM_EXTENSION_SERVO_MIN 130
#define BOTTOM_EXTENSION_SERVO_MAX 550

#define LED_MIN 0
#define LED_MAX 4095

#define CAMERA_SERVO_MIN 180
#define CAMERA_SERVO_MAX 280

#define MOTOR_FORWARDS 0
#define MOTOR_BACKWARDS 1

//Servo Driver pins----------------------------------------------------------------------------------------------
#define TOP_EXTENSION_PIN 0
#define BOTTOM_EXTENSION_PIN 1

#define LED_PIN 4

#define CAMERA_SERVO_PIN 8

#define H_BRIDGE_PIN_IA 9
#define H_BRIDGE_PIN_IB 10

#define PITCH_SERVO_PIN 14
#define YAW_SERVO_PIN 15
//position counting----------------------------------------------------------------------------------------------

long pitchPos = 0;
byte pitchMove = 1;
long yawPos = 0;
byte yawMove = 1;
long linPos = 0;
byte linMove = 1;

int pitchAngle = 0;
int linearValue = 0;
int yawAngle = 0;

int x_Coord = 400;
int y_Coord = 0;
int z_Coord = 160;
int x_targetCoord = 400;
int y_targetCoord = 0;
int z_targetCoord = 160;

float servoPitch = 0;
float servoYaw = 0;


//gearing constants-------------------------------------------------------------------------------------------------

//conversion values for pulse to angle/linear distance
const float pitchGearing = 0.00796568626;//360*(1/200)*(1/8)*(1/15.3)*(13/24) 200pulses/revolution, 1/8th microstepping,1:15.3 gearbox,13 to 24 tooth sprcoket, output in degrees
const float yawGearing = 0.00735294117;//  360*(1/200)*(1/16)*(1/15.3) 200pulses/revolution, 1/16th microstepping,1:15.3 gearbox,output in degrees
const float linGearing = 0.0215985;  //(1/200)*(1/16)*22*(3.1416) 200pulses/revolution, 1/16th microstepping,22 tooth spur gear,3.1614 mm per tooth,output in mm
//kinomatic constants
const float forkH = 150.6;//horizontal length of fork
const float forkV = 150.6;//horizontal height of fork
const float minLin = 284;//126+158;//The minium distance between pitch pivot and end of linear arm can be

//misc------------------------------------------------------------------------------------------------------------------

boolean verboseOutput = true;
int pulseTime = 1000;
int minPulseTime = 100;
int maxPulseTime = 800;
int absMinPulseTime = 100;
int absMaxPulseTime = 800;

int homePulseTime = 1000;
const float pi = 3.14159;

