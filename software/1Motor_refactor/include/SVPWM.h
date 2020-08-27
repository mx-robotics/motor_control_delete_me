//
// Created by firat on 21.01.20.
//

#ifndef INC_1MOTOR_REFACTOR_SVPWM_H
#define INC_1MOTOR_REFACTOR_SVPWM_H

#include <Arduino.h>
#include <array>
#include "Motor.h"


struct SPWMDutyCycles{
    uint16_t inDutyCycleW;
    uint16_t inDutyCycleV;
    uint16_t inDutyCycleU;

};

struct ModulationIndexScalingParams{
    constexpr ModulationIndexScalingParams(float m, float c): offsetParam_m(m), offsetParam_c(c){};
    float offsetParam_m;
    float offsetParam_c;

};

class LUTGenerator{

public:
    static constexpr uint16_t maxSensorResolution = 16384;
    static constexpr uint8_t motorPoleCount = 11;
    static constexpr uint16_t LUTSize = maxSensorResolution / motorPoleCount;
    static constexpr uint16_t PWM_FREQ = 20000;
    static constexpr uint16_t MAX_DUTY_CYCLE = (F_BUS / PWM_FREQ) / 2;

    /**
     * Generates the Look Up Table for PWM duty cycles
     * CalculateDutyCycleW can be re-implemented for other SVPWM schemes such as symmetric, discontinuous etc.
     * - constexpr function, called once at compile time
     * @return
     */
    constexpr static  std::array<uint16_t, LUTSize> generate() {

        std::array <uint16_t, LUTSize> temp_table{};

        float modulation_index = 1.1547005f;

        uint angle = 0;
        for (int i = 0; i < LUTSize; i++) {

            temp_table[i] = static_cast<uint16_t>(calculateDutyCycleW(angle,modulation_index,MAX_DUTY_CYCLE));

            angle++;
        }

        return temp_table;
    }

