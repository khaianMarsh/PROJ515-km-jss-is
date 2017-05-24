#include "highLevel.h"

//comands

//basic commands
#define cmd_enable 1
#define cmd_direction 2
#define cmd_pulse 3
#define cmd_delay 4


//homeing commands
#define cmd_home 10
#define cmd_pitchHome 11
#define cmd_linHome 12
#define cmd_yawHome 13

//movement commands
#define cmd_movePoint 20
#define cmd_moveAngle 21
#define cmd_moveJoints 22
#define cmd_extendGripper 23
#define cmd_led 24
#define cmd_cameraAngle 25
#define cmd_motorDriverForwards 26
#define cmd_motorDriverBackwards 27

//return commands
#define get_position 31
#define get_gearing 32
#define get_pulseposition 33

//setttings
#define cmd_timeing 40
#define cmd_mintimeing 41
#define cmd_maxtimeing 42
#define cmd_absmintimeing 43
#define cmd_absmaxtimeing 44
#define cmd_verbose 45

//misc 
#define testcommand 69

int x_target, y_target, z_target;
int x_position, y_position, z_position;

void blinkLED(int flashes, int msDelay){
    for(int i = 0; i < flashes; i++){
        digitalWrite(13, HIGH);
        delay(msDelay);
        digitalWrite(13, LOW);
        delay(msDelay);
    }
}
void setup() { 
    Serial.begin(57600);
    while (!Serial) {//wait for connection
        delay(1);
    }
    delay(100);
    serialClear();  
    debugPrint("serial online", 0);
    debugPrint("verbose output: " + String(verboseOutput),0);
    debugPrint("initalising", 0);
    //enable all pins and disable drivers
    for (int i=2; i<13; i++)  if (i!=limitSwitch) pinMode(i, OUTPUT);
    pinMode(limitSwitch,INPUT);
    setEnable(false);
    setTimeing(200);
    dir(B000);

    setServoDriver();
  
    debugPrint("system ready",101);
    Serial.println("Homing in 2 second");
//    delay(1000);
    setEnable(true);
    moveEndEffectorServos(0 * PI / 180, 0 * PI / 180);//Home the end effector servos
    extendGripper(10);
    gripperLEDs(30);
    cameraServoAngle(0 * pi / 180);
    motorDriver(MOTOR_FORWARDS, 10);
    //motorDriver(MOTOR_BACKWARDS, 30);
    //sweepServo(150, 400, 12, 5);
//    delay(1000);
//    movePitchHome();
//    moveLinHome();
//    moveYawHome();
    Serial.print("flag");//2 right 1 builtin 0 left
}

void loop() {
    //wait until a command is present in the serial buffer
//    while(true){
//        digitalWrite(RELAY_PIN, HIGH);
//        delay(5);
//        digitalWrite(RELAY_PIN, LOW);
//        delay(50);
//    }
    while (Serial.available() == 0){//while (Serial.available() != 1){
        delay(1);
    }
    
    byte instruction = Serial.read(); 
    //blinkLED(instruction, 800);
    
    //give handaling to correct command to establish any remaining bytes needed
    switch (instruction){
        case cmd_enable: {
            debugPrint("command recived: enable/disable",0);
            
            while (Serial.available() != 1){}
            bool state = (Serial.read() == 1); 
            setEnable(state);
            if (state==1){
                debugPrint("motors enabled",0);
            }
            else{          
                debugPrint("motors disabled",0);
            }
        }break;
        case cmd_movePoint:{//movement---------------------------------------------------------
            debugPrint("command recived: move arm to point", 0);
            while (Serial.available() < 6){}
            x_target = (Serial.read() << 8) + Serial.read();
            y_target = (Serial.read() << 8) + Serial.read();
            z_target = (Serial.read() << 8) + Serial.read();
            Serial.flush();
            moveArmCartesian(x_target, y_target, z_target);       
        }break;
        case cmd_moveAngle:{
            debugPrint("command recived:move arm to angle",0);
            while (Serial.available() < 6){}
                float pitch = ((Serial.read() << 8) + Serial.read()) / 100;  
                float lin = ((Serial.read() << 8) + Serial.read()); 
                float yaw = ((Serial.read() << 8) + Serial.read()) / 100; 
                Serial.flush();
                moveArmJoints(pitch, lin, yaw);
        }break;
        case cmd_moveJoints:{
            while (Serial.available() < 10){}
                float pitch = (float)((Serial.read() << 8) + Serial.read()) / 5000;  
                float linear = ((Serial.read() << 8) + Serial.read()); 
                float yaw = (float)((Serial.read() << 8) + Serial.read()) / 5000;
                float servoPitch = (float)((Serial.read() << 8) + Serial.read()) / 5000;
                float servoYaw = (float)((Serial.read() << 8) + Serial.read()) / 5000; 
                Serial.flush();
                moveArmJoints(pitch, linear, yaw);
                moveEndEffectorServos(servoPitch, servoYaw);
                delay(200);//delay to ensure the servos are in position
                Serial.print("flag");
        }break;
        case cmd_extendGripper:{
            while (Serial.available() < 2){}
            int extension = ((Serial.read() << 8) + Serial.read());  
            Serial.flush();
            extendGripper(extension);
            Serial.print("flag");
        }break;
        case cmd_led:{
            while (Serial.available() < 2){}
            int brightness = ((Serial.read() << 8) + Serial.read());  
            Serial.flush();
            gripperLEDs(brightness);
        }break;
        case cmd_cameraAngle:{
            while (Serial.available() < 2){}
            float cameraAngle = (float)((Serial.read() << 8) + Serial.read()) / 5000; 
            Serial.flush();
            cameraServoAngle(cameraAngle);
        }break;
        case cmd_motorDriverForwards:{
            while (Serial.available() < 2){}
            int motorDutyCycle = (Serial.read() << 8) + Serial.read(); 
            Serial.flush();
            motorDriver(MOTOR_FORWARDS, motorDutyCycle);
        }break;
        case cmd_motorDriverBackwards:{
            while (Serial.available() < 2){}
            int motorDutyCycle = (Serial.read() << 8) + Serial.read(); 
            Serial.flush();
            motorDriver(MOTOR_BACKWARDS, motorDutyCycle);
        }break;
    }
}


