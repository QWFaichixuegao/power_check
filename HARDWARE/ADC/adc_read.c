#include "adc_read.h"
#include "i2c.h"
#include "at24cxx.h"
READ_ADC read_adc;
HANDLE 	 handle;
USARTX_HANDLE usart1_handle;
SAVEADC_DATA  saveadc_data;
//char ADC_BUF[ADC_BUFLEN] __attribute__ ((at(0X20001500)));
/** 

	*/
void adc_init(READ_ADC * read_adc)
{
	read_adc->adc_value[0] 	= 0;
	read_adc->adc_value[1] 	= 0;	
	handle.flagstate = SAVE;
	handle.savecount = 0;
	saveadc_data.send_count=0;
	saveadc_data.data_countbuf[2]=',';
	AT24CXX_WriteData(0x0002, &saveadc_data.data_countbuf[2], 1);
	memset(saveadc_data.clean_data,0x00,128);
	HAL_I2C_Mem_Read_DMA(&hi2c1, R_IIC_ADDR_AT24CXX, 0x0000, I2C_MEMADD_SIZE_16BIT, saveadc_data.data_countbuf, 2);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}	
  saveadc_data.data_count	= (saveadc_data.data_countbuf[1]<<8)|saveadc_data.data_countbuf[0];
}



