#include "lowLevel.h"
//implements forwards and backwards kinomatics

bool testangles(float pitch,float lin,float yaw){//Tests if the motor positions are in range
    bool positionvalid = true;
    //hard limits
    if((pitch > 15 * pi / 180) || (pitch < -70 * pi / 180)) {
        debugPrint("pitch out of range", 252);
        positionvalid = false;
    }
    if((lin < linearMinium)||(lin > linearMaximum)){
        debugPrint("lin out of range", 251);
        positionvalid = false;
    }
    if((yaw > 90 * pi / 180)||(yaw < -90 * pi / 180)){
        debugPrint("yaw out of range", 250);
        positionvalid = false;
    }
    return positionvalid;  
}

//void inversekino(int x, int y, int z, float (&arrayOut)[3]){//TODO lin need defining properly, check ll calcs
//    float pitch = 0;
//    float lin = 0;
//    float yaw = 0;
//    
//    yaw = atan2(x, y);
//    float h = (y / cos(yaw)) - forkH;//horizontal diffrence between pitch pivot and endpoint
//    float z1 = z - forkV;//vertical diffrence between pitch pivot and endpoint
//    lin = sqrt((h * h) + (z1  * z1)) - minLin;
//    pitch = atan2(z1, h);
//    
//    arrayOut[0] = pitch;
//    arrayOut[1] = lin;
//    arrayOut[2] = yaw;
//}

void inverseKinomatics(float x_target, float y_target, float z_target, float arrayOut[]){
    x_targetCoord = x_target;
    y_targetCoord = y_target;
    z_targetCoord = z_target;
    
    float pitch = 0;
    float linear = 0;
    float yaw = 0;
    float zDifferance = z_target - Z_OFFSET;

    yaw = atan2(y_target, x_target);

    float xyDifferance = sqrt(sq(x_target) + sq(y_target));// x_target / cos(yaw) - forkH;
    linear = sqrt(sq(xyDifferance) + sq(zDifferance)) - linearOffset;// - minLin;
    pitch = atan2(zDifferance, xyDifferance);

    if(testangles(pitch, linear, yaw) == true){//if angles are in range
        arrayOut[0] = pitch;
        arrayOut[1] = linear;//TODO get limit
        arrayOut[2] = yaw;
    }
    else{
        x_targetCoord = x_Coord;//Sets the target positions to the current position if out of range
        y_targetCoord = y_Coord;//Sets the target positions to the current position if out of range
        z_targetCoord = z_Coord;//Sets the target positions to the current position if out of range
        arrayOut[0] = (float)pitchPos * (float)pitchGearing;//Sets the angle to the current angle
        arrayOut[1] = (float)linPos * (float)linGearing;//Sets the angle to the current angle
        arrayOut[2] = (float)yawPos * (float)yawGearing;//Sets the angle to the current angle
        return;
    }
}

void forwardKino(float pitch,float lin,float yaw,unsigned int (&arrayOut)[3]){
    float lengthH = cos(pitch) * (lin + minLin);
    
    arrayOut[0] = sin(yaw) * (forkH+lengthH);//x
    arrayOut[1] = cos(yaw) * (forkH+lengthH);//y
    arrayOut[2] = forkV + sin(pitch) * (lin + minLin);//z
}

int centerOfMass(float pitch, float lin){
    //takes into account wieght of lin section and lin motor, everything else is assumed to be balanced
    //returns values in Nmm
    //lin 0 == all the way back,(190mm front to pivot)
    //lin motor distance to pivot 95mm, weight of 1.1kg
    //lin arm length 1000mm, weight of 1.5kg
    
    int linArmTorque=((lin - 500 + 190) * cos(pitch)) * 15.7;
    int linMotorTorque=(-95 * cos(pitch)) * 11;
    
    int torqueApplied = linArmTorque + linMotorTorque;
    return torqueApplied;
}

int recommendedTimeing(float pitch, float lin){
    //calcuate recommended minimum timeings based upon center of mass of piching componants
    // values max at roughtly 5700
    int timeing = abs(centerOfMass(pitch, lin)) / 7;
    if (timeing > absMaxPulseTime) timeing = absMaxPulseTime;
    if (timeing < absMinPulseTime) timeing = absMinPulseTime;
    
    return timeing;
}







