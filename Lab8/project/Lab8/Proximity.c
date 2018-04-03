// I2C3SCL connected to PD0 and to SCL on VL53L0X
// I2C3SDA connected to PD1 and to SDA on VL53L0X
// SCL and SDA lines pulled to +3.3 V with 10 k resistors
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              10           // number of receive attempts before giving up
void Proximity_Init(void){
  SYSCTL_RCGCI2C_R |= 0x0008;           // activate I2C3
  SYSCTL_RCGCGPIO_R |= 0x0008;          // activate port D
  while((SYSCTL_PRGPIO_R&0x0008) == 0){};// ready?

  GPIO_PORTD_AFSEL_R |= 0x03;           // 3) enable alt funct on PD0,1
  GPIO_PORTD_ODR_R |= 0x02;             // 4) enable open drain on PB3 only
  GPIO_PORTD_DEN_R |= 0x03;             // 5) enable digital I/O on PD0,1
                                        // 6) configure PD0,1 as I2C
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFFFF00)+0x00000033;
  GPIO_PORTD_AMSEL_R &= ~0x03;          // 7) disable analog functionality on PD0,1
  I2C3_MCR_R = I2C_MCR_MFE;      // 9) master function enable
  I2C3_MTPR_R = 24;              // 8) configure for 100 kbps clock
  // 20*(TPR+1)*20ns = 10us, with TPR=24
}

// receives one byte from specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
// Note for TMP102 thermometer only:
// Used to read the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
uint8_t I2C_Recv(int8_t slave){
  int retryCounter = 1;
  do{
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C3_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C3_MCS_R = (0
                        // & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C3_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (I2C3_MDR_R&0xFF);          // usually returns 0xFF on error
}
// receives two bytes from specified slave
// Note for HMC6352 compass only:
// Used with 'A' commands
// Note for TMP102 thermometer only:
// Used to read the contents of the pointer register
uint16_t I2C_Recv2(int8_t slave){
  uint8_t data1,data2;
  int retryCounter = 1;
  do{
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C3_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C3_MCS_R = (0
                         | I2C_MCS_ACK      // positive data ack
                       //  & ~I2C_MCS_STOP    // no stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data1 = (I2C3_MDR_R&0xFF);       // MSB data sent first
    I2C3_MCS_R = (0
                       //  & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                       //  & ~I2C_MCS_START   // no start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data2 = (I2C3_MDR_R&0xFF);       // LSB data sent last
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C3_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (data1<<8)+data2;                  // usually returns 0xFFFF on error
}

// sends one byte to specified slave
// Note for HMC6352 compass only:
// Used with 'S', 'W', 'O', 'C', 'E', 'L', and 'A' commands
//  For 'A' commands, I2C_Recv2() should also be called
// Note for TMP102 thermometer only:
// Used to change the pointer register
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send1(int8_t slave, uint8_t data1){
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
  I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
  I2C3_MSA_R &= ~0x01;             // MSA[0] is 0 for send
  I2C3_MDR_R = data1&0xFF;         // prepare first byte
  I2C3_MCS_R = (0
                    //   & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // generate stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
  return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
// sends two bytes to specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
//  For 'r' and 'g' commands, I2C_Recv() should also be called
// Note for TMP102 thermometer only:
// Used to change the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send2(int8_t slave, uint8_t data1, uint8_t data2){
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
  I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
  I2C3_MSA_R &= ~0x01;             // MSA[0] is 0 for send
  I2C3_MDR_R = data1&0xFF;         // prepare first byte
  I2C3_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                    //   & ~I2C_MCS_STOP    // no stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN    // master disable
                        );   
                                          // return error bits if nonzero
    return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C3_MDR_R = data2&0xFF;         // prepare second byte
  I2C3_MCS_R = (0
                      // & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // generate stop
                      // & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
  return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
// sends three bytes to specified slave
// Note for HMC6352 compass only:
// Used with 'w' and 'G' commands
// Note for TMP102 thermometer only:
// Used to change the contents of the pointer register
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send3(int8_t slave, uint8_t data1, uint8_t data2, uint8_t data3){
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
  I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
  I2C3_MSA_R &= ~0x01;             // MSA[0] is 0 for send
  I2C3_MDR_R = data1&0xFF;         // prepare first byte
  I2C3_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                       );   
                                          // return error bits if nonzero
    return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C3_MDR_R = data2&0xFF;         // prepare second byte
  I2C3_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                     //  & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                        );
                                          // return error bits if nonzero
    return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C3_MDR_R = data3&0xFF;         // prepare third byte
  I2C3_MCS_R = (0
                    //   & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // generate stop
                    //   & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
  return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}

uint16_t Proximity_Read(){
	uint16_t distance;
	distance = I2C_Recv2(0x29);
	return distance;
}
