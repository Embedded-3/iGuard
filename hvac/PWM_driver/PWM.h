
#ifndef PWM_H_
#define PWM_H_

#define ISR_PRIORITY_TOM    20                                      /* Interrupt priority number                    */
#define FAN                 IfxGtm_TOM0_9_TOUT1_P02_1_OUT
#define SERVO               IfxGtm_TOM0_1_TOUT103_P10_1_OUT //IfxGtm_TOM0_13_TOUT5_P02_5_OUT //IfxGtm_TOM0_13_TOUT5_P02_5_OUT
#define PWM_PERIOD          100000                                   /* PWM period for the TOM                       */
#define SERVO_PWM           7812 //15625 //*2                      //2000000 //(50000)   //2000000 // 20ms 단위 1 이 10ns

#define SERVO_PWM_MIN       780/2 ///2 //(48) //100000
#define SERVO_PWM_CENTER    1170/2   //2 //(72) //150000 
#define SERVO_PWM_MAX       1560/2    ///2 //(96) //200000


void initGtmTomPwm(void);
void initServoPwm(void);
void setDutyCycle(uint32);
void setSERVODutyCycle(uint32);

#endif /* PWM_H_ */
