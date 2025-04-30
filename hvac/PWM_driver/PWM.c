
#include "PWM.h"
#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"


// FAN용
IfxGtm_Tom_Pwm_Config g_fanConfig;
IfxGtm_Tom_Pwm_Driver g_fanDriver;

// SERVO용
IfxGtm_Tom_Pwm_Config g_servoConfig;
IfxGtm_Tom_Pwm_Driver g_servoDriver;
uint32 g_fadeValue = 0;                                             /* Fade value, starting from 0                  */
sint8 g_fadeDir = 1;                                                /* Fade direction variable                      */



void setDutyCycle(uint32 dutyCycle);                                /* Function to set the duty cycle of the PWM    */
//void controlFan(void);

/* This function initializes the TOM */
void initGtmTomPwm(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */

    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock                         */

    /* Initialize the configuration structure with default parameters */
    IfxGtm_Tom_Pwm_initConfig(&g_fanConfig, &MODULE_GTM);

    g_fanConfig.tom = FAN.tom;                                      /* Select the TOM depending on the LED          */
    g_fanConfig.tomChannel = FAN.channel;                           /* Select the channel depending on the LED      */
    g_fanConfig.period = PWM_PERIOD;                                /* Set the timer period                         */
    g_fanConfig.dutyCycle = 0;
    g_fanConfig.pin.outputPin = &FAN;                               /* Set the LED port pin as output               */
    g_fanConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_fanDriver, &g_fanConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_fanDriver, TRUE);                       /* Start the PWM                                */
}
void initServoPwm(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */

    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock*/
    
    /* Initialize the configuration structure with default parameters */
    IfxGtm_Tom_Pwm_initConfig(&g_servoConfig, &MODULE_GTM);

    g_servoConfig.tom = SERVO.tom;                                      /* Select the TOM depending on the LED          */
    g_servoConfig.tomChannel = SERVO.channel;                           /* Select the channel depending on the LED      */
    g_servoConfig.period = SERVO_PWM;                                /* Set the timer period                         */
    //g_servoConfig.dutyCycle = SERVO_PWM_CENTER;
    g_servoConfig.pin.outputPin = &SERVO;                               /* Set the LED port pin as output               */
    g_servoConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */
    
    g_servoConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    
    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_servoDriver, TRUE);                       /* Start the PWM                                */
}


/* This function sets the duty cycle of the PWM */
void setDutyCycle(uint32 dutyCycle)
{
    g_fanConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_fanDriver, &g_fanConfig);                /* Re-initialize the PWM                        */

}

void setSERVODutyCycle(uint32 dutyCycle)
{
    g_servoConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Re-initialize the PWM                        */
}

/*
g_servoConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;

100MHz를 256으로 나눠 -> 390625Hz
390625Hz는 0.00256ms

서보모터는 20ms가 주기 -> 20ms / 0.00256ms = 7812.5

Period 틱 설정 : 7812

*/