/**********************************************************************
**  Device: ADF4157                                                  **
**  File  : ADF4157.cpp                                              **
**                                                                   **
**  Created by Dawid Szymanski SQ6EMM 08/2023                        **
**  Updated by Dawid Szymanski SQ6EMM 02/2024 - registry calculator  **
**  Based on ADF4351 by:                                             **
**            JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com   **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF4157     **
**  module use on Arduino with SPI.                                  **
**                                                                   **
**  This code is free software; you can redistribute it and/or       **
**  modify it under the terms of the GNU Lesser General Public       **
**  License as published by the Free Software Foundation; either     **
**  version 2.1 of the License, or (at your option)                  **
**  any later version.                                               **
**                                                                   **
**  Copyright (C) 2014 JA Soler EA3HMJ.                              **
**                                                                   **
**********************************************************************/

// Arduino (Uno 11) (ESP32 19) MOSI to DATA
// Arduino (Uno 13) (ESP32 5) SCK to CLK
// Arduino (Uno 3) (ESP32 32) Select to LE

#include <Arduino.h>
#include "ADF4157.h"

// Public

ADF4157::ADF4157(byte LE)
{
    pinLE = LE;
}


void ADF4157::Initialize(double RFout)
{
    ADF4157::regs regarr = ADF4157::Prepare(RFout);
    SPI.begin();                            // Setup SPI bus
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);             // CPHA = 0 and Clock Polarity (CPOL) positive

    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);

    for (int8_t i = 4; i >= 0; i--)  {      // Load all regs for nominal frequency. Later on only relevant register(s) should be loaded
        WriteRegister(regarr.array[i]);
    }
}

void ADF4157::SetFrequency(double RFout)
{
    ADF4157::regs regarr = ADF4157::Prepare(RFout);
    for (int8_t i = 1; i >= 0; i--)
    {
        WriteRegister(regarr.array[i]);
    }
}

// Private
void ADF4157::WriteRegister(const uint32_t value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    digitalWrite(pinLE, LOW);
}

ADF4157::regs ADF4157::Prepare(double RFout)
{
  // values hardset
  // NOTE: these must stay uint32_t (not int/unsigned int/bool) - on AVR targets
  // (e.g. Uno) int is only 16 bits, which silently truncates values like REFin
  // and overflows the register shifts below (verified fine on ESP32's 32-bit int).
  const uint32_t ClckDivVal = 0; // 0 - 4095
  const uint32_t ClckDivMode = 0; // 0 (*Off) or 1 (On) - Enables switched R fastlock
  const uint32_t NegBleedCur = 0; // 0 (*Off) or 3 (Switched R FastLock Enabled) - Negative Bleed Current
  const uint32_t CounterRst = 0; // 0 (*Disabled) or 1 (Enabled)
  const uint32_t CP3St = 0; // 0 (*Disabled) or 1 (Enabled) - Charge Pump Three-State
  const uint32_t SDRst = 0; // 0 (*Enabled) or 1 (Disabled)
  const uint32_t LDP = 0; // 0 (*24 PFD Cycles) or 1 (40 PFD Cycles) - Lock Detect Precision
  const uint32_t PDPol = 1; // 0 (Negative) or 1 (*Positive) - Phase Detector Polarity
  const uint32_t CSR = 0; // 0 (*Disabled) or 1 (Enabled) - Cycle Slip Reduction
  const uint32_t Muxout = 6;
  const uint32_t ChargePump = 13; // set it to 7mA for DF9NP design
  uint32_t Prescaler = 0; if ( RFout > 3000000000 ) Prescaler = 1; // either 4/5 (0) or 8/9 (1)
  const uint32_t RCounter = 1;
  const uint32_t RefDivBy2 = 0;
  const uint32_t PD = 0;
  const uint32_t REFin = 10000000UL; // usual 10MHz GPSDO, OCXO input
  const uint32_t RefDouble = 1; // double the REFin in order to improve phase noise

  double fPFD = REFin*((1.0 + RefDouble) / (RCounter * (1.0 + RefDivBy2)));

  double N = RFout/fPFD; // integer division factor
  uint32_t INT = (uint32_t)N; // must be between 23 and 4095
  double fxSB = ((RFout/fPFD) - INT) * pow(2,12);
  uint32_t fMSB = (uint32_t)fxSB;
  uint32_t fLSB = (uint32_t)round(((fxSB - fMSB) * pow(2,13)));

  regs aregs;

  aregs.array[0] = (0UL<<31)|(Muxout<<27)|(INT<<15)|(fMSB<<3)|(0UL<<0); // Done
  aregs.array[1] = (0UL<<31)|(0UL<<30)|(0UL<<29)|(0UL<<28)|(fLSB<<15)|(1UL<<0); // Done
  aregs.array[2] = (0UL<<31)|(CSR<<28)|(ChargePump<<24)|(Prescaler<<22)|(RefDivBy2<<21)|(RefDouble<<20)|(RCounter<<15)|(2UL<<0); // Done
  aregs.array[3] = (0UL<<31)|(SDRst<<14)|(LDP<<7)|(PDPol<<6)|(PD<<5)|(CP3St<<4)|(CounterRst<<3)|(3UL<<0); // Done
  aregs.array[4] = (0UL<<31)|(NegBleedCur<<23)|(ClckDivMode<<19)|(ClckDivVal<<7)|(4UL<<0); // Done

  return aregs;
}
