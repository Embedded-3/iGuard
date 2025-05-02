//mq135.h
#ifndef MQ135_H
#define MQ135_H

#include"asclin_driver/asclin.h"
#include "Ifx_Types.h"
#include "IfxVadc.h"
#include "IfxVadc_Adc.h"
#include "math.h"

enum{
    CO = 0,
    Alcohol = 1,
    CO2 = 2,
    Toluen = 3,
    NH4 = 4,
    Aceton = 5,
};

typedef struct{
    double a;
    double b;
}Gas;


typedef struct
{
    IfxVadc_Adc vadc; /* VADC handle */
    IfxVadc_Adc_Group adcGroup;
} App_VadcAutoScan;



IFX_EXTERN App_VadcAutoScan g_VadcAutoScan;


void Driver_MQ135_Init(void);
void Driver_Adc0_ConvStart(void);
uint16 Driver_Adc0_DataObtain(void);
double calculate_ppm(uint32 adcval);

/*
  Exponential regression:
GAS      | a      | b
CO       | 605.18 | -3.937
Alcohol  | 77.255 | -3.18
CO2      | 110.47 | -2.862
Toluen  | 44.947 | -3.445
NH4      | 102.2  | -2.473
Aceton  | 34.668 | -3.369
*/


#endif // MQ135_H
