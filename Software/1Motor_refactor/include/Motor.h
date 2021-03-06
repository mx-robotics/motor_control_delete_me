//
// Created by firat on 20.01.20.
//

#ifndef INC_1MOTOR_REFACTOR_MOTOR_H
#define INC_1MOTOR_REFACTOR_MOTOR_H
#include <Arduino.h>
#include <array>


struct InhibitPins {
    const uint8_t InhibitPinU;
    const uint8_t InhibitPinV;
    const uint8_t InhibitPinW;

    constexpr InhibitPins(uint8_t PinU_, uint8_t PinV_, uint8_t PinW_) : InhibitPinU(PinU_), InhibitPinV(PinV_),
                                                                         InhibitPinW(PinW_) {}

};

struct InitPins {
    const uint8_t InitPinU;
    const uint8_t InitPinV;
    const uint8_t InitPinW;

    constexpr InitPins(uint8_t PinU_, uint8_t PinV_, uint8_t PinW_) : InitPinU(PinU_), InitPinV(PinV_),
                                                                      InitPinW(PinW_) {}

};

struct ServoPins {
    const uint8_t servoPin1;
    const uint8_t servoPin2;

    constexpr ServoPins(uint8_t servoPin1_, uint8_t servoPin2_) : servoPin1(servoPin1_), servoPin2(servoPin2_) {}
};

struct SPIPins {
    const uint8_t MISO;
    const uint8_t MOSI;
    const uint8_t CS;

    constexpr SPIPins(uint8_t MISO_, uint8_t MOSI_, uint8_t CS_) : MISO(MISO_), MOSI(MOSI_), CS(CS_) {}
};


struct ISPins {
    const uint8_t ISPinU;
    const uint8_t ISPinV;
    const uint8_t ISPinW;

    constexpr ISPins(uint8_t ISPinU_, uint8_t ISPinV_, uint8_t ISPinW_) : ISPinU(ISPinU_), ISPinV(ISPinV_),
                                                                          ISPinW(ISPinW_) {}

};


enum Direction {
    CCW = -1, STOP, CW
};





class Motor {
public:
    Motor(InhibitPins inhibitPins_, InitPins initPins_, SPIPins SpiPins_, ISPins IsPins_)
    : inhibitPins(inhibitPins_), initPins(initPins_), SpiPins(SpiPins_), IsPins(IsPins_){}

    const  InhibitPins inhibitPins;
    const InitPins initPins;
    const SPIPins SpiPins; //@TODO dont need the whole set of pins, SPICOM calss should handle the others
    const ISPins IsPins;
    Direction direction = Direction::CW;
    float speedRPM = 0;
    float torque = 0;
    float speedScalar= 80; // something between 0 and 1 update later so you dont multiplie once one
    //int16_t fieldWeakening = -100; // best : 80 for -1;
    int16_t fieldWeakening = -70; // best : 80 for -1;
    uint16_t rotaryEncoderPosition = 0;
    uint16_t previousRotaryEncoderValue = 0; // hold the previous rotaryEncoderValue
    uint16_t scaledRotaryEncoderPosition = 0; // accounts for the fieldWeakening
    uint16_t encoderCumulativeValue = 0;

    void setSensorOffset(uint16_t sensorOffset_) {
        fieldWeakening = sensorOffset_;
    }
    void updateEncoderCumulativeValue(){
        encoderCumulativeValue = 0;
    }
     void cumulativeAdd(uint16_t rotPos){
        uint16_t diff = abs(rotPos - previousRotaryEncoderValue);
        if(diff < 2){
            diff = 0;
        }
        if(diff > 100){
            diff = 16384 - diff;
        }
         previousRotaryEncoderValue = rotPos;
         encoderCumulativeValue += diff;




    }

    void updateRotaryEncoderPosition(uint16_t rotPos){
        scaledRotaryEncoderPosition = 1489 - rotPos;
        //scaledRotaryEncoderPosition = rotPos;
        rotaryEncoderPosition = rotPos;

    }
    void updatePrevRotaryEncoderPosition(uint16_t rotPos){
        previousRotaryEncoderValue = rotPos;

    }

        void updateSpeedRPM(float_t rpm){
        speedRPM = rpm;
    }

    void updateSpeedScalar(float_t modIndex){
        speedScalar=modIndex;
    };



};




#endif //INC_1MOTOR_REFACTOR_MOTOR_H
