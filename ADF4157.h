/**********************************************************************
**  Device: ADF4157                                      **
**  File  : ADF4157.h                                                **
**                                                                   **
**  Created by Dawid Szymanski SQ6EMM 08/2023                        **
**  Updated by Dawid Szymanski SQ6EMM 02/2024 - registry calculator  **
**  Based on ADF4351 by:                                             **
**            JA Soler EA3HMJ 05/2014, http://qlfecv.wordpress.com   **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF4351     **
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

#include <SPI.h>

#ifndef __ADF4157_H__
#define __ADF4157_H__

class ADF4157
{
    public:
        ADF4157(byte LE);
        void Initialize(double RFout);
        void SetFrequency(double RFout); // Freq 0-4
    private:
        struct regs {
          uint32_t array[5];
        };
        ADF4157::regs Prepare(double RFout);
        void WriteRegister(const uint32_t value);
        byte pinLE; // The Aurdino pin where the SPI LE to the device is controlled
};

#endif
