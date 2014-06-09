//==========================================================================
// Worker Thread
//==========================================================================
//  Copyright (c) 2013-Today DogRatIan.  All rights reserved.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//==========================================================================
// Naming conventions
// ~~~~~~~~~~~~~~~~~~
//                Class : Leading C
//               Struct : Leading T
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//==========================================================================
#ifndef _INC_WORKER_H
#define _INC_WORKER_H

//==========================================================================
//==========================================================================
#include <QThread>
#include <QElapsedTimer>

#include "usb_cdc.h"

//==========================================================================
// Defines
//==========================================================================
struct TSensorInfo
{
    char name[64];
    char version[16];
    float temperature;
    float humidity;
};
Q_DECLARE_METATYPE(TSensorInfo)

//==========================================================================
// Class define
//==========================================================================
class CWorker : public QThread
{
    Q_OBJECT

public:
    CWorker (QObject * aParent = 0, CUsbCdc *aDevice = NULL);
    ~CWorker ();

    void run() Q_DECL_OVERRIDE;

    void requestAbort (void);


signals:
    void logMessage (const QString &s);
    void disconnected (void);
    void updateSensor (struct TSensorInfo aInfo);

private:
    bool abort;

    CUsbCdc *device;
    QElapsedTimer timerProcess;
    struct TSensorInfo info;

    unsigned char txBuf[512];
    unsigned char rxBuf[512];

    void flushDevice (void);
    int sendCommand (int aTxLen);
    void processReading (void);
};

//==========================================================================
//==========================================================================
#endif

