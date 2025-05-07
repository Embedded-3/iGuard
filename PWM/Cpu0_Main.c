#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxPort.h"

IfxCpu_syncEvent g_cpuSyncEvent = 0;

/* 모터 핀 매핑 */
#define MOTOR_PWM    &MODULE_P02, 0   // PWM_A (P2.0)
#define MOTOR_DIR    &MODULE_P10, 1   // DIR_A (P10.1)
#define MOTOR_BRAKE  &MODULE_P02, 7

/* 함수 선언 */
void initMotorPins(void);


void core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    initMotorPins();  // 모터 핀 초기화


    while(1)
    {
        // 출력 HIGH  brake LOW
        for(volatile int i=0;i<40000000;i++);
        IfxPort_setPinHigh(MOTOR_DIR);
        IfxPort_setPinLow(MOTOR_BRAKE);
        // brake HIGH
        for(volatile int i=0;i<10000000;i++);
        IfxPort_setPinHigh(MOTOR_BRAKE);
        for(volatile int i=0;i<40000000;i++);
        IfxPort_setPinLow(MOTOR_DIR);
        IfxPort_setPinLow(MOTOR_BRAKE);
        for(volatile int i=0;i<10000000;i++);
        IfxPort_setPinHigh(MOTOR_BRAKE);
//        for(volatile int i=0;i<3000000;i++);
//        IfxPort_setPinLow(MOTOR_BRAKE);
//        for(volatile int i=0;i<3000000;i++);
//        IfxPort_setPinLow(MOTOR_DIR);
    }
}

/* 모터 방향 핀 초기화 */
void initMotorPins(void)
{
    IfxPort_setPinModeOutput(MOTOR_DIR, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(MOTOR_BRAKE, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    /* 초기 상태: 정지 */
    IfxPort_setPinLow(MOTOR_DIR);
    IfxPort_setPinHigh(MOTOR_BRAKE);
}
