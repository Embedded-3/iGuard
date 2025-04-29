// dht22.c

#include "dht22.h"

#define DHT11_PORT IfxPort_P02_0  // DHT11 데이터 핀
#define TIMEOUT 10000  // 타임아웃 값

void delay_us(uint32 us) {
    Ifx_TickTime ticks = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, us);
    waitTime(ticks);
}

uint8 DHT11_ReadByte(void) {
     uint8 result = 0;
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

int DHT11_process(DHT22_Data* data) {

    // MCU → DHT11: Start Signal
    IfxPort_setPinModeOutput(DHT11_PORT.port, DHT11_PORT.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(DHT11_PORT.port, DHT11_PORT.pinIndex);
    delay_us(8000);  // 최소 8ms 동안 LOW 유지  // modified
    IfxPort_setPinHigh(DHT11_PORT.port, DHT11_PORT.pinIndex);
    delay_us(40);  // 20~40us 동안 HIGH 유지

    // MCU ← DHT11: 응답 대기
    IfxPort_setPinModeInput(DHT11_PORT.port, DHT11_PORT.pinIndex, IfxPort_Mode_inputPullUp);

    //DHT11에서 Low -> High -> Low 보내옴
    uint32 timeout = TIMEOUT;
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
    uint8 buffer[5] = {0};
    for (int i = 0; i < 5; i++) {
        buffer[i] = DHT11_ReadByte();
    }

    // 체크섬 검증
    if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) {
        return -2;  // 체크섬 오류
    }

    // 데이터 저장
    data->humidity = (uint32)(((uint16_t)buffer[0] << 8) | buffer[1]);
    data->temperature = (uint32)(((uint16_t)buffer[2] << 8) | buffer[3]);

    return 0;
}
