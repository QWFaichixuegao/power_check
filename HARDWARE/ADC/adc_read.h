#ifndef __ADC_READ_H_
#define __ADC_READ_H_
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#define ADC_BUFLEN 10*1024
extern char ADC_BUF[ADC_BUFLEN];
typedef struct {
	__IO uint16_t  adc_value[2];		
	float          charge;
	float          usb;
}READ_ADC;
extern READ_ADC read_adc;

typedef enum
{
	SAVE	 =1,
	SEND	 =2,
	CLEAN  =3,
	STOP   =4, 
	WRITE  =5,
	READ	 =6
} CAR_STATE;


typedef struct {
	
	uint8_t flagstate ;
	uint8_t getdataflag ;	
		uint8_t usbflag;
	uint16_t	savecount;
	
}HANDLE;
extern HANDLE handle;

#define RX_SIZE     8
#define SAVE_SIZE   8
typedef struct {
	uint16_t  rx_count;
	uint16_t  rx_len;
	uint8_t   save_buf[SAVE_SIZE];
	uint8_t   rx_buf[RX_SIZE];	
}USARTX_HANDLE;
extern USARTX_HANDLE usart1_handle;

typedef struct
{

	uint8_t  at24_r_data[128];	
	uint8_t  clean_data[128];	
	uint8_t  data_countbuf[8];	
	uint8_t  at24_w_data[8];
	uint8_t  boot_connectIOT_tomes;
	uint16_t data_count;
	uint16_t send_count;	
	uint16_t clean_count;	
} SAVEADC_DATA;
extern SAVEADC_DATA saveadc_data;	

void adc_init(READ_ADC * read_adc);

#endif

