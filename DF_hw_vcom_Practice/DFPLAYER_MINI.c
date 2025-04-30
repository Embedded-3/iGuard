#include  "DFPLAYER_MINI.h"
#include "string.h"
#include "hw_vcom.h"
#include <stdio.h>

extern UART_HandleTypeDef huart1;

void Send_Cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
    Checksum = 0 - Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY);
}

void Sound_Track(uint8_t nums){
    switch (nums) {
			case 1:
				Send_Cmd(0x03, 0x00, 1);
			  break;
		  case 2:
			  Send_Cmd(0x03, 0x00, 2);
      default:
				break;
    }
}

void DF_Init (uint8_t volume)
{
	Printf("DF_Init works \r\n");
	Send_Cmd(0x3F, 0x00, Source); // initialize parameters
	HAL_Delay(2000);
	Send_Cmd(0x06, 0x00, volume);
	HAL_Delay(2000);
}
