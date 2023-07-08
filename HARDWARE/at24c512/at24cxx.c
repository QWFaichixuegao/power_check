#include "at24cxx.h"
/** ����  ����EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
	*         ���ܳ���EEPROMҳ�Ĵ�С��
	* ����  ��
	*         -MemAddress �������ݵ�EEPROM�ĵ�ַ����WORD ADDRESS����ռ��Device Addressʱ�轫ռ�õ�λ�����
	*					-pBuffer 		Ҫд��EEPROM���ݵ�ָ�뻺����
	*         -BufferSize Ҫд��EEPROM���ֽ���
	*/
HAL_StatusTypeDef AT24CXX_PageWrite(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	uint8_t dev_addr;//EEPROM���豸��ַ    
	HAL_StatusTypeDef status = HAL_OK;
																			//16λ�ֽڵ�ַ��3�ֽڷ��͡�
//FIRST WORD ADDRESS�д�ŵ���  	1     0     1    0     A2   A1   A0   0��
//SECOND WORD ADDRESS�д�ŵ���	a15   a14   a13  a12    a11  a10   a9  a8��
//Device Address�д�ŵ��� 			 a7    a6    a5   a4     a3   a2   a1  a0��
	dev_addr = W_IIC_ADDR_AT24CXX | ((A2A1A0 << 1) & 0x0E);     
	status = HAL_I2C_Mem_Write_DMA(&hi2c1, dev_addr, (uint16_t)MemAddress, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
	{}
	/* Check if the EEPROM is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&hi2c1, dev_addr, 100, 100) == HAL_TIMEOUT);
	return status;
}

HAL_StatusTypeDef AT24CXX_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
	HAL_StatusTypeDef err = HAL_OK;

	Addr = MemAddress % AT24CXX_PAGE_SIZE;//��д���ַ�ǲ���128������������0�Ļ���ʼ��ַ�պ���ҳ��ʼ��ַ����Ϊ0����ʼ��ַƫ��ҳ��ʼ��ַ
	count = AT24CXX_PAGE_SIZE - Addr;//����ʼ��ַƫ��ҳ��ʼ��ַ����������ƫ���ַ��ʼ����ҳĩβ��д��ʣ���ֽ���
	NumOfPage =  BufferSize / AT24CXX_PAGE_SIZE;//��д��������ǲ���128������������0�Ļ�<=128
	NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;//ʣ��С��һҳ���ֽ�����(��ʼ��ַ�պ���ҳ��ʼ��ַд����ĩβ��һҳ����ʼ��ַƫ��ҳ��ʼ��ַд�ڿ�ͷ)
 
	//������ʼ��ַ��������������ֱ��ж�
	if(Addr == 0)//���д���ַ��ҳ��ַ ���� ����ʼ��ַ�պ���ҳ��ʼ��ַ
		{
			//����д����ֽ�����������������ֱ��ж� 
			if(NumOfPage == 0)//���д���С С�ڵ��� ҳ��С 
			{
				//////  д��ҳ  //////
				return (AT24CXX_PageWrite(MemAddress,pBuffer,BufferSize));
			}
			else/*���д���С ���� ҳ��С*/
			{
				/*-------д��ҳ--------*/
				while(NumOfPage--)
				{
					err = AT24CXX_PageWrite(MemAddress,pBuffer,AT24CXX_PAGE_SIZE);
					if(err != HAL_OK)
						return err; 
					MemAddress +=  AT24CXX_PAGE_SIZE;
					pBuffer += AT24CXX_PAGE_SIZE;
				}
				/*------д���һҳ------*/
				if(NumOfSingle!=0)
				{
					return (AT24CXX_PageWrite(MemAddress,pBuffer,NumOfSingle)); 
				}
			}
		}
	else//���д���ַ��ҳ��ַ ������ ����ʼ��ַƫ��ҳ��ʼ��ַ
		{
			//����д����ֽ�����������������ֱ��ж�
			if(BufferSize <= count)//���д���С С�ڵ��� ҳʣ���С
			{
				//////  д��ҳ  //////
				return (AT24CXX_PageWrite(MemAddress,pBuffer,NumOfSingle));
			}
			else/*���д���С ���� ҳʣ���С*/
			{
				BufferSize -= count;
				NumOfPage =  BufferSize / AT24CXX_PAGE_SIZE;
				NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;	
				/*-------��д��ǰҳ--------*/
				err = AT24CXX_PageWrite(MemAddress,pBuffer,count);
				if(err != HAL_OK)
					return err; 
				MemAddress += count;
				pBuffer += count;
				/*-------д�м���ҳ--------*/
				while(NumOfPage--)
				{
					err = AT24CXX_PageWrite(MemAddress,pBuffer,AT24CXX_PAGE_SIZE);
					if(err != HAL_OK)
						return err;
					MemAddress +=  AT24CXX_PAGE_SIZE;
					pBuffer += AT24CXX_PAGE_SIZE;  
				}
				/*-------д���һҳ--------*/
				if(NumOfSingle != 0)
				{
					return (AT24CXX_PageWrite(MemAddress,pBuffer,NumOfSingle)); 
				}
			}
		}
	return err;
}

