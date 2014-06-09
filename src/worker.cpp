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
#include <QDebug>
#include <QApplication>
#include <math.h>
#include "worker.h"

//==========================================================================
// Defines
//==========================================================================
#define INTERVAL_CHECK_DEVICE       2000
#define INTERVAL_UPDATE             5000

//==========================================================================
// Variables
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CWorker::CWorker (QObject * aParent, CUsbCdc *aDevice) :
    QThread (aParent),
    abort (false),
    device (aDevice)
{
    qRegisterMetaType<TSensorInfo>();
}

//==========================================================================
// Destroyer
//==========================================================================
CWorker::~CWorker ()
{
}

//==========================================================================
// Main operation
//==========================================================================
void CWorker::run()
{
    bool deep_sleep = false;

    abort = false;
    timerProcess.restart ();

    emit logMessage ("CWorker start");
    qDebug () << "CWorker start";

    try
    {
        if (device == NULL)
            throw "Device not define.";

        while (abort == false)
        {
            // Check connection
            if (device->isOpen)
            {
                if (device->checkConnection() < 0)
                {
                    if (device->init () < 0)
                        throw "Device init fail.";
                    emit disconnected ();
                    qDebug () << "Disconnected";
                }
            }

            // Suspend handling
            if (QApplication::applicationState() == Qt::ApplicationSuspended)
            {
                // Get into deep sleep when suspended
                if (deep_sleep == false)
                {
                    deep_sleep = true;
                    emit logMessage ("CWorker deep sleep");
                    qDebug () << "CWorker deep sleep";
                }
                msleep (2000);
                continue;
            }

            //
            if (deep_sleep)
            {
                // Restore from dep sleep
                deep_sleep = false;
                emit logMessage ("CWorker wake up");
                qDebug () << "CWorker wake up";
            }

            //

            //
            msleep (100);
            flushDevice ();
            processReading ();


//            ftdi->write((unsigned char *)"ABC", 3);

//            unsigned char buf[16];
//            int rx_len;
//            rx_len = ftdi->read (buf, sizeof (buf));
//            if (rx_len > 0)
//            {
//                qDebug () << "RXed " << rx_len;
//            }
        }
    }
    catch (const char *aMsg)
    {
        emit logMessage (QString (aMsg));
    }
    catch (QString aMsg)
    {
        emit logMessage (aMsg);
    }

    //
    emit logMessage ("CWorker stopped");
    qDebug () << "CWorker stopped";
}

//==========================================================================
// Request Abort
//==========================================================================
void CWorker::requestAbort (void)
{
    abort = true;
}


//==========================================================================
//==========================================================================
// Public members
// ^^^^^^^^^^^^^^^
//==========================================================================
//==========================================================================
// Private members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================

//==========================================================================
// Flush device
//==========================================================================
void CWorker::flushDevice (void)
{
    // Check Device
    if (device == NULL)
        return;
    if (!device->isOpen)
        return;

    device->flush ();
}

//==========================================================================
// Send Command
//==========================================================================
int CWorker::sendCommand (int aTxLen)
{
    QElapsedTimer timer;
    int tx_len;
    int rxing_len;
    int len;
    int i;
    unsigned char *ptr;
    unsigned char buf[64];
    int retry;

    // Check Device
    if (device == NULL)
        return -1;
    if (!device->isOpen)
        return -1;

    retry = 3;

    do
    {
        // Send Command
        ptr = txBuf;
        tx_len = aTxLen;
        while (tx_len)
        {
            if (tx_len > device->txMaxPacketSize)
                len = device->txMaxPacketSize;
            else
                len = tx_len;

            if (device->write(ptr, len) != len)
            {
                emit logMessage("write error.");
                return -1;
            }
            msleep(1);

            tx_len -= len;
            ptr += len;
        }

        // Wait Response
        timer.restart();
        ptr = rxBuf;
        rxing_len = 0;
        while (timer.elapsed() < 250)
        {
            msleep(2);
            len = device->read (buf, sizeof (buf));

            if (len  < 0)
            {
                emit logMessage("read error.");
                return -1;
            }
            if (len == 0)
            {
                msleep (50);
                continue;
            }

            for (i = 0; i < len; i ++)
            {
                if (rxing_len < (int)sizeof (rxBuf))
                {
                    if (buf[i] >= ' ')
                    {
                        *ptr = buf[i];
                        ptr ++;
                        rxing_len ++;
                    }
                    if (((buf[i] == '\n') || (buf[i] == '\r')) && (rxing_len > 0))
                        return rxing_len;
                }
            }
        }

        //
        if (rxing_len)
            break;

        retry --;
    } while (retry);


    return rxing_len;
}

//==========================================================================
// Process Reading
//==========================================================================
void CWorker::processReading (void)
{
    int tx_len;
    int rx_len;

    // Check Device
    if (device == NULL)
        return;
    if (!device->isOpen)
        return;

    //
    if (timerProcess.elapsed() < INTERVAL_UPDATE)
        return;

    timerProcess.restart();

    //
    memset (&info, 0, sizeof (info));
    info.temperature = NAN;
    info.humidity = NAN;
    do
    {
        // Get ID
        tx_len = 3;
        memcpy (txBuf, "GI\n", tx_len);
        rx_len = sendCommand (tx_len);
        if (rx_len <= 0)
            break;
        rxBuf[rx_len] = 0;
        qstrncpy (info.name, (char *)rxBuf, sizeof (info.name));

        // Get Version
        tx_len = 3;
        memcpy (txBuf, "GV\n", tx_len);
        rx_len = sendCommand (tx_len);
        if (rx_len <= 0)
            break;
        rxBuf[rx_len] = 0;
        qstrncpy (info.version, (char *)rxBuf, sizeof (info.version));

        // Get Temperature
        tx_len = 3;
        memcpy (txBuf, "GT\n", tx_len);
        rx_len = sendCommand (tx_len);
        if (rx_len <= 0)
            break;
        rxBuf[rx_len] = 0;
        info.temperature = atof ((char *)rxBuf);

        // Get Humidity
        tx_len = 3;
        memcpy (txBuf, "GH\n", tx_len);
        rx_len = sendCommand (tx_len);
        if (rx_len <= 0)
            break;
        rxBuf[rx_len] = 0;
        info.humidity = atof ((char *)rxBuf);

    } while (0);

    emit updateSensor(info);
}
