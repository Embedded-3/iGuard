/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "PWM.h"
#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"
#include "asclin.h"
#include "IfxGtm_Cmu.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_TOM    20                                      /* Interrupt priority number                    */
#define FAN                 IfxGtm_TOM0_9_TOUT1_P02_1_OUT
#define SERVO               IfxGtm_TOM0_13_TOUT5_P02_5_OUT
#define PWM_PERIOD          100000                                   /* PWM period for the TOM                       */
#define SERVO_PWM           7812

#define SERVO_PWM_CENTER    150000
#define SERVO_PWM_MIN       100000
#define SERVO_PWM_MAX       200000
/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
// FAN용
IfxGtm_Tom_Pwm_Config g_fanConfig;
IfxGtm_Tom_Pwm_Driver g_fanDriver;

// SERVO용
IfxGtm_Tom_Pwm_Config g_servoConfig;
IfxGtm_Tom_Pwm_Driver g_servoDriver;
uint32 g_fadeValue = 0;                                             /* Fade value, starting from 0                  */
sint8 g_fadeDir = 1;                                                /* Fade direction variable                      */




/*********************************************************************************************************************/
/*-----------------------------------------------Function Prototypes-------------------------------------------------*/
/*********************************************************************************************************************/
void setDutyCycle(uint32 dutyCycle);                                /* Function to set the duty cycle of the PWM    */
//void controlFan(void);
/*********************************************************************************************************************/
/*--------------------------------------------Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/
void initFanTomPwm(void)
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

    uint32 fxclkFreq = IfxGtm_Cmu_getModuleFrequency(&MODULE_GTM);
    uint32 period = g_fanConfig.period;
    float period_s = (float)period / fxclkFreq;       // 초 단위
    float period_ms = period_s * 1000.0f;              // 밀리초
    float period_us = period_s * 1000000.0f;           // 마이크로초

    print("--------------------------\n\r");
    print("FXCLK Frequency: %d Hz\n\r", fxclkFreq);
    print("Period ticks: %d\n\r", period);
    print("Period: %.4f s\n\r", period_s);
    print("Period: %.3f ms\n\r", period_ms);
    print("Period: %.0f us\n\r", period_us);
}
void initServoPwm(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */

    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock*/
    /* Initialize the configuration structure with default parameters */
    IfxGtm_Tom_Pwm_initConfig(&g_servoConfig, &MODULE_GTM);

    g_servoConfig.tom = SERVO.tom;                                      /* Select the TOM depending on the LED          */
    g_servoConfig.tomChannel = SERVO.channel;                           /* Select the channel depending on the LED      */
    g_servoConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    g_servoConfig.period = SERVO_PWM;                                /* Set the timer period                         */
    g_servoConfig.dutyCycle = 60;
    g_servoConfig.pin.outputPin = &SERVO;                               /* Set the LED port pin as output               */
    g_servoConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_servoDriver, TRUE);                       /* Start the PWM                                */
    uint32 fxclkFreq = IfxGtm_Cmu_getModuleFrequency(&MODULE_GTM);
    uint32 period = g_servoConfig.period;
    uint32 period2 = g_servoConfig.clock;
    float period_s = (float)period / fxclkFreq;       // 초 단위
    float period_ms = period_s * 1000.0f;              // 밀리초
    float period_us = period_s * 1000000.0f;           // 마이크로초


    print("--------------------------\n\r");
    print("FXCLK Frequency: %d Hz\n\r", fxclkFreq);
    print("Period ticks: %d\n\r", period);
    print("Period: %.4f s\n\r", period_s);
    print("Period: %.3f ms\n\r", period_ms);
    print("Period: %.0f us\n\r", period_us);





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




