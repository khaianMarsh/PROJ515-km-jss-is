#include "kino.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(SERVO_DRIVER_ADDRESS);

void setServoDriver(){
    servoDriver.begin();
    servoDriver.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void moveEndEffectorServos(float pitch, float yaw){
//    Serial.print("pitch = ");
//    Serial.println(pitch);
//    Serial.print("yaw = ");
//    Serial.println(yaw);
    int pitchServoPulse = round(mapNumber(pitch, -pi / 2, pi / 2, PITCH_SERVO_MIN, PITCH_SERVO_MAX ));
    int yawServoPulse = round(mapNumber(yaw, -pi / 2, pi / 2, YAW_SERVO_MIN, YAW_SERVO_MAX ));
//    Serial.print("ppulse = ");
//    Serial.println(pitchServoPulse);
//    Serial.print("ypulse = ");
//    Serial.println(yawServoPulse);
    servoDriver.setPWM(PITCH_SERVO_PIN, 0, pitchServoPulse);
    servoDriver.setPWM(YAW_SERVO_PIN, 0, yawServoPulse);
}

void moveEndEffectorServosSlow(float pitch, float yaw){
    float pitchDiff = pitch * 180 / PI - servoPitch * 180 / PI;
    float yawDiff = yaw * 180 / PI - servoYaw * 180 / PI;
    
    float steps = pitchDiff > yawDiff ? pitchDiff : yawDiff;

    int stepsVar = round(steps * 180 / PI);
    
    for(int i = 1; i <= stepsVar; i ++){
        float pitchVar = servoPitch + (pitchDiff * i) / steps;//Might need to type cast these to floats
        float yawVar = servoYaw + (yawDiff * i) / steps;
        int pitchServoPulse = round(mapNumber(pitchVar, -pi / 2, pi / 2, PITCH_SERVO_MIN, PITCH_SERVO_MAX ));
        int yawServoPulse = round(mapNumber(yawVar, -pi / 2, pi / 2, YAW_SERVO_MIN, YAW_SERVO_MAX ));
        servoDriver.setPWM(PITCH_SERVO_PIN, 0, pitchServoPulse);
        servoDriver.setPWM(YAW_SERVO_PIN, 0, yawServoPulse);
        delay(100);
    }
}

void extendGripper(int extension){
    int topExtensionServoPulse = round(mapNumber(extension, 0, 100, TOP_EXTENSION_SERVO_MIN, TOP_EXTENSION_SERVO_MAX ));
    int bottomExtensionServoPulse = round(mapNumber(extension, 100, 0, BOTTOM_EXTENSION_SERVO_MIN,  BOTTOM_EXTENSION_SERVO_MAX ));
    servoDriver.setPWM(TOP_EXTENSION_PIN, 0, topExtensionServoPulse);
    servoDriver.setPWM(BOTTOM_EXTENSION_PIN, 0, bottomExtensionServoPulse);
}

void gripperLEDs(int brightness){
    int LEDPWM = round(mapNumber(brightness, 0, 100, LED_MIN, LED_MAX ));
    servoDriver.setPWM(LED_PIN, 0, LEDPWM);
}

void cameraServoAngle(float angle){
    int cameraAnglePulse = round(mapNumber(angle, 40 * pi /180, 0, CAMERA_SERVO_MIN, CAMERA_SERVO_MAX));
    servoDriver.setPWM(CAMERA_SERVO_PIN, 0, cameraAnglePulse);
}

void motorDriver(int motorDirection, int dutyCycle){
    int dutyPulse = round(mapNumber(dutyCycle, 0, 100, 0, 4095));
    
    if(motorDirection == MOTOR_FORWARDS){
        servoDriver.setPWM(H_BRIDGE_PIN_IA, 0, dutyPulse);
        servoDriver.setPWM(H_BRIDGE_PIN_IB, 0, 0);
    }
    else if(motorDirection == MOTOR_BACKWARDS){
        servoDriver.setPWM(H_BRIDGE_PIN_IA, 0, 0);
        servoDriver.setPWM(H_BRIDGE_PIN_IB, 0, dutyPulse);    
    }
}

void sweepServo(int minimum, int maximum, int servoPin, int msDelay){
    int pulse;
    while(true){
        for(int i = 0; i <= 180; i++){
            pulse = round(mapNumber(i, 0, 180, minimum, maximum));
            servoDriver.setPWM(servoPin, 0, pulse);
            delay(msDelay);
        }
        for(int i = 180; i >= 0; i--){
            pulse = round(mapNumber(i, 0, 180, minimum, maximum));
            servoDriver.setPWM(servoPin, 0, pulse);
            delay(msDelay);
        }
    }
}

void movePitchHome(){
    if (digitalRead(limitSwitch) != 1){
        debugPrint("limit switch already active",0);  
        debugPrint("home command failed", 253);  
        return;
    }
    int  tmp = pulseTime;
    setTimeing(homePulseTime); 
    debugPrint("moving pitch to limit switch",0);
    niceDir(pitchUp, false, false);
    while (digitalRead(limitSwitch) == 1){
        pulse(pitchOnly, 1, true);
    }
    niceDir(pitchDown, false, false);
    delay(100);
    while (digitalRead(limitSwitch) != 1){
        pulse(pitchOnly, 200, true);
    }
    pulse(pitchOnly, 17 / pitchGearing, false);  
    pulseTime = tmp;
    pitchPos = 0;
    setTimeing(tmp);
    debugPrint("home complete", 0);  
}    

void moveLinHome(){
    if (digitalRead(limitSwitch) != 1){
        debugPrint("limit switch already active", 0);  
        debugPrint("home command failed", 253);  
        return;
    }
  
    int  tmp = pulseTime;
    setTimeing(homePulseTime); 
    debugPrint("moving lin to home position",0);
    niceDir(false, linForwards, false);
    while (digitalRead(limitSwitch) == 1){
        pulse(linOnly, 1, true);
    }
    niceDir(false, linBackwards, false);
    delay(100);
    while (digitalRead(limitSwitch) != 1){
        pulse(linOnly, 400, true);
    }
    pulse(linOnly, 668 / linGearing, false);  
    pulseTime = tmp;
    linPos = 0;
    setTimeing(tmp);
    debugPrint("home complete", 0);   
}

void moveYawHome(){
    if (digitalRead(limitSwitch) != 1){
        debugPrint("limit switch already active", 0);  
        debugPrint("home command failed", 253);  
        return;
    }
    int  tmp = pulseTime;
    setTimeing(homePulseTime);  
    debugPrint("moving yaw to limit switch", 0);
    niceDir(false, false, yawLeft);
    while (digitalRead(limitSwitch) == 1){
        pulse(yawOnly, 1, true);
    }
    niceDir(false, false, yawRight);
    delay(100);
    while (digitalRead(limitSwitch) != 1){
        pulse(yawOnly, 400, true);
    }
    pulse(yawOnly, 177 / yawGearing, false);
    pulseTime = tmp;
    yawPos = 0;
    setTimeing(tmp);//TODO remove duplicate code pulseTime = tmp;
    debugPrint("home complete", 0);
}

void moveArmJoints(float pitch, float linear, float yaw){
    if (testangles(pitch, linear, yaw) == false) return;//test for a valid position
    
    //convert angles to pulse count
    long pitchPulseCount = round((pitch * (180 / pi)) / pitchGearing);//pulse count at the target position
    long linearPulseCount = round(linear / linGearing); 
    long yawPulseCount = round((yaw * (180 / pi)) / yawGearing); 
//    Serial.print("DEBUG: yawPulseCount = ");
//    Serial.println(yawPulseCount);

    long pitchPulses = pitchPulseCount - pitchPos;//pulses required to get to the target position
    long linearPulses = linearPulseCount - linPos;//pulses required to get to the target position
    long yawPulses = yawPulseCount - yawPos;//pulses required to get to the target position

//    Serial.print("DEBUG: in ikine yawPos = ");
//    Serial.println(yawPos);
    
    //establish direction for each motor
    bool pitchDirection = pitchPulses < 0 ? pitchDown : pitchUp;     
    bool linearDirection = linearPulses > 0 ? linForwards : linBackwards;  
    bool yawDirection = yawPulses > 0 ? yawLeft : yawRight;  
    
    niceDir(pitchDirection, linearDirection, yawDirection);//Sets the correct directions

    pitchPulses = abs(pitchPulses);//Absolute number of pulases required
    linearPulses = abs(linearPulses);//Absolute number of pulases required
    yawPulses = abs(yawPulses);//Absolute number of pulases required
//    Serial.print("DEBUG: pitchPulses = ");
//    Serial.println(pitchPulses);
//    Serial.print("DEBUG: linearPulses = ");
//    Serial.println(linearPulses);
//    Serial.print("DEBUG: yawPulses = ");
//    Serial.println(yawPulses);
//    Serial.println("");

    //Sets larargetNumberOfPulases to the maximum number of pulases needed to reach a target position
    unsigned long larargetNumberOfPulases = pitchPulses > linearPulses ? pitchPulses : linearPulses;
    larargetNumberOfPulases = larargetNumberOfPulases > yawPulses ? larargetNumberOfPulases : yawPulses;
//    Serial.print("DEBUG: larargetNumberOfPulases = ");
//    Serial.println(larargetNumberOfPulases);
    
    //gets the number of pulses per step so all motor reach the target simultaneously
    double pitchPulseSpace = larargetNumberOfPulases + 1;
    double linearPulseSpace = larargetNumberOfPulases + 1;
    double yawPulseSpace = larargetNumberOfPulases + 1;
    
    if(pitchPulses != 0) pitchPulseSpace = (double)larargetNumberOfPulases / (double)pitchPulses;
    if(linearPulses != 0) linearPulseSpace = (double)larargetNumberOfPulases / (double)linearPulses;
    if(yawPulses != 0) yawPulseSpace = (double)larargetNumberOfPulases / (double)yawPulses;
    
//    Serial.print("DEBUG: pitchPulseSpace = ");
//    Serial.println(pitchPulseSpace);
//    Serial.print("DEBUG: linearPulseSpace = ");
//    Serial.println(linearPulseSpace);
//    Serial.print("DEBUG: yawPulseSpace = ");
//    Serial.println(yawPulseSpace);

    int pitchTempCount = 0;
    int linearTempCount = 0;
    int yawTempCount = 0;

    for (unsigned long i = 0; i < larargetNumberOfPulases; i++){
        //calculate speed of next pulse
        float percentDone = (float(i) / float(larargetNumberOfPulases)) * 100;
        byte moveSpeed = percentDone > 50 ? round(sq(percentDone - 50) / 25.0) : round(sq(50 - percentDone) / 25.0);

        int targetspeed = moveSpeed * ((maxPulseTime - minPulseTime) / 100) + minPulseTime;
        int recommendedSpeed = recommendedTimeing(pitchPos, linPos);
        
        if(targetspeed < recommendedSpeed){//limits and sets the speed
            setTimeing(targetspeed);
        }
        else{
            setTimeing(recommendedSpeed); 
        }

        bool pitchPulseFlag = false;
        bool linearPulseFlag = false;
        bool yawPulseFlag = false;

        if( i >= pitchPulseSpace * pitchTempCount && pitchPulses != 0){
            pitchTempCount++;
            pitchPulseFlag = true;
        }
        
        if( i >= linearPulseSpace * linearTempCount && linearPulses != 0){
            linearTempCount++;
            linearPulseFlag = true;
        }
        
        if( i >= yawPulseSpace * yawTempCount && yawPulses != 0){
            yawTempCount++;
            yawPulseFlag = true;
        }
        pulse(pitchPulseFlag, linearPulseFlag, yawPulseFlag, 1, false);
    }
    x_Coord = x_targetCoord;//After a move update the coordinates to target coordinate
    y_Coord = y_targetCoord;//After a move update the coordinates to target coordinate
    z_Coord = z_targetCoord;//After a move update the coordinates to target coordinate
}

void moveArmCartesian(float x, float y, float z){
    //moves the arm to a given cartesian positon
    float jointPositions[3];//declaring the array for the target joint positions
    inverseKinomatics(x, y, z, jointPositions);//Calculates the joint angles required
    float pitch = jointPositions[0];
    float linear = jointPositions[1];
    float yaw = jointPositions[2];
//    Serial.println("After inverseKinematics.........................................");
//    Serial.print("Pitch = ");
//    Serial.println(pitch);
//    Serial.print("Linear = ");
//    Serial.println(linear);
//    Serial.print("Yaw = ");
//    Serial.println(yaw);
    moveArmJoints(jointPositions[0], jointPositions[1], jointPositions[2]);
    Serial.print("flag");
}

