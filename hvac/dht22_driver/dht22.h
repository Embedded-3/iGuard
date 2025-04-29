// dht22.h
#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>
#include <stdbool.h>
#include "Bsp.h"
#include "IfxPort.h"
#include "IfxPort_PinMap.h"

typedef struct{
    uint32 humidity;
    uint32 temperature;
}DHT22_Data;

void delay_us(uint32);
uint8 DHT11_ReadByte(void);
int DHT11_process(DHT22_Data*);

#endif // DHT22_H