//void loop() {
//    //wait until a command is present in the serial buffer
//    while (Serial.available() == 0){//while (Serial.available() != 1){
//        delay(1);
//    }
//    
//    byte instruction = Serial.read(); 
//    //blinkLED(2, 800);
//    
//    //give handaling to correct command to establish any remaining bytes needed
//    switch (instruction){
//        case cmd_enable: {
//            debugPrint("command recived: enable/disable",0);
//            
//    		while (Serial.available() != 1){}
//    		bool state = (Serial.read() == 1); 
//    		setEnable(state);
//            if (state==1){
//                debugPrint("motors enabled",0);
//            }
//            else{          
//                debugPrint("motors disabled",0);
//            }
//        }break;
//        case cmd_direction:{
//            debugPrint("command recived:set motor direction",0);
//        		while (Serial.available() !=1){}
//        		byte motors = Serial.read(); 
//                //blinkLED(motors, 200);
//                dir(motors);
//                debugPrint("diretion set",0);
//        } break;
//        case cmd_pulse: {
//            debugPrint("command recived:pulse motors",0);
//        		while (Serial.available() != 3){}
//        		byte motors = Serial.read(); 
//        		unsigned int iteration=(Serial.read() << 8) + Serial.read();
//                pulse(motors&B100 >> 2 ,motors&B10 >> 1, motors&B10, iteration, false);   
//                debugPrint("motors pulsed: " + String(iteration), 0); 
//        }break;
//        case cmd_delay:{
//            debugPrint("command recived:system delay",0);
//        		while (Serial.available() !=2){}
//        		unsigned int value=(Serial.read() << 8) + Serial.read();
//        		sysDelay(value);
//        }break;   
//    	case cmd_home:{//homeing----------------------------------------------------------------------
//            debugPrint("command recived:move home", 0);
//            setEnable(true);
//            movePitchHome();
//            moveLinHome();
//            moveYawHome();
//        }break;
//        case cmd_pitchHome:{
//            debugPrint("command recived:move pitch home", 0);
//            movePitchHome();
//        }break; 
//        case cmd_linHome:{
//            debugPrint("command recived:move lin home", 0);
//            moveLinHome();
//        }break;
//        case cmd_yawHome:{
//            debugPrint("command recived:move yaw home", 0);
//            moveYawHome();
//        }break;
//        case cmd_movePoint:{//movement---------------------------------------------------------
//            debugPrint("command recived: move arm to point", 0);
//            while (Serial.available() != 6){}
//            x_target = (Serial.read() << 8) + Serial.read();
//            y_target = (Serial.read() << 8) + Serial.read();
//            z_target = (Serial.read() << 8) + Serial.read();
//            moveArmCartesian(x_target, y_target, z_target);       
//        }break;
//        case cmd_moveAngle:{
//            debugPrint("command recived:move arm to angle",0);
//            while (Serial.available() !=6){}
//        		float pitch = ((Serial.read() << 8) + Serial.read()) / 100;  
//        		float lin = ((Serial.read() << 8) + Serial.read()); 
//        		float yaw = ((Serial.read() << 8) + Serial.read()) / 100; 
////        		movearmraw(pitch,lin,yaw);
//                moveArmJoints(pitch, lin, yaw);
//        }break;
//        case get_position:{ //feedback commands------------------------------------------------------------
//            debugPrint("Pitch:" + String(pitchPos * pitchGearing, 10), pitchPos * pitchGearing);
//            debugPrint("lin  :" + String(linPos * linGearing,10), linPos * linGearing);
//            debugPrint("yaw  :" + String(yawPos * yawGearing,10), yawPos * yawGearing);
//    	}break;
//        case get_gearing:{
//            debugPrint("Pitch gearing : " + String(pitchGearing, 10), pitchGearing);
//            debugPrint("lin gearing : " + String(linGearing, 10), linGearing);
//            debugPrint("yaw gearing : " + String(yawGearing, 10), yawGearing);
//        }break;
//        case get_pulseposition:{
//            debugPrint("raw Pitch:" + String(pitchPos, 10), pitchPos);
//            debugPrint("raw lin  :" + String(linPos, 10), linPos);
//            debugPrint("raw yaw  :" + String(yawPos, 10), yawPos);
//        }break;
//        case cmd_timeing: {//settings-------------------------------------------------------------------- 
//            debugPrint("command recived:set pulse timing",0);
//        		while (Serial.available() !=2){}
//        		unsigned int value=(Serial.read()<<8)+Serial.read();
//            setTimeing(value);
//            debugPrint("pulse time set to: "+String(value),0);  
//        }break;
//        case cmd_mintimeing: {
//            debugPrint("command recived:set pin pulse timing",0);
//            while (Serial.available() !=2){}
//            unsigned int value=(Serial.read()<<8)+Serial.read();
//            setMinTimeing(value);
//            debugPrint("min pulse time set to: "+String(value),0);  
//        }break;
//        case cmd_maxtimeing: {
//            debugPrint("command recived:set max pulse timing",0);
//            while (Serial.available() != 2){}
//            unsigned int value=(Serial.read() << 8)+Serial.read();
//            setMaxTimeing(value);
//            debugPrint("max pulse time set to: "+String(value),0);  
//        }break;   
//    	  case cmd_absmintimeing: {
//            debugPrint("command recived:set pin pulse timing",0);
//            while (Serial.available() != 2){}
//            unsigned int value=(Serial.read() << 8)+Serial.read();
//            setAbsMinTimeing(value);
//            debugPrint("abs min pulse time set to: "+String(value),0);  
//        }break;
//        case cmd_absmaxtimeing: {
//            debugPrint("command recived:set max pulse timing",0);
//            while (Serial.available() != 2){}
//            unsigned int value=(Serial.read() << 8)+Serial.read();
//            setAbsMaxTimeing(value);
//            debugPrint("abs max pulse time set to: "+String(value),0);  
//        }break;
//    	  case cmd_verbose:{
//            debugPrint("command recived:change verbose output",0);
//            while (Serial.available() !=1){}  
//            changeVerbose(Serial.read()==1);
//        }break;
//        case testcommand:{//misc-----------------------------------------------------------------------
//            setEnable(true);
//            movePitchHome();
//            moveLinHome();
//            moveYawHome();
//            //moveArmCartesian(carteasianHome);
//            while(1){
//                moveArmCartesian(500, 300, 300 + 150);
//                moveArmCartesian(500, -300, 300 + 150);
//                moveArmCartesian(500, -300, 300 - 150);
//                moveArmCartesian(500, 300, 300 - 150);
//                                
//                float sinVar = 0;
//                float cosVar = 0;
//                float radius = 250;
//                
//                for(int i = 0; i < 360; i += 5){
//                    sinVar = radius * sin(i * pi / 180);
//                    cosVar = radius * cos(i * pi / 180);
//                    moveArmCartesian(750, sinVar, cosVar + 300);
//                }
//            }
//        }break;
//        default:
//            debugPrint(" command not found",249);
//        break;
//    }
//    //send confirmation that command is done and that the sytem is ready for a new one
//    debugPrint("command done",1);
//}
