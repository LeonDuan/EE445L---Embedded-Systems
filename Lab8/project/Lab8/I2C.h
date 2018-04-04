// I2C0.h
// Runs on LM4F120/TM4C123
// Provide a function that initializes, sends, and receives the I2C0 module
// interfaced with an HMC6352 compass or TMP102 thermometer.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Section 8.6.4 Programs 8.5, 8.6 and 8.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// I2C0SCL connected to PB2 and to pin 4 of HMC6352 compass or pin 3 of TMP102 thermometer
// I2C0SDA connected to PB3 and to pin 3 of HMC6352 compass or pin 2 of TMP102 thermometer
// SCL and SDA lines pulled to +3.3 V with 10 k resistors (part of breakout module)
// ADD0 pin of TMP102 thermometer connected to GND
void I2C_Init(void);

// receives one byte from specified slave
uint8_t I2C_Recv(int8_t slave);

// receives two bytes from specified slave
uint16_t I2C_Recv2(int8_t slave);

// recieves 6 bytes and stores them in an array
void I2C_Recv6(int8_t slave, unsigned char *Buff);

// sends one byte to specified slave
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send1(int8_t slave, uint8_t data1);

// sends two bytes to specified slave
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send2(int8_t slave, uint8_t data1, uint8_t data2);

// sends three bytes to specified slave
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send3(int8_t slave, uint8_t data1, uint8_t data2, uint8_t data3);

// sends six bytes to specified slave
// Returns 0 if successful, nonzero if error
uint32_t I2C_Send6(int8_t slave, unsigned char *Buff);

// sends a list of register/value pairs
// first value in list is count of register/value pairs in the list
void I2C_SendList(unsigned char *List);
