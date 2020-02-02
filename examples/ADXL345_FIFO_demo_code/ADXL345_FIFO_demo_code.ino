/*****************************************************************************/
//  Function:    Uses ADXL345 in FIFO mode. Up to 33 samples of X/Y/Z
//               acceleration are stored in the FIFO of the sensor and are than
//               read back within a bulk read. FIO mode is essential for low
//               power applications where only the sensor stays awake.
//  Hardware:    3-Axis Digital Accelerometer(+-16g)
//  Arduino IDE: Arduino-1.8.9
//  Author:  Hans "Blackcow"
//  Date:    SEP 13,2019
//  Version: v1.0
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/*******************************************************************************/

/*****************************************************************************/
//  Function:    Get the accelemeter of X/Y/Z axis and print out on the
//          serial monitor.
//  Hardware:    3-Axis Digital Accelerometer(��16g)
//  Arduino IDE: Arduino-1.0
//  Author:  Frankie.Chu
//  Date:    Jan 11,2013
//  Version: v1.0
//  by www.seeedstudio.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/*******************************************************************************/

#include <Wire.h>
#include <ADXL345.h>


ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

void setup() {
    Serial.begin(115200);
    adxl.powerOn();

    //set activity/ inactivity thresholds (0-255)
    adxl.setActivityThreshold(75); //62.5mg per increment
    adxl.setInactivityThreshold(75); //62.5mg per increment
    adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

    //look of activity movement on this axes - 1 == on; 0 == off
    adxl.setActivityX(1);
    adxl.setActivityY(1);
    adxl.setActivityZ(1);

    //look of inactivity movement on this axes - 1 == on; 0 == off
    adxl.setInactivityX(1);
    adxl.setInactivityY(1);
    adxl.setInactivityZ(1);

    //look of tap movement on this axes - 1 == on; 0 == off
    adxl.setTapDetectionOnX(0);
    adxl.setTapDetectionOnY(0);
    adxl.setTapDetectionOnZ(1);

    //set values for what is a tap, and what is a double tap (0-255)
    adxl.setTapThreshold(50); //62.5mg per increment
    adxl.setTapDuration(15); //625us per increment
    adxl.setDoubleTapLatency(80); //1.25ms per increment
    adxl.setDoubleTapWindow(200); //1.25ms per increment

    //set values for what is considered freefall (0-255)
    adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment

    //setting all interrupts to take place on int pin 1
    //I had issues with int pin 2, was unable to reset it
    adxl.setInterruptMapping(ADXL345_INT_DATA_READY_BIT,   ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN);
    adxl.setInterruptMapping(ADXL345_INT_WATERMARK_BIT,     ADXL345_INT1_PIN);

    //register interrupt actions - 1 == on; 0 == off
    adxl.setInterrupt(ADXL345_INT_DATA_READY_BIT, 1);
    adxl.setInterrupt(ADXL345_INT_SINGLE_TAP_BIT, 0);
    adxl.setInterrupt(ADXL345_INT_DOUBLE_TAP_BIT, 0);
    adxl.setInterrupt(ADXL345_INT_FREE_FALL_BIT,  0);
    adxl.setInterrupt(ADXL345_INT_ACTIVITY_BIT,   0);
    adxl.setInterrupt(ADXL345_INT_INACTIVITY_BIT, 0);
    adxl.setInterrupt(ADXL345_INT_WATERMARK_BIT, 1);

    //setting lowest sampling rate
    adxl.setRate(6.25);
    //setting device into FIFO mode
    adxl.setMode(ADXL345_MODE_FIFO);
    //set watermark for Watermark interrupt
    Serial.println("Current operation mode: ");
    Serial.println(adxl.getMode());
    adxl.setWatermark(30);
    delay(100);
}

void loop() {
    //Boring accelerometer stuff
    int x[32], y[32], z[32];
    byte fifoentries, intEvent;


    fifoentries = adxl.getFifoEntries();
    if ((fifoentries % 5) == 0) {       //Printing only every 5th sample to prevent spam on console
        Serial.print("Current FIFO entries: ");
        Serial.println(fifoentries);
    }


    intEvent = adxl.getInterruptSource();  // reading interrupt status flags
    if (adxl.triggered(intEvent, ADXL345_WATERMARK)) { // if watermark interrupt occured
        Serial.println("Watermark interrupt triggered. Fetching data now.");

        if (fifoentries != 0) {
            adxl.burstReadXYZ(&x[0], &y[0], &z[0], fifoentries); // reading all samples of FIFO
            for (int i = 0; i < fifoentries; i = i + 5) { //Printing only every 5th sample to prevent spam on console
                Serial.print("FIFO data sample: ");
                Serial.print(i);
                Serial.print(", x: ");
                Serial.print(x[i]);
                Serial.print(", y: ");
                Serial.print(y[i]);
                Serial.print(", z: ");
                Serial.println(z[i]);
            }

        }
    }
    delay(200);
}
