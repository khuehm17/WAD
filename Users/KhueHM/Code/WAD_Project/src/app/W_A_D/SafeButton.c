/********************************************************************
 *
 *
 *
 *******************************************************************/

#include <stdio.h>
#include "board.h"
#include "gpio_pins.h"
#include "gpio_imx.h"
#include "debug_console_imx.h"

/* button relevent variables */
static uint8_t keyPressCount;
static bool on = false;
static volatile uint8_t gpio_mode;
#ifdef BOARD_GPIO_KEY_CONFIG
static uint8_t keyLastState;
static uint8_t keyState;
static volatile uint8_t button_pressed_flag;
#endif
#define INTERRUPT 1
#define POLLING   0

/*!
 * @brief Initialize GPIO_LED controller.
 */
void GPIO_Ctrl_InitLedPin()
{
#ifdef BOARD_GPIO_LED_CONFIG
    /* GPIO module initialize, configure "LED" as output and drive the output high level */
    gpio_init_config_t ledInitConfig = {
        .pin = BOARD_GPIO_LED_CONFIG->pin,
        .direction = gpioDigitalOutput,
        .interruptMode = gpioNoIntmode
    };
    GPIO_Init(BOARD_GPIO_LED_CONFIG->base, &ledInitConfig);
#endif
}

/*!
 * @brief Initialize GPIO INT\POLLING controller.
 */
void GPIO_Ctrl_InitKeyPin(void)
{
#ifdef BOARD_GPIO_KEY_CONFIG
/* GPIO module initialize, configure button as interrupt mode. */
        gpio_init_config_t keyInitConfig = {
            .pin = BOARD_GPIO_KEY_CONFIG->pin,
            .direction = gpioDigitalInput,
            .interruptMode = gpioIntFallingEdge,
        };
        GPIO_Init(BOARD_GPIO_KEY_CONFIG->base, &keyInitConfig);
        NVIC_EnableIRQ(BOARD_GPIO_KEY_IRQ_NUM);
        /* Clear the interrupt state, this operation is necessary, because the GPIO module maybe confuse
           the first rising edge as interrupt*/
        GPIO_ClearStatusFlag(BOARD_GPIO_KEY_CONFIG->base, BOARD_GPIO_KEY_CONFIG->pin);
        /* Enable GPIO pin interrupt */
        GPIO_SetPinIntMode(BOARD_GPIO_KEY_CONFIG->base, BOARD_GPIO_KEY_CONFIG->pin, true);
#endif
}

/*!
 * @brief Wait user to press key in INT\NOINT mode.
 */
void GPIO_WaitKeyPressed(void)
{
#ifdef BOARD_GPIO_KEY_CONFIG
        while( button_pressed_flag == 0 );
        button_pressed_flag = 0;
#endif
}

/*!
 * @brief Toggle the led
 */
void GPIO_LED_Toggle()
{
#ifdef  BOARD_GPIO_LED_CONFIG
    GPIO_WritePinOutput(BOARD_GPIO_LED_CONFIG->base,BOARD_GPIO_LED_CONFIG->pin, on ? gpioPinSet : gpioPinClear);
    PRINTF("Button %s\n\r", on ? "released" : "pressed");
#else
    PRINTF("%c ", on ? '+' : '-');
#endif
    on = !on;
}

/*!
 * @brief disable gpio interrupt.
 */
void GPIO_DisableInt()
{
#ifdef BOARD_GPIO_KEY_CONFIG
    /* Now disable the interrupt */
    NVIC_DisableIRQ(BOARD_GPIO_KEY_IRQ_NUM);
    GPIO_SetPinIntMode(BOARD_GPIO_KEY_CONFIG->base, BOARD_GPIO_KEY_CONFIG->pin, false);
#endif
}

/******************************************************************************
*
* Function Name: main
* Comments: GPIO module initialize, interrupt and IO operation.
*   This example include 2 step:
*   1)Configure BUTTON1 as interrupt mode, falling edge, and test
*     by pressing the button 3 times to trigger interrupt.
*   2)Configure BUTTON1 as GPIO functionality
*     and check the button's state(pressed or released) through switch LED
*     to on or off if this board has LED.
*
******************************************************************************/

int main( void )
{
    /* hardware initialiize, include RDC, IOMUX, Uart debug initialize */
    hardware_init();
    PRINTF("\n\r====================== GPIO Example ========================\n\r");

    /* first show the interrupt mode */
    gpio_mode = INTERRUPT;

    /* GPIO module initialize, configure "LED" as output and button as interrupt mode. */
    GPIO_Ctrl_InitLedPin();
    GPIO_Ctrl_InitKeyPin();

    /* wait for user to press button */
#ifdef BOARD_GPIO_KEY_CONFIG
    PRINTF("\n\r=================== GPIO Interrupt =====================\n\r");
    PRINTF("The (FUNC1) button is configured to trigger GPIO interrupt \n\r");
    PRINTF("Press the (FUNC1) button 3 times to continue.\n\n\r");
#else
    PRINTF("\n\r============ Use key to simulate GPIO button ==============\n\r");
    PRINTF("Input any data from terminal 3 times to continues.\n\n\r");
#endif
    keyPressCount = 1;
    while(keyPressCount < 4)
    {
        GPIO_WaitKeyPressed();
        PRINTF("Button pressed %d time. \n\r", keyPressCount);
        keyPressCount++;
    }

    /* Now disable the interrupt */
    GPIO_DisableInt();

    /* Second show the polling mode */
    gpio_mode = POLLING;

    /* GPIO module initialize, configure button as polling mode. */
    GPIO_Ctrl_InitKeyPin();

    /* Configure button as GPIO functionality
       and check the button's state(pressed or released) to switch LED on or off */
    /* Check the buttion's status(pressed or released) */
    PRINTF("\n\r================= GPIO Functionality==================\n\r");
    PRINTF("The button state is now polled.\n\r");
    PRINTF("Press the button to switch LED on or off\n\n\r");
    while(true)
    {
        GPIO_WaitKeyPressed();
        GPIO_LED_Toggle();
    }

}

/******************************************************************************
* Function Name: BOARD_GPIO_BTN_HANDLER
* Comments: The interrupt service routine triggered by gpio
* Note: Need to consider how to eliminate the button shake problem
******************************************************************************/
#ifdef BOARD_GPIO_KEY_CONFIG
void BOARD_GPIO_KEY_HANDLER(void)
{
    button_pressed_flag = 1;
    /* clear the interrupt status */
    GPIO_ClearStatusFlag(BOARD_GPIO_KEY_CONFIG->base, BOARD_GPIO_KEY_CONFIG->pin);
}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/

