//==========================================================================
// Top Controller
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
#ifndef _INC_CONTROL_TOP_H
#define _INC_CONTROL_TOP_H

//==========================================================================
//==========================================================================
#include <QObject>

#include "usb_cdc.h"
#include "worker.h"

//==========================================================================
//==========================================================================
class CControlTop : public QObject
{
    Q_OBJECT


public:
    explicit CControlTop (QObject *aParent = 0);
    ~CControlTop (void);

    Q_INVOKABLE void init (void);
    Q_INVOKABLE void exit (void);
    Q_INVOKABLE int check (void);
    Q_INVOKABLE int open (void);
    Q_INVOKABLE void close (void);
    Q_INVOKABLE void test (int aType);

    // Properties for QML
    Q_PROPERTY(int isOpen READ readIsOpen)
    int readIsOpen (void);

    Q_PROPERTY(float temperature READ readTemperature)
    float readTemperature (void);

    Q_PROPERTY(float humidity READ readHumidity)
    float readHumidity (void);

    Q_PROPERTY(QString sensorName READ readSensorName)
    QString readSensorName (void);

    Q_PROPERTY(QString sensorVersion READ readSensorVersion)
    QString readSensorVersion (void);

    Q_PROPERTY(QString deviceName READ readDeviceName)
    QString readDeviceName (void);

    Q_PROPERTY(int devicePid READ readDevicePid)
    int readDevicePid (void);

    Q_PROPERTY(int deviceVid READ readDeviceVid)
    int readDeviceVid (void);


signals:
    void logMessage (QString aStr);
    void deviceDisconnect (void);
//    void updateDeviceName (QString aStr);
//    void updatePID (QString aStr);
//    void updateVID (QString aStr);
    void deviceConnect (void);
    void deviceInfoChanged (void);
    void sensorChanged (void);
//    void updateTemperature (QString aStr);
//    void updateHumidity (QString aStr);

public slots:
    void handleUpdateSensor (struct TSensorInfo aInfo);

private:
    CUsbCdc usbCdc;
    CWorker worker;
    struct TSensorInfo sensorInfo;
};

//==========================================================================
//==========================================================================
#endif
