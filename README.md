# MPU6050_Motion-Interrupt

Arduino and MPU6050 MotionDetect interrupt test

As I don't find any working sample code for MPU6050 to generate interrupts when moved, I dig into datasheets ind wrote my own code. I just want to share my project.
  
Tested with: Arduino Nano, gy-521 module

Wiring:

MPU6050 VCC       -> Arduino VCC

MPU6050 GND       -> Arduino GND

MPU6050 SDA       -> Arduino A4

MPU6050 SCL       -> Arduino A5

MPU6050 INT       -> Arduino D3
 
 
 Info is got from 
 
 MPU-6000/MPU-6050 Register Map and Descriptions
 Document Number: RM-MPU-6000A-00
 Revision: 3.2
 Release Date: 11/14/2011
 
 And
 
 MPU-6000/MPU-6050 Product Specification
 Document Number: PS-MPU-6000A-00
 Revision: 3.3
 Release Date: 5/16/2012
 
 Section 8.1 of Specification: Motion Interrupt
 
 Newer releases of datasheet and register map got removed 
 any motion interrupt-related sections for some reason,
 so I used older versions of documents to write this code.
