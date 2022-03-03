
#include "w25qxx.h" 
#include "delay.h" 

u16 W25QXX_TYPE = W25Q256;

void W25QXX_Init(void)
{ 
  SPI_InitType SPI_InitStruct;
  GPIO_InitType GPIO_InitStruct;
  
  /* CSN IOC0 output-CMOS mode */
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUTPUT_CMOS;
  GPIOBToF_Init(GPIOC,&GPIO_InitStruct);
 
  W25QXX_CS_1;
  
  //SPI2 Initlization
  SPI_DeviceInit(SPI2);
  SPI_InitStruct.Mode          = SPI_MODE_MASTER;
  SPI_InitStruct.SPH           = SPI_SPH_1;   
  SPI_InitStruct.SPO           = SPI_SPO_1;   
  SPI_InitStruct.ClockDivision = SPI_CLKDIV_2;
  SPI_InitStruct.CSNSoft       = SPI_CSNSOFT_ENABLE;
  SPI_InitStruct.SWAP          = SPI_SWAP_DISABLE;
  SPI_Init(SPI2, &SPI_InitStruct);
  SPI_ReceiveFIFOLevelConfig(SPI2, SPI_RXFLEV_7);
  SPI_Cmd(SPI2, ENABLE);
  
  SPI_ReadWriteByte(0xff);
  //Read Flash ID
  //W25QXX_TYPE = W25QXX_ReadID();
} 

//读取W25QXX的状态寄存器
// Read W25QXX status register
// BIT 7    6   5   4   3   2   1   0
//     SPR  RV  TB  BP2 BP1 BP0 WEL BUSY
// SPR             :  Default value 0, status register protect bit
// TB,BP2,BP1,BP0  :  FLASH write-protection setting
// WEL             :  Write Enable Lock
// BUSY            :  Busy flag (1 busy, 0 not busy)
// return value    :  Default value 0x00
u8 W25QXX_ReadSR(void)   
{  
  u8 byte = 0;   
  W25QXX_CS_0;                              // Enable Device   
  SPI_ReadWriteByte(W25X_ReadStatusReg);    // Send Command, read status register    
  byte=SPI_ReadWriteByte(0Xff);             // Read 1 byte 
  W25QXX_CS_1;                              // Disable CS     
  return byte;   
} 

// Write W25QXX status registert
void W25QXX_Write_SR(u8 sr)   
{   
  W25QXX_CS_0;                            // Enable Device   
  SPI_ReadWriteByte(W25X_WriteStatusReg); // Send Command, write status register    
  SPI_ReadWriteByte(sr);                  // write 1 byte  
  W25QXX_CS_1;                            // Disable CS     	      
}   

// W25QXX Enable Write	   
void W25QXX_Write_Enable(void)   
{
  W25QXX_CS_0;                            // Enable Device  
  SPI_ReadWriteByte(W25X_WriteEnable);    // Enable Write  
  W25QXX_CS_1;                            // Disable CS     	      
} 

//W25QXX写禁止	
void W25QXX_Write_Disable(void)   
{  
  W25QXX_CS_0;                            // Enable Device   
  SPI_ReadWriteByte(W25X_WriteDisable);   // Disable Write    
  W25QXX_CS_1;                            // Disable CS     	      
} 		

// Read Chip ID
// Return value:				   
// 0XEF13, W25Q80  
// 0XEF14, W25Q16    
// 0XEF15, W25Q32  
// 0XEF16, W25Q64 
// 0XEF17, W25Q128
// 0X4019, W25Q256
u16 W25QXX_ReadID(void)
{
  u16 Temp = 0;	  
  W25QXX_CS_0;				    
  SPI_ReadWriteByte(0x90);	    
  SPI_ReadWriteByte(0x00); 	    
  SPI_ReadWriteByte(0x00); 	    
  SPI_ReadWriteByte(0x00); 	 			   
  Temp|=SPI_ReadWriteByte(0xFF)<<8;  
  Temp|=SPI_ReadWriteByte(0xFF);	 
  W25QXX_CS_1;				    
  return Temp;
}   		    

// Read SPI FLASH  
// pBuffer: the pointer of read buffer
// ReadAddr: the start-address of operation(24bit)
// NumByteToRead: the length of operation(max 65535)
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
  u16 i;   										    
  W25QXX_CS_0;                             
  SPI_ReadWriteByte(W25X_ReadData);         
  SPI_ReadWriteByte((u8)((ReadAddr)>>16));    
  SPI_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
  { 
    pBuffer[i]=SPI_ReadWriteByte(0XFF);  
  }
  W25QXX_CS_1;  				    	      
}  

