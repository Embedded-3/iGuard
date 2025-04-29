// dht22.c

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "GTM_TOM_PWM.h"
#include "Bsp.h"

#include "IfxPort.h"
#include "IfxPort_PinMap.h"

#include "Driver_Stm.h"
#include "Fnd.h"
#include "Sound.h"

#include "Driver_Adc.h"

//GPIO related
#define PCn_2_IDX 19
#define P2_IDX 2
#define PCn_1_IDX 11
#define P1_IDX 1

#define PCn_0_IDX 3
#define SCLK IfxPort_P00_0
#define RCLK IfxPort_P00_1
#define DIO IfxPort_P00_2



// ERU related
#define EXIS0_IDX 4
#define FEN0_IDX 8
#define EIEN0_IDX 11
#define INP0_IDX 12
#define IGP0_IDX 14

// SRC related
#define SRE_IDX 10
#define TOS_IDX 11


#define WAIT_TIME   10              /* Number of milliseconds to wait between each duty cycle change                */


#define DHT11_PORT IfxPort_P10_4  // DHT11 데이터 핀
#define TIMEOUT 10000  // 타임아웃 값

typedef struct
{
    uint32_t u32nuCnt1ms;
    uint32_t u32nuCnt10ms;
    uint32_t u32nuCnt100ms;
    uint32_t u32nuCnt1000ms;

}TestCnt;

typedef struct{
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
    uint8_t checksum;
}DHT11_Data;    //40bit

// Task scheduling related
void AppScheduling(void);
void AppTask1ms(void);
void AppTask10ms(void);
void AppTask100ms(void);
void AppTask1000ms(void);

void send(unsigned char);
void send_port(unsigned char, unsigned char);
void delay(double);
void sendInt(int);
void delay_fnd(double);
void status_action(int n);
void initGPIO(void);
//void initERU(void);

//void DHT11_process(void);
void delay_us(uint32_t us);
uint8_t DHT11_ReadByte(void);
int DHT11_process(DHT11_Data *data);

/***********************************************************************/
/*Variable*/
/***********************************************************************/
TestCnt stTestCnt;

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int p4, p3, p2, p1; //각 포트 출력 값
uint32_t adcResult=0;


int core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    /* Initialize a time variable */
    Ifx_TickTime ticksFor10ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME);

    /* Initialize GTM TOM module */


    initGPIO();
    Driver_Stm_Init();
    Driver_Adc_Init();
    initGtmTomPwm();

    while(1)
    {
        AppScheduling();
        //        IfxPort_P02_7 blue
        //        IfxPort_P10_5 green
        //        IfxPort_P10_3 red

//        fadeLED(adcResult);                  /* Change the intensity of the LED  */
//        waitTime(ticksFor10ms);     /* Delay of 10ms              */

        int sw1 = IfxPort_getPinState(IfxPort_P02_0.port, IfxPort_P02_0.pinIndex);
        int sw2 = IfxPort_getPinState(IfxPort_P02_1.port, IfxPort_P02_1.pinIndex);

        DHT11_Data data;
        int result = DHT11_process(&data);  // DHT11 데이터 읽기
        sendInt(data.humidity_int);
        //sendInt(data.temperature_int);

        //waitTime(ticksFor10ms);

        //sendInt(adcResult);
        //delay_fnd(0.001/5*5);
    }
    return (1);
}
void status_action(int n){

}

