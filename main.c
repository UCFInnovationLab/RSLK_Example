/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "Library/Reflectance.h"
#include "Library/Clock.h"
#include "Library/Bump.h"
#include "Library/Motor.h"

void Initialize_System();

uint8_t light_data; // QTR-8RC
uint8_t light_data0, light_data1, light_data2, light_data3, light_data4, light_data5, light_data6, light_data7;
int32_t Position; // 332 is right, and -332 is left of center
uint8_t bump_data;
uint8_t bump_data0, bump_data1, bump_data2, bump_data3, bump_data4, bump_data5;
int left_motor_pwm = 0;
int left_motor_dir = 0;
int right_motor_pwm = 0;
int right_motor_dir = 0;




int main(void)
{
    // SMCLK = 12Mhz
    //
    Clock_Init48MHz();

    Initialize_System();

    Reflectance_Init();

    Bump_Init();

    Motor_Init();

    left_motor_pwm = 0;
    right_motor_pwm = 0;

    while (1)
    {
        // Read Reflectance data into a byte.
        // Each bit corresponds to a sensor on the light bar
        light_data = Reflectance_Read(1000);
        // put into individual variables so we can view it in GC
        light_data0 = (light_data>>0)&0x01;
        light_data1 = (light_data>>1)&0x01;
        light_data2 = (light_data>>2)&0x01;
        light_data3 = (light_data>>3)&0x01;
        light_data4 = (light_data>>4)&0x01;
        light_data5 = (light_data>>5)&0x01;
        light_data6 = (light_data>>6)&0x01;
        light_data7 = (light_data>>7)&0x01;
        // Convert light_data into a Position using a weighted sum
        Position = Reflectance_Position(light_data);

        // Read Bump data into a byte
        // Lower six bits correspond to the six bump sensors
        bump_data = Bump_Read();
        // put into individual variables so we can view it in GC
        bump_data0 = (bump_data>>0)&0x01;
        bump_data1 = (bump_data>>1)&0x01;
        bump_data2 = (bump_data>>2)&0x01;
        bump_data3 = (bump_data>>3)&0x01;
        bump_data4 = (bump_data>>4)&0x01;
        bump_data5 = (bump_data>>5)&0x01;

        // Set PWM from GC global variable
        Set_Left_Motor_PWM(left_motor_pwm);
        Set_Right_Motor_PWM(right_motor_pwm);

        // Set DIR from GC global variable
        if (left_motor_dir)
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN7);
        else
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
        if (right_motor_dir)
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
        else
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

        Clock_Delay1ms(10);
    }
}

void Initialize_System()
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring GPIO LED1 as an output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configuring SysTick to trigger at 1500000 (MCLK is 1.5MHz so this will
     * make it toggle every 1s) */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(4800000);
    MAP_SysTick_enableInterrupt();

    /*
     * Configure P1.1 for button interrupt
     */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    /*
     * Configure P1.4 for button interrupt
     */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    /* Enabling interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableMaster();

}
/* Port1 ISR - This ISR will progressively step up the duty cycle of the PWM
 * on a button press
 */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Button S1
    if (status & GPIO_PIN1)
    {
        if (left_motor_pwm > 1000)
            left_motor_pwm = 0;
        else
            left_motor_pwm += 100;
    }

    // Button S2
    if (status & GPIO_PIN4)
    {
        if (right_motor_pwm > 1000)
            right_motor_pwm = 0;
        else
            right_motor_pwm += 100;
    }
}

void SysTick_Handler(void)
{
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
