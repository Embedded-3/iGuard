#include "PWM.h"
#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"
#include "asclin.h"
#include "IfxGtm_Cmu.h"


#define ISR_PRIORITY_TOM    20                                      /* Interrupt priority number                    */
#define FAN                 IfxGtm_TOM0_9_TOUT1_P02_1_OUT
#define SERVO               IfxGtm_TOM0_13_TOUT5_P02_5_OUT
#define PWM_PERIOD          60000                                   /* PWM period for the TOM                       */
#define SERVO_PWM           7812

#define SERVO_PWM_CENTER    680
#define SERVO_PWM_MIN       290
#define SERVO_PWM_MAX       1000


// FAN용
IfxGtm_Tom_Pwm_Config g_fanConfig;
IfxGtm_Tom_Pwm_Driver g_fanDriver;

// SERVO용
IfxGtm_Tom_Pwm_Config g_servoConfig;
IfxGtm_Tom_Pwm_Driver g_servoDriver;



void setFanDutyCycle(uint32 dutyCycle);
void controlFan(Fan_Speed speed);
void setDutyCycle(uint32 dutyCycle);



void initFanTomPwm(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock                         */


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
//    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */
//    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock*/


    IfxGtm_Tom_Pwm_initConfig(&g_servoConfig, &MODULE_GTM);


    g_servoConfig.tom = SERVO.tom;                                      /* Select the TOM depending on the LED          */
    g_servoConfig.tomChannel = SERVO.channel;                           /* Select the channel depending on the LED      */
    g_servoConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    g_servoConfig.period = SERVO_PWM;                                /* Set the timer period                         */
    g_servoConfig.dutyCycle = SERVO_PWM_CENTER;
    g_servoConfig.pin.outputPin = &SERVO;                               /* Set the LED port pin as output               */
    g_servoConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_servoDriver, TRUE);                       /* Start the PWM                                */
}

void controlFan(Fan_Speed speed)
{
    uint32 Fanduty = 0;

    switch(speed)
    {
        case STOP:
            Fanduty = 0;
            break;
        case LOW_SPEED:
            Fanduty = PWM_PERIOD * 0.3;
            break;
        case MID_SPEED:
            Fanduty = PWM_PERIOD * 0.6;
            break;
        case HIGH_SPEED:
            Fanduty = PWM_PERIOD * 0.9;
            break;
        default:
            Fanduty = 0;
            break;
    }

    setFanDutyCycle(Fanduty);
}

void changeMode(Mode mode)
{
    uint32 Servoduty = 0;
    switch(mode)
    {
        case EXT_MODE:
            Servoduty = SERVO_PWM_CENTER;
            break;
        case INT_MODE:
            Servoduty = SERVO_PWM_MIN;
            break;
        default:
            Servoduty = 450;
            break;
    }

    setSERVODutyCycle(Servoduty);
}


void setFanDutyCycle(uint32 dutyCycle)
{
    g_fanConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_fanDriver, &g_fanConfig);                /* Re-initialize the PWM                        */

}

void setSERVODutyCycle(uint32 dutyCycle)
{
    g_servoConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Re-initialize the PWM                        */

}