void initGPIO(void){
    /*P10_2    Digital Output*/ //파랑
    IfxPort_setPinModeOutput(IfxPort_P10_2.port, IfxPort_P10_2.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(IfxPort_P10_2.port, IfxPort_P10_2.pinIndex);

    /*P10_1    Digital Output*/ //빨강
    IfxPort_setPinModeOutput(IfxPort_P10_1.port, IfxPort_P10_1.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(IfxPort_P10_1.port, IfxPort_P10_1.pinIndex);


    IfxPort_setPinModeOutput(SCLK.port, SCLK.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(RCLK.port, RCLK.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(DIO.port, DIO.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxPort_setPinModeInput(IfxPort_P02_1.port, IfxPort_P02_1.pinIndex, IfxPort_Mode_inputPullUp);  //sw2
    IfxPort_setPinModeInput(IfxPort_P02_0.port, IfxPort_P02_0.pinIndex, IfxPort_Mode_inputPullUp);  //sw1

    //D9 D10 D11
    IfxPort_setPinModeOutput(IfxPort_P02_7.port, IfxPort_P02_7.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(IfxPort_P10_5.port, IfxPort_P10_5.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(IfxPort_P10_3.port, IfxPort_P10_3.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxPort_setPinModeInput(IfxPort_P10_4.port, IfxPort_P10_4.pinIndex, IfxPort_Mode_inputPullUp);  //DHT11

}


void send(unsigned char X)
{
  for (int i = 8; i >= 1; i--)
  {
    if (X & 0x80)
    {
        IfxPort_setPinHigh(DIO.port, DIO.pinIndex);
    }
    else
    {
        IfxPort_setPinLow(DIO.port, DIO.pinIndex);
    }
    X <<= 1;
    IfxPort_setPinLow(SCLK.port, SCLK.pinIndex);
    IfxPort_setPinHigh(SCLK.port, SCLK.pinIndex);
  }
}

void send_port(unsigned char X, unsigned char port)
{
  send(X);
  send(port);
  IfxPort_setPinLow(RCLK.port, RCLK.pinIndex);
  IfxPort_setPinHigh(RCLK.port, RCLK.pinIndex);
}

void AppTask1ms(void)
{
    stTestCnt.u32nuCnt1ms++;
    adcResult = Driver_Adc0_DataObtain();
    Driver_Adc0_ConvStart();
}

void AppTask10ms(void)
{
    stTestCnt.u32nuCnt10ms++;



}

void AppTask100ms(void)
{
    stTestCnt.u32nuCnt100ms++;

    //IfxPort_togglePin(IfxPort_P10_1.port, IfxPort_P10_1.pinIndex);
}

void AppTask1000ms(void)
{

    stTestCnt.u32nuCnt1000ms++;


    //IfxPort_togglePin(IfxPort_P10_2.port, IfxPort_P10_2.pinIndex);
}

void AppScheduling(void)
{
    if(stSchedulingInfo.u8nuScheduling1msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        AppTask1ms();

        if(stSchedulingInfo.u8nuScheduling10msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            AppTask10ms();
        }

        if(stSchedulingInfo.u8nuScheduling100msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            AppTask100ms();
        }
        if(stSchedulingInfo.u8nuScheduling1000msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling1000msFlag = 0u;
            AppTask1000ms();
        }
    }
}
void delay(double n){
    for(int i=0;i<1000000 * n;i++){
        ;
    }
}
void sendInt(int t){
    if(t>9999) t=0;
    if(t<= 9999){
        p4 = t/1000;
        p3 = t/100 - p4 * 10;
        p2 = t/10 - p4 * 100 - p3 * 10;
        p1 = t%10;

        send_port(LED[p1], PORT[1]);
        send_port(LED[p2], PORT[2]);
        send_port(LED[p3], PORT[3]);
        send_port(LED[p4], PORT[4]);
    }
    else{   //비정상적
        send_port(LED[0], PORT[1]);
        send_port(LED[0], PORT[2]);
        send_port(LED[p3], PORT[3]);
        send_port(LED[0], PORT[4]);
    }
    delay_fnd(0.001/5*7);
}
void delay_fnd(double n){
    for(int i=0;i<10000000 * n;i++){
        send_port(LED[p1], PORT[1]);
        send_port(LED[p2], PORT[2]);
        send_port(LED[p3], PORT[3]);
        send_port(LED[p4], PORT[4]);

    }
}


void delay_us(uint32_t us) {
    Ifx_TickTime ticks = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, us);
    waitTime(ticks);
}

uint8_t DHT11_ReadByte(void) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        while (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex) == 0); // LOW면 대기
        delay_us(30);  // 26~28us면 '0', 70us면 '1'

        if (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex)) {    //70us이상일 때 High기록
            result |= (1 << (7 - i));   //MSB부터 write
        }

        while (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex) == 1); // HIGH면 대기
    }
    return result;
}

int DHT11_process(DHT11_Data *data) {
    uint8_t buffer[5] = {0};

    // MCU → DHT11: Start Signal
    IfxPort_setPinModeOutput(DHT11_PORT.port, DHT11_PORT.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(DHT11_PORT.port, DHT11_PORT.pinIndex);
    delay_us(18000);  // 최소 18ms 동안 LOW 유지
    IfxPort_setPinHigh(DHT11_PORT.port, DHT11_PORT.pinIndex);
    delay_us(40);  // 20~40us 동안 HIGH 유지

    // MCU ← DHT11: 응답 대기
    IfxPort_setPinModeInput(DHT11_PORT.port, DHT11_PORT.pinIndex, IfxPort_Mode_inputPullUp);

    //DHT11에서 Low -> High -> Low 보내옴
    uint32_t timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex) == 1) {
        if (--timeout == 0) return -1;  // 응답 없음
    }
    timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex) == 0) {
        if (--timeout == 0) return -1;
    }
    timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT11_PORT.port, DHT11_PORT.pinIndex) == 1) {
        if (--timeout == 0) return -1;
    }

    // 데이터 수신 (40비트 = 5바이트)
    for (int i = 0; i < 5; i++) {
        buffer[i] = DHT11_ReadByte();
    }

    // 체크섬 검증
    if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) {
        return -2;  // 체크섬 오류
    }

    // 데이터 저장
    data->humidity_int = buffer[0];
    data->humidity_dec = buffer[1];
    data->temperature_int = buffer[2];
    data->temperature_dec = buffer[3];
    data->checksum = buffer[4];


    return 0;
}
