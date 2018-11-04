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
#include "Library/Encoder.h"

void Initialize_System();

uint8_t light_data; // QTR-8RC
uint8_t light_data0;
int32_t Position; // 332 is right, and -332 is left of center
uint8_t bump_data;
uint8_t bump_data0;

int tick=0;

bool button_S1_pressed = false;
bool button_S2_pressed = false;

typedef enum
{
    START = 0,
    WAIT,
    GO,
    STOP
} my_state_t;

my_state_t state = START;

int main(void)
{


    Initialize_System();

    Reflectance_Init();

    Bump_Init();

    Motor_Init();

    Encoder_Init();

    Set_Left_Motor_PWM(0);
    Set_Right_Motor_PWM(0);

    while (1)
    {
        // Read Reflectance data into a byte.
        // Each bit corresponds to a sensor on the light bar
        light_data = Reflectance_Read(1000);
        light_data0 = LIGHT_BAR(light_data,0);

        // Convert light_data into a Position using a weighted sum
        Position = Reflectance_Position(light_data);

        // Read Bump data into a byte
        // Lower six bits correspond to the six bump sensors
        bump_data = Bump_Read();
        // put into individual variables so we can view it in GC
        bump_data0 = BUMP_SWITCH(bump_data,1);

        // Emergency stop switch S2
        // Switch to state "STOP" if pressed
        if (button_S2_pressed) state = STOP;

        // Main State Machine
        switch (state) {
        case START:
                state = WAIT;
            break;
        case WAIT:
                if (button_S1_pressed) {
                    Set_Left_Motor_PWM(200);
                    Set_Right_Motor_PWM(200);
                    state = GO;
                }
            break;
        case GO:
            break;
        case STOP:
            Set_Left_Motor_PWM(0);          // Stop all motors
            Set_Right_Motor_PWM(0);
            break;
        }

        Clock_Delay1ms(10);
    }
}

void Initialize_System()
{
    /*
     * Initialize main clock
     *
     * SMCLK = 12Mhz
     */
    Clock_Init48MHz();

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring GPIO LED1 as an output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configure GPIO LED Red, LED Green, LED Blue */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);


    /*
     * Configuring SysTick to trigger at .1 sec (MCLK is 48Mhz)
     */
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

    /* Enable interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableMaster();

}
/* Port1 ISR - This Interrupt Handler will service the Port1 interrupts.
 * Port1 pin 1 & 4 are connected to the two side buttons.
 */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Button S1
    if (status & GPIO_PIN1)
    {
        button_S1_pressed = true;
    }

    // Button S2
    if (status & GPIO_PIN4)
    {
        button_S2_pressed = true;
    }
}

/*
 * Handle the SysTick Interrupt.  Currently interrupting at 1/10 second.
 *
 * Increment the tick counter "tick"
 * Blink the red led
 */
void SysTick_Handler(void)
{
    tick++;
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}



