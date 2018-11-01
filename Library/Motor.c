// Motor.c
// Runs on MSP432

// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board and connect VCCMD to 3.3V.
//   This makes P3.7 and P3.6 low power disables for motor drivers.  0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
//   This separates P3.7 and P3.6 allowing for independent control
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Timer_A PWM Configuration Parameter */
/*
 * SMCLK = 12Mhz
 * Divide by 12 to get a 1Mhz timer clock source
 * Set divider to 1000 to get a 1000Hz pwm rate
 * Start off at 100 * 1/64 for the high time
 */
Timer_A_PWMConfig right_motor_pwm_config =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_12,
        1000,
        TIMER_A_CAPTURECOMPARE_REGISTER_3,
        TIMER_A_OUTPUTMODE_RESET_SET,
        100
};
Timer_A_PWMConfig left_motor_pwm_config =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_12,
        1000,
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        100
};

void Motor_Init(void){
    /*
    * Configuring GPIO2.6 as peripheral output for PWM of Right Motor
    * Configure GPIO1.6 as peripheral output for DIR of Right Motor
    */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6,
           GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6,
                  GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

    /*
    * Configuring GPIO2.7 as peripheral output for PWM of Left Motor
    * Configure GPIO1.7 as peripheral output for DIR of Left Motor
    */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
           GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN7,
                  GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);

    /*
    * Configuring Timer_A to have a period of approximately 500ms and
    * an initial duty cycle of 10% of that (3200 ticks)
    */
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &right_motor_pwm_config);
  
}

void Set_Left_Motor_PWM(int pwm)
{
    left_motor_pwm_config.dutyCycle = pwm;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &left_motor_pwm_config);
}

void Set_Right_Motor_PWM(int pwm)
{
    right_motor_pwm_config.dutyCycle = pwm;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &right_motor_pwm_config);
}

void Set_Left_Motor_Dir(bool dir)
{
    if (dir)
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN7);
    else
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
}

void Set_Right_Motor_Dir(bool dir)
{
    if (dir)
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
    else
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
}