    /**
     *
     * @param rotor_position - scaled (modulo'ed) encoder value that represents rotor flux. This is then modified to
     * a degree equivalent.
     * @param modulation_index - determines the power transferred to the Motor. Right now all tables are generated for
     * max power and then scaled down accordingly when needed.
     *
     * @param MAX_DUTY_CYCLE - required for calculation
     * @return The duty cycle of one phase
     */
    constexpr static uint16_t calculateDutyCycleW(uint16_t rotor_position,float modulation_index,uint16_t MAX_DUTY_CYCLE){
        constexpr float angleScaler = 360.0f / LUTSize;
        constexpr float deg_to_radians = 0.017453f;
        uint16_t IN_W_duty_cycle=0;

        float r1 = 0;
        float r2 = 0;
        float t7 = 0;
        float t0 = 0;
        float t1 = 0;
        float t2 = 0;
        float angle = static_cast<float>(rotor_position) * angleScaler;

        if (angle >= 0 && angle < 60) {

            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - angle) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) *
                 (1 - modulation_index * cos(angle * deg_to_radians) + (modulation_index / 6) * cos(3 * angle * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = (t2 + t7 + t1);
            //IN_U_duty_cycle = t2 + t7;
            //IN_V_duty_cycle = t7;

        } else if (angle >= 60 && angle < 120) {
            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - (angle - 60)) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle - 60) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) * (1 - modulation_index * cos((angle - 60) * deg_to_radians) +
                                         (modulation_index / 6) * cos(3 * (angle - 60) * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = t1 + t0;
            //IN_U_duty_cycle = t1 + t2 + t0;
            //IN_V_duty_cycle = t0;

        } else if (angle >= 120 && angle < 180) {

            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - (angle - 120)) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle - 120) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) * (1 - modulation_index * cos((angle - 120) * deg_to_radians) +
                                         (modulation_index / 6) * cos(3 * (angle - 120) * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = t7;
            //IN_U_duty_cycle = t1 + t2 + t7;
            //IN_V_duty_cycle = t2 + t7;


        } else if (angle >= 180 && angle < 240) {
            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - (angle - 180)) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle - 180) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) * (1 - modulation_index * cos((angle - 180) * deg_to_radians) +
                                         (modulation_index / 6) * cos(3 * (angle - 180) * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = t0;
            //IN_U_duty_cycle = t1 + t0;
            //IN_V_duty_cycle = t1 + t2 + t0;

        } else if (angle >= 240 && angle < 300) {
            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - (angle - 240)) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle - 240) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) * (1 - modulation_index * cos((angle - 240) * deg_to_radians) +
                                         (modulation_index / 6) * cos(3 * (angle - 240) * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = t2 + t7;
            //IN_U_duty_cycle = t7;
            //IN_V_duty_cycle = t1 + t2 + t7;


        } else if (angle >= 300 && angle < 360) {

            r1 = (sqrt(3) / 2) * modulation_index * sin((60 - (angle - 300)) * deg_to_radians);
            r2 = (sqrt(3) / 2) * modulation_index * sin((angle - 300) * deg_to_radians);

            t1 = r1 * (MAX_DUTY_CYCLE);
            t2 = r2 * (MAX_DUTY_CYCLE);
            t0 = (MAX_DUTY_CYCLE / 2) * (1 - modulation_index * cos((angle - 300) * deg_to_radians) +
                                         (modulation_index / 6) * cos(3 * (angle - 300) * deg_to_radians));
            t7 = MAX_DUTY_CYCLE - (t1 + t2 + t0);

            IN_W_duty_cycle = t1 + t2 + t0;
            //IN_U_duty_cycle = t0;
            //IN_V_duty_cycle = t1 + t0;


        }
        return IN_W_duty_cycle;


    }

    /***
     * LUT PWM values are calculated for maximum modulation index (max power). Due to complex nature of SVPWM,
     * a simple scaling doesnt work. Simply multiplying the full duty cycle with a scalar (0.5 for example) doesnt procude
     * the correct duty cycles. After this multiplication an offset need to be added to the scaled down duty cycle
     * This offset is a linear affine function of the scalar. The parameters of this functions are calculated here.
     * - constexpr function, called at compile time once.
     * @return
     */
    static  constexpr ModulationIndexScalingParams calculateModulationIndexScalingOffsetParameters() {

            uint16_t rotor_position = 200;
            float modulation_index_max = 1.1547005f;
            float IN_W_duty_cycle_max = calculateDutyCycleW(rotor_position, modulation_index_max, MAX_DUTY_CYCLE);

            float scalar1 = 0.4f;
            float modulationIndexNew = scalar1 * modulation_index_max;
            float dutyCycleScaledWithModuloIndex1 = IN_W_duty_cycle_max * scalar1;
            float dutyCycleCalculatedWithModuloIndex1 = calculateDutyCycleW(rotor_position, modulationIndexNew,
                                                                            MAX_DUTY_CYCLE);

            float scalar2 = 0.9f;
            modulationIndexNew = scalar2* modulation_index_max;
            float dutyCycleScaledWithModuloIndex2 = IN_W_duty_cycle_max * scalar2;
            float dutyCycleCalculatedWithModuloIndex2 = calculateDutyCycleW(rotor_position, modulationIndexNew,
                                                                            MAX_DUTY_CYCLE);


            float yy1 = dutyCycleCalculatedWithModuloIndex1 - dutyCycleScaledWithModuloIndex1;
            float yy2 = dutyCycleCalculatedWithModuloIndex2 - dutyCycleScaledWithModuloIndex2;

            float xx1 =  scalar1*100;
            float xx2 =  scalar2*100;

            float m = (yy2 - yy1) / (xx2 - xx1);

            float c = yy2 - (m * xx2);


            ModulationIndexScalingParams params{m, c};

            /*Serial.print(", c = ");
            Serial.print(static_cast<int16_t >(params.offsetParam_c));
            Serial.print(", m = ");
            Serial.println(static_cast<int16_t >(floorf(params.offsetParam_m )));
*/


        return params;

    }




};
/***
 * Space Vector Pulse Width Modulation(SVPWM) class -
 * All attributes are initialized at compile time.
 * Responsible for fetching correct value from the LUT
 */
