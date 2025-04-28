#include  "DFPLAYER_MINI.h"
#include <stdio.h>

#define SOURCE_TF_CARD 0x02

int flag1,flag2,i;
int audio_num=0;
int audio_flag=0;
int audio_state=0;

extern UART_HandleTypeDef huart1;

void checkState_DF() {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET) {
        // Busy == Low: audio is playing
        audio_state=1;
    } else {
        // Busy == High: audio is not playing
       audio_state=2;
    }
}

void Send_Cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
    Checksum = 0 - Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY);
}

void Query_Status(void) {
    // query the current status
    Send_Cmd(0x42, 0x00, 0x00);

    // response receive buffer
    uint8_t response[10] = {0};
    if (HAL_UART_Receive(&huart1, response, 10, 100) == HAL_OK) {
        // response process
        if (response[3] == 0x42) { // response == status information 
            uint8_t current_track = response[6]; // check current track number
            if (current_track == 0) {
                audio_state = 0; // status that playing is completed
            } else {
                audio_state = 1; // status that playing is on going
            }
        }
    }
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

	audio_flag=0;
}

//****************************Send_Command************************************//
void DF_Init (uint8_t volume)
{
	Send_Cmd(0x3F, 0x00, Source); // initialize parameters
	HAL_Delay(200);
	Send_Cmd(0x06, 0x00, volume);
	HAL_Delay(500);
}
	//****************************Check_Key Code***************************************//
