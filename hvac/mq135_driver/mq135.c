// mq135.c 로 저장해주세요

#include "mq135.h"


static void Driver_Adc0_Init(void);

App_VadcAutoScan g_VadcAutoScan;
IfxVadc_Adc_ChannelConfig adcChannelConfig[8];
IfxVadc_Adc_Channel   adcChannel[8];
uint16 adcDataResult[8] = {0u,};

// 핀 설정 : 핀 A2
uint32 chnIx = 5; //7;
uint16 groutId = IfxVadc_GroupId_4;

const Gas gas[6]=
{
    {605.18, -3.937},   // CO
    {77.255, -3.18},    // Alcohol
    {110.47, -2.862},   // CO2
    {44.947, -3.445},   // Toluen
    {102.2, -2.473},    // NH4
    {34.668, -3.369},   // Aceton
};


static void Driver_Adc0_Init(void)
{
    //uint32    chnIx = 7;

    /* VADC Configuration */

    /* create configuration */
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    /* initialize module */
    IfxVadc_Adc_initModule(&g_VadcAutoScan.vadc, &adcConfig);

    /* create group config */
    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcAutoScan.vadc);

    /* with group 0 */
    adcGroupConfig.groupId = groutId; //IfxVadc_GroupId_4;
    adcGroupConfig.master  = adcGroupConfig.groupId;

    /* enable scan source */
    adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;

    /* enable auto scan */
    adcGroupConfig.scanRequest.autoscanEnabled = TRUE;

    /* enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    /* initialize the group */
    /*IfxVadc_Adc_Group adcGroup;*/    //declared globally
    IfxVadc_Adc_initGroup(&g_VadcAutoScan.adcGroup, &adcGroupConfig);

    /*channel init*/
    IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &g_VadcAutoScan.adcGroup);

    adcChannelConfig[chnIx].channelId      = (IfxVadc_ChannelId)(chnIx);
    adcChannelConfig[chnIx].resultRegister = (IfxVadc_ChannelResult)(chnIx);  /* use dedicated result register */

    /* initialize the channel */
    IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);

    /* add to scan */
    unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
    unsigned mask     = channels;
    IfxVadc_Adc_setScan(&g_VadcAutoScan.adcGroup, channels, mask);

}


void Driver_MQ135_Init(void)
{
    /*ADC0 Converter Init*/
    Driver_Adc0_Init();

    /*ADC0 Converter Start*/
    Driver_Adc0_ConvStart();
}

void Driver_Adc0_ConvStart(void)
{
    /* start autoscan */
    IfxVadc_Adc_startScan(&g_VadcAutoScan.adcGroup);
}

uint16 Driver_Adc0_DataObtain(void)
{
    //uint32    chnIx = 7;
    Ifx_VADC_RES conversionResult; /* wait for valid result */

    /* check results */
    do
    {
        conversionResult = IfxVadc_Adc_getResult(&adcChannel[chnIx]);
    } while (!conversionResult.B.VF);

    adcDataResult[chnIx] = conversionResult.B.RESULT;
    return adcDataResult[chnIx];
}

double calculate_ppm(uint32 adcval)
{
    #define RL 1
    #define V_REF 5
    #define ADC_MAX_VAL 4095.0
    #define R0 3.6267

//    double sensor_volt = adcval * (V_REF/ADC_MAX_VAL);
//    double rs_cal = sensor_volt * RL / sensor_volt - RL;
//    double ratio = rs_cal/R0;
//    double ppm = 0;
//
//    print("|    CO   |  Alcohol |   CO2  |  Tolueno  |  NH4  |  Acteona  |\n\r");
//    print("|   ");
//    for(int i=0;i < 6;i++){
//        ppm = gas[i].a * pow(ratio, gas[i].b);
//        print("   |   %lf", ppm);
//    }
//    print("   |\n]r");

//    double ppm = a*pow(ratio, b);
//


    print("\n\radcval : %d\n\r", adcval);
    double sensor_volt = adcval * (V_REF/ADC_MAX_VAL);
    //print("sensor_V : %lf\n\r", sensor_volt);
    float rs_calc = (V_REF * RL)/ sensor_volt - RL;
    float ratio = rs_calc / R0;
    //print("%lf\n\r", ratio);

    double ppm =110.47 * pow(ratio, -2.862);

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

    return ppm;
}