class SVPWM {
private:
    static constexpr uint16_t LUTSize = LUTGenerator::LUTSize;
    static constexpr uint16_t angleOffset = LUTSize/4; // @TODO calculate -90 + 90, plot for variations
    static constexpr auto LUT = LUTGenerator::generate();
    static constexpr ModulationIndexScalingParams modulationIndexParams = LUTGenerator::calculateModulationIndexScalingOffsetParameters();

public:
    /***
     * This function does the actual work and consists of 4 steps:
     * 1- Calculates modulationIndexOffset : There is an offset for different modulation indices. This is calculate by
     *  the speedScalar (0 - 100)
     * 2 - Calculate field weakening : To achieve higher speeds field weakening - a small shift on the LUT index is required.
     *  This can be skipped entirely and is just an optimisation. For the old motors, instead of a fix field weakening,
     *  a speed scalar based index shift produces better results. Field weakening seems to be slightly different for different
     *  directions, hence * x.direction - 20
     *
     * 3- Base index calculation : By combining
     *      - scaledRotaryEncoderPosition - Corresponds to rotor flux angle @TODO : automate calculating this
     *      - fieldWeakening : Explained above, an optimisation
     *      - angleOffset : +90 -90 degrees (1/4 of the entire LUT range) to produces torque
     *  the LUT index that gives the optimal PWM Duty cycle value is calculated.  "+ LUTSize) % LUTSize " is required since
     *  adding the angleOffset can produce a negative index.
     *
     * 4 - Calculation other phase's indices : By simply adding 120 ° and 240° (LUTSize/3 and LUTSize*2/3) other phase
     * indices are acquired
     *
     * 5 - Scaling down the duty cycles for lower modulation indices : This is done by multiplying full duty cycle first
     * and then adding the modulationIndexOffset.
     *
     * @param x - motor object
     * @return SVPWM duty cycles for each phase of a motor
     */
    static SPWMDutyCycles calculateDutyCycles(Motor &x){
        SPWMDutyCycles temp;

        uint16_t modulationIndexOffset =  scaleDutyCyclesToModulationIndex(x.speedScalar);
        //int8_t fieldWeakening = -x.speedScalar; // gives the best results
        //uint16_t base = (x.scaledRotaryEncoderPosition + angleOffset * x.direction + LUTSize) % LUTSize;
        //x.setAngleOffset(-60 + x.speedScalar * x.direction)
        int32_t localOffset = x.calculateAngleOffsetFromSpeedCommand(x.speedScalar);
        uint16_t base = (x.scaledRotaryEncoderPosition + (angleOffset + x.angleOffset ) * x.direction +  LUTSize) % LUTSize;
        //uint16_t base = (x.scaledRotaryEncoderPosition + ((fieldWeakening + 600+ angleOffset) * x.direction - 20) + LUTSize) % LUTSize;
        /*
         * This part is tricky; there is a field-weakening and the best results has been found at -120 and + 80
         * To avoid an else-if check the fieldWeakening is set to 100 and with this -20 it is set to -120 or 80 according to the direction of the motor
         * It is very peculiar that the implicit Sensor Offet in the scaledRotaryEncoderPosition is different in each direction, one direction has a 40 degrees offset compared to the other
         * This issue clearly shows itself with the max speed difference if left unattended. With the current fieldWeakening parameter the max speed is around 13.50 in each direction
         *
         *
         * */
        uint16_t LUTIndexW = base;
        uint16_t LUTIndexV = (base + (LUTSize / 3) ) % LUTSize;
        uint16_t LUTIndexU = (base + (2 * (LUTSize / 3)) ) % LUTSize;
        float intermediateMultiplier = x.speedScalar * 0.01f;
        temp.inDutyCycleW = static_cast<uint16_t >(LUT[LUTIndexW]* intermediateMultiplier) + modulationIndexOffset;
        temp.inDutyCycleU = static_cast<uint16_t >(LUT[LUTIndexV]* intermediateMultiplier) + modulationIndexOffset;
        temp.inDutyCycleV = static_cast<uint16_t >(LUT[LUTIndexU]* intermediateMultiplier) + modulationIndexOffset;

        return temp;

    };
    /***
     * Calculates the offset required for the correct duty cycles after multiplying the full duty cycles with
     * the speed scalar
     * @param scalar - speed scalar (0..100)
     * @return
     */
    inline static uint16_t scaleDutyCyclesToModulationIndex(float scalar){
        return static_cast<uint16_t >(modulationIndexParams.offsetParam_m*scalar + modulationIndexParams.offsetParam_c);
    }


    static const auto &getLUT(){
        return LUT;

    }
    static const uint16_t getLutSize() {
        return LUTSize;
    }


};
#endif //INC_1MOTOR_REFACTOR_SVPWM_H