// SPI page write(the length must less 256 byte)	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i;  
  W25QXX_Write_Enable();
  W25QXX_CS_0;                              
  SPI_ReadWriteByte(W25X_PageProgram);         
  SPI_ReadWriteByte((u8)((WriteAddr)>>16));     
  SPI_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)SPI_ReadWriteByte(pBuffer[i]);  
  W25QXX_CS_1;                             
  W25QXX_Wait_Busy();					   //等待写入结束
} 
// Write SPI FLASH without chechsum 
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
  u16 pageremain;	   
  pageremain=256-WriteAddr%256;		 	    
  if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
  while(1)
  {	   
    W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
    if(NumByteToWrite==pageremain)break;
    else 
    {
      pBuffer+=pageremain;
      WriteAddr+=pageremain;	
      NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
      if(NumByteToWrite>256)pageremain=256; 
      else pageremain=NumByteToWrite; 	  
    }
  };	    
} 

//write SPI FLASH 每幅图片240x240分辨率,115200bytes,
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
  u32 secpos;
  u16 secoff;
  u16 secremain;
  u16 i; 
  u8 * W25QXX_BUF;
  W25QXX_BUF=W25QXX_BUFFER;	     
  secpos=WriteAddr/4096;// Sector-address  
  secoff=WriteAddr%4096;// Sector-offset-address
  secremain=4096-secoff;// Size of Sector remained
  if(NumByteToWrite <= secremain)
    secremain = NumByteToWrite;
  while(1) 
  {	
    W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
    for(i=0;i<secremain;i++)
    {
      if(W25QXX_BUF[secoff+i]!=0XFF)break;  	  
    }
    if(i<secremain)
    {
      W25QXX_Erase_Sector(secpos);
      for(i=0;i<secremain;i++)	  
      {
        W25QXX_BUF[i+secoff]=pBuffer[i];
      }
      W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);  
    }
    else 
      W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain); 	
    
    if(NumByteToWrite==secremain)break;
    else
    {
      secpos++;
      secoff=0; 	 
      
      pBuffer+=secremain;  
      WriteAddr+=secremain;	   
      NumByteToWrite-=secremain;				//字节数递减
      if(NumByteToWrite>4096)secremain=4096;
      else 
        secremain=NumByteToWrite;			//下一个扇区可以写完了
    }	 
  };	 
}

// Chip erase		  
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();       
    W25QXX_Wait_Busy();   
    W25QXX_CS_0;                           
    SPI_ReadWriteByte(W25X_ChipErase);  
    W25QXX_CS_1;                          	      
    W25QXX_Wait_Busy();   	
}   

// Sector erase
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{    	  
 Dst_Addr*=4096;
 W25QXX_Write_Enable();  	 
 W25QXX_Wait_Busy();   
 W25QXX_CS_0;               
 SPI_ReadWriteByte(W25X_SectorErase);    
 SPI_ReadWriteByte((u8)((Dst_Addr)>>16));   
 SPI_ReadWriteByte((u8)((Dst_Addr)>>8));   
 SPI_ReadWriteByte((u8)Dst_Addr);  
 W25QXX_CS_1;                               	      
 W25QXX_Wait_Busy(); 
} 

// Wait for Idle
void W25QXX_Wait_Busy(void)   
{   
  while((W25QXX_ReadSR()&0x01)==0x01);
}  

// Enter power-down mode
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS_0;                              
  SPI_ReadWriteByte(W25X_PowerDown);    
  W25QXX_CS_1;                             	      
  delay_us(3);                
}   

// Wake up chip
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS_0;                              
  SPI_ReadWriteByte(W25X_ReleasePowerDown);// send W25X_PowerDown command 0xAB    
  W25QXX_CS_1;                               	      
  delay_us(3);                  
}   


//=========================SPI DRIVER================================//
//SPI Transfer one byte
u8 SPI_ReadWriteByte(u8 TxData)
{		 			 
  while (SPI_GetStatus(SPI2, SPI_STS_TFE) == 0){} 
	SPI_SendData(SPI2, TxData);
		
  while (SPI_GetStatus(SPI2, SPI_STS_RNE) == 0){}  
	return SPI_ReceiveData(SPI2);	
}
