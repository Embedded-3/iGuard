// PWM.h
 #ifndef PWM_H_
 #define PWM_H_
 
#define ISR_PRIORITY_TOM    20                                      /* Interrupt priority number                    */
#define FAN                 IfxGtm_TOM0_9_TOUT1_P02_1_OUT   // D3 핀
#define SERVO               IfxGtm_TOM0_13_TOUT5_P02_5_OUT  // D7 핀
#define PWM_PERIOD          60000                                   /* PWM period for the TOM                       */

#define SERVO_PWM           7812
#define SERVO_PWM_CENTER    680
#define SERVO_PWM_MIN       290
#define SERVO_PWM_MAX       1000

#include "hvac_ctl.h"
#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"
#include "asclin.h"
#include "IfxGtm_Cmu.h"
 
void initFanTomPwm(void);
void initServoPwm(void);
void controlFan(uint8 speed);
void changeMode(uint8 mode);
 
 #endif /* PWM_H_ */
