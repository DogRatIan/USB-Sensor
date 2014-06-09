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
#include <QDebug>
#include <math.h>

#include "control_top.h"


//==========================================================================
// Constants
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CControlTop::CControlTop (QObject *aParent) :
    QObject(aParent),
    worker (aParent, &usbCdc)
{
}

//==========================================================================
// Destroyer
//==========================================================================
CControlTop::~CControlTop (void)
{
    exit ();
}

//==========================================================================
// Initialization
//==========================================================================
void CControlTop::init (void)
{
    //
    emit logMessage ("CControlTop is ready.");
    memset (&sensorInfo, 0, sizeof (sensorInfo));

    // Connections
    connect (&usbCdc, &CUsbCdc::logMessage, this, &CControlTop::logMessage);
    connect (&worker, &CWorker::logMessage, this, &CControlTop::logMessage);
    connect (&worker, &CWorker::disconnected, this, &CControlTop::deviceDisconnect);
    connect (&worker, &CWorker::updateSensor, this, &CControlTop::handleUpdateSensor);

    //
    if (usbCdc.init() < 0)
        emit logMessage ("CDC Init fail.");
    else
        emit logMessage ("CDC Init done.");
    emit deviceInfoChanged ();

    // Start thread
    worker.start();

}

//==========================================================================
// exit
//==========================================================================
void CControlTop::exit (void)
{
    // Stop the Thread
    worker.requestAbort ();
    worker.wait ();

    // Remove connections
    disconnect (&usbCdc, &CUsbCdc::logMessage, this, &CControlTop::logMessage);
    disconnect (&worker, &CWorker::logMessage, this, &CControlTop::logMessage);
    disconnect (&worker, &CWorker::disconnected, this, &CControlTop::deviceDisconnect);
    disconnect (&worker, &CWorker::updateSensor, this, &CControlTop::handleUpdateSensor);
}

//==========================================================================
// Check Device
//==========================================================================
int CControlTop::check (void)
{
    QString str;
    int ret;

    ret = usbCdc.checkDevice();
    if (ret < 0)
    {
        emit logMessage ("Check device fail.");
        return -1;
    }
    else if (ret > 0)
    {
        emit logMessage ("Device found.");
        emit deviceInfoChanged ();
//        if (usbCdc.info.name.length() > 0)
//            emit updateDeviceName(usbCdc.info.name);
//        else
//            emit updateDeviceName ("-");
//        str.sprintf("0x%04X", usbCdc.info.productId);
//        emit updatePID(str);
//        str.sprintf("0x%04X", usbCdc.info.vendorId);
//        emit updateVID(str);
        return 0;
    }
    else
    {
        emit logMessage ("Device not found.");
        emit deviceInfoChanged ();
//        emit updateDeviceName ("-");
//        emit updatePID ("-");
//        emit updateVID ("-");
        return -1;
    }
}

//==========================================================================
// Open Device
//==========================================================================
int CControlTop::open (void)
{
    if (usbCdc.openDevice() < 0)
    {
        emit logMessage ("Open device fail.");
        return -1;
    }
    else
        return 0;
}

//==========================================================================
// Close Device
//==========================================================================
void CControlTop::close (void)
{
    usbCdc.closeDevice();
    emit logMessage ("Device closed.");
}

//==========================================================================
// Test
//==========================================================================
void CControlTop::test (int aType)
{
    int len;
    unsigned char buf[16];
    QString str;

    if (!usbCdc.isOpen)
    {
        emit logMessage("Device not open.");
        return;
    }

    if (usbCdc.checkConnection() < 0)
    {
        emit deviceDisconnect();
        emit deviceInfoChanged();
//        emit updateDeviceName ("-");
//        emit updatePID ("-");
//        emit updateVID ("-");
    }

    switch (aType)
    {
        case 1:
            if (usbCdc.write ((unsigned char*)"GI\n", 3) < 0)
                emit logMessage("TX fail");
            else
                emit logMessage("TX done");
            break;

        case 2:
            memset (buf, 0, sizeof (buf));
            len = usbCdc.read (buf, sizeof (buf) - 1);
            str.sprintf ("RX %d bytes", len);
            emit logMessage (str);

            for (int i = 0; i < len; i ++)
                if (buf[i] < 0x20)
                    buf[i] = 0x20;
            str.sprintf("%s", buf);
            if (str.length() > 0)
                emit logMessage (str);
            break;
    }
}

//==========================================================================
// Handle updateSensor
//==========================================================================
void CControlTop::handleUpdateSensor (struct TSensorInfo aInfo)
{
    bool changed;

    qDebug () << "name=" << aInfo.name;
    qDebug () << "version=" << aInfo.version;


    changed = false;
    if (memcmp (&sensorInfo, &aInfo, sizeof (sensorInfo)))
    {
        changed = true;
        memcpy (&sensorInfo, &aInfo, sizeof (sensorInfo));
    }

    if (changed)
        emit sensorChanged();


    if ((qstrnlen(aInfo.name, sizeof (aInfo.name)) > 0) && (qstrnlen (aInfo.version, sizeof (aInfo.version)) > 0))
    {
        emit deviceConnect ();
    }
    else
    {
        emit deviceDisconnect();
    }

//    if (isnan (aInfo.temperature))
//    {
//        if (showCelsius)
//            emit updateTemperature ("- C");
//        else
//            emit updateTemperature ("- F");
//    }
//    else
//    {
//        if (showCelsius)
//            str.sprintf("%.2f C", aInfo.temperature);
//        else
//            str.sprintf("%.2f F", aInfo.temperature * 9 / 5 + 32);
//        emit updateTemperature (str);
//    }

//    if (isnan (aInfo.humidity))
//        emit updateHumidity ("- %");
//    else
//    {
//        str.sprintf("%.2f %%", aInfo.humidity);
//        emit updateHumidity (str);
//    }

}

//==========================================================================
// Properties - isOpen
//==========================================================================
int CControlTop::readIsOpen (void)
{
    return usbCdc.isOpen;
}

//==========================================================================
// Properties - Sensor info
//==========================================================================
float CControlTop::readTemperature (void)
{
    return sensorInfo.temperature;
}

float CControlTop::readHumidity (void)
{
    return sensorInfo.humidity;
}

QString CControlTop::readSensorName (void)
{
    return sensorInfo.name;
}

QString CControlTop::readSensorVersion (void)
{
    return sensorInfo.version;
}

//==========================================================================
// Properties - Device info
//==========================================================================
QString CControlTop::readDeviceName (void)
{
    return usbCdc.info.name;
}

int CControlTop::readDevicePid (void)
{
    return usbCdc.info.productId;
}

int CControlTop::readDeviceVid (void)
{
    return usbCdc.info.vendorId;
}
