#include "DataFlash.h"

uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) {
    uint16_t i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;
    
//    FLASH_UnlockBank1();
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
      FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
      i = i+2;
      iAddress = iAddress + 2;
      buf = buf + 2;
    }
    
    return iNumByteToWrite;
}
/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to write into flash
  * @retval if success return the number to write, -1 if error
  *  
  */
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite) {
                /* Unlock the Flash Bank1 Program Erase controller */
        uint32_t secpos;
        uint32_t iNumByteToWrite = iNbrToWrite;
				uint16_t secoff;
				uint16_t secremain;  
				uint16_t i = 0;    
        uint8_t tmp[FLASH_PAGE_SIZE];
				volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
        
        FLASH_UnlockBank1();
				secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;//扇区地址 
				secoff=iAddress & (FLASH_PAGE_SIZE -1);     //在扇区内的偏移
				secremain=FLASH_PAGE_SIZE-secoff;           //扇区剩余空间大小 
        
        
        if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//不大于4096个字节
				while( 1 ) {
					Flash_Read(secpos, tmp, FLASH_PAGE_SIZE);   //读出整个扇区
					for(i=0;i<secremain;i++) {       //校验数据
						if(tmp[secoff+i]!=0XFF) break;       //需要擦除 
					}
					if(i<secremain) {  //需要擦除
							FLASHStatus = FLASH_ErasePage(secpos); //擦除这个扇区
							if(FLASHStatus != FLASH_COMPLETE)
								return -1;
							for(i=0;i<secremain;i++) {   //复制
											tmp[i+secoff]=buf[i];   
							}
							Flash_Write_Without_check(secpos ,tmp ,FLASH_PAGE_SIZE);//写入整个扇区  
					} else {
							Flash_Write_Without_check(iAddress,buf,secremain);//写已经擦除了的,直接写入扇区剩余区间.
					}
            
					if(iNumByteToWrite==secremain) //写入结束了
							break;
					else {
							secpos += FLASH_PAGE_SIZE;
							secoff = 0;//偏移位置为0 
							buf += secremain;  //指针偏移
							iAddress += secremain;//写地址偏移    
							iNumByteToWrite -= secremain;  //字节数递减
							if(iNumByteToWrite>FLASH_PAGE_SIZE) secremain=FLASH_PAGE_SIZE;//下一个扇区还是写不完
							else secremain = iNumByteToWrite;  //下一个扇区可以写完了
					}   
        }
        FLASH_LockBank1();
        return iNbrToWrite; 
}






/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to read from flash
  * @retval if success return the number to write, without error
  *  
  */
int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) {
        int i = 0;
        while(i < iNbrToRead ) {
           *(buf + i) = *(__IO uint8_t*) iAddress++;
           i++;
        }
        return i;
}
