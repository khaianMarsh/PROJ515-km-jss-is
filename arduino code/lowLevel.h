#include "declarations.h"
//implements i/o level functions
//converts pins assignements in declarations to useable functions

//serial------------------------------------------------------------------
void changeVerbose(boolean state){
    verboseOutput = state;
}

void serialClear(){
    //code the clear a serial interface
    Serial.write(27);    
    Serial.print("[2J");   
    Serial.write(27);
    Serial.print("[H");    
}

void debugPrint(String longAns, byte shortAns){
    //prints a line to the termanal, if the verbose constant is:
    //true, it gives a human friendly text ouput
    //false, it gives a numeric code
    
    if (verboseOutput == true){
        Serial.println(String(shortAns) + " : " + longAns);
    }
    else  if (shortAns != 0){
        Serial.println(shortAns);
    }
}

double mapNumber(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//driver control----------------------------------------------------------------
void setEnable(boolean state){
    //sets the enable/disable input of all motor drivers
    //state is inverted as enable pins are actualy !enable
    digitalWrite(pitch_ena,!state);
    digitalWrite(lin_ena,!state);
    digitalWrite(yaw_ena,!state);
}

void dir(byte motors){
    //sets the direction input of all the motor drivers
    
    byte pitchMove = (motors&B001)>>0;
    byte linMove = (motors&B010)>>1;
    byte yawMove = (motors&B100)>>2;  
    
    digitalWrite(pitch_dir,pitchMove);
    digitalWrite(lin_dir,linMove);
    digitalWrite(yaw_dir,yawMove);
    delayMicroseconds(30);
}

inline void niceDir (bool pitch, bool lin, bool yaw){
    //verbose varient of setDir to allow easier control
    if (pitch == pitchUp){
        pitchMove = pitchUp; 
    }
    else{
        pitchMove = pitchDown;
    }
    if (lin == linForwards){
        linMove = linForwards;
    }
    else{
        linMove = linBackwards;
    }
    if (yaw == yawLeft){
        yawMove = yawLeft; 
    }
    else{
        yawMove = yawRight;
    }
    
    digitalWrite(pitch_dir, pitchMove);
    digitalWrite(lin_dir, linMove);
    digitalWrite(yaw_dir, yawMove);
    delayMicroseconds(30);
}

void pulse(bool pitch,bool lin, bool yaw,unsigned long iteration, boolean overideLS){
    //pulses given motors a given number of times, accounting for a preset pulse length
    	
    if (pitch == true){
        if (pitchMove == pitchUp){
            pitchPos = pitchPos + iteration; 
        }
        else{
            pitchPos = pitchPos - iteration;
        }
    }
    if (lin == true){
        if (linMove == linForwards){
            linPos = linPos + iteration; 
        }
        else{
            linPos = linPos - iteration;
        }
    }
    if (yaw == true){
        if(yawMove == yawLeft){
            yawPos = yawPos + iteration; 
        }
        else{
            yawPos = yawPos - iteration;
        }       
    }

    for (int i = 0; i < iteration; i++){ //TODO check true == HIGH
        digitalWrite(pitch_pul, pitch);
        digitalWrite(lin_pul, lin); 
        digitalWrite(yaw_pul, yaw);
        
        delayMicroseconds(pulseTime - 10);
        digitalWrite(pitch_pul, LOW);
        digitalWrite(lin_pul, LOW);
        digitalWrite(yaw_pul, LOW);
        delayMicroseconds(10);
    
        if (digitalRead(limitSwitch) == 0 && overideLS == false){
            //setEnable(false);
            debugPrint("limit switch hit,system locked", 254);
            while(true) delay(1000);
        }
    }
}

//timeing control-------------------------------------------------------------------
void setTimeing(unsigned int value){
    //sets the time that a pulse should be completed in
    if ((value > absMaxPulseTime) || (value < absMinPulseTime)){
        debugPrint("value out of range", 248);
    return;
    }
    pulseTime = value;	
}

void setMinTimeing(unsigned int value){
    //sets the shortest time a pulse may be completed in,in a complex movement
    if ((value>absMaxPulseTime) || (value<absMinPulseTime)||(value>maxPulseTime)){
      debugPrint("value out of range", 248);
      return;
    }
    minPulseTime = value;  
}

void setMaxTimeing(unsigned int value){
    //sets the longest time a pulse may be completed in,in a complex movement
    if ((value > absMaxPulseTime) || (value<absMinPulseTime) || (value>minPulseTime)){
        debugPrint("value out of range", 248);
        return;
    }
    maxPulseTime = value;  
}

void setAbsMaxTimeing(unsigned int value){
    //sets the absolute longest time a pulse may be completed in
    absMaxPulseTime = value;  
}

void setAbsMinTimeing(unsigned int value){
    //sets the absolute shortest time a pulse may be completed in
    minPulseTime = value;  
}

void sysDelay(unsigned int value){
    //forces the system to delay for a specific amount of time
    //reminemt of system batch procccessing commmands
    delay(value);
}

//position control-------------------------------------------------------------------
void clearPos(){
    //clears the internal models position
    pitchPos = 0;
    linPos = 0;
    yawPos = 0;
}

