 /*
  *
  * Pololu #3542 Romi Encoder connected to Pololu #3543 Motor Driver and Power Distribution Board
  * This connects motor, power, encoders, and grounds.  Kit includes this hardware.  See images.
  * Sever VPU = VREG jumper on Motor Driver and Power Distribution Board and connect VPU to 3.3V.
  * This is necessary because MSP432 inputs are not 5V tolerant.
  *
  * Left Encoder A connected to P6.4 (J1)                     .
  * Left Encoder B connected to P6.5 (J1)
  * Right Encoder A connected to P5.4 (J3)
  * Right Encoder B connected to P5.5 (J3)
  *
  * Pololu encoder has 12 counts per revolution (counting all 4 edges).
  * The motor has a gearbox with a 120:1 ratio.
  * This gives 12*120 = 1440 counts per revolution of the wheel.
  * Since we are only counting 1 edge of the encoder we need to divide by 4 for a total of 360 counts per revolution.
 */



#include <stdint.h>
#include "msp.h"
#include "Encoder.h"

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Initialize Encoder inputs

int Left_Motor_Count = 0;
int Right_Motor_Count = 0;

void Encoder_Init(void){
    /* Configuring P6.4 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT6);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN5);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT5);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN5);
}

int Get_Left_Motor_Count() {
    return Left_Motor_Count;
}

int Get_Right_Motor_Count() {
    return Right_Motor_Count;
}

/* GPIO ISR */
void PORT6_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, status);

    /* Toggling the output on the LED */
    if(status & GPIO_PIN4)
    {
        if (MAP_GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5))
            Left_Motor_Count++;
        else
            Left_Motor_Count--;

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }

}

/* GPIO ISR */
void PORT5_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    /* Toggling the output on the LED */
    if(status & GPIO_PIN4)
    {
        if (MAP_GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN5))
            Right_Motor_Count++;
        else
            Right_Motor_Count--;

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }

}



///* Timer_A Continuous Mode Configuration Parameter */
//const Timer_A_ContinuousModeConfig continuousModeConfig =
//{
//        TIMER_A_CLOCKSOURCE_SMCLK,           // SMCLK Clock Source
//        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // SMCLK/1 = 12MHz
//        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
//        TIMER_A_SKIP_CLEAR                   // Skip Clear Counter
//};
//
///* Timer_A Capture Mode Configuration Parameter */
//const Timer_A_CaptureModeConfig captureModeConfig =
//{
//        TIMER_A_CAPTURECOMPARE_REGISTER_1,        // CC Register 2
//        TIMER_A_CAPTUREMODE_RISING_EDGE,          // Rising Edge
//        TIMER_A_CAPTURE_INPUTSELECT_CCIxB,        // CCIxB Input Select
//        TIMER_A_CAPTURE_SYNCHRONOUS,              // Synchronized Capture
//        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupt
//        TIMER_A_OUTPUTMODE_OUTBITVALUE            // Output bit value
//};


//int main(void)
//{
//    /* Stop watchdog timer */
//    MAP_WDT_A_holdTimer();
//
//    /* Configuring P1.0 as output */
//    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
//
//    /* Configuring P2.4 as peripheral input for capture and P4.2 for ACLK
//     * output */
//    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN2,
//            GPIO_PRIMARY_MODULE_FUNCTION);
//    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN4,
//            GPIO_PRIMARY_MODULE_FUNCTION);
//
//    /* Setting ACLK = VLO = 14kHz */
//    MAP_CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
//
//    /* Configuring Capture Mode */
//    MAP_Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);
//
//    /* Configuring Continuous Mode */
//    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
//
//    /* Enabling interrupts and going to sleep */
//    MAP_Interrupt_enableSleepOnIsrExit();
//    MAP_Interrupt_enableInterrupt(INT_TA0_N);
//    MAP_Interrupt_enableMaster();
//
//    /* Starting the Timer_A0 in continuous mode */
//    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
//
//    MAP_PCM_gotoLPM0();
//
//}

//******************************************************************************
//
//This is the TIMERA interrupt vector service routine.
//
//******************************************************************************
//void TA0_N_IRQHandler(void)
//{
//    uint32_t jj;
//
//    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
//            TIMER_A_CAPTURECOMPARE_REGISTER_1);
//
//    timerAcaptureValues[timerAcapturePointer++] =
//            MAP_Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
//                                             TIMER_A_CAPTURECOMPARE_REGISTER_1);
//
//    if (timerAcapturePointer >= NUMBER_TIMER_CAPTURES)
//    {
//        while (1)
//        {
//            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
//            for(jj=0;jj<10000;jj++);
//        }
//    }
//}

