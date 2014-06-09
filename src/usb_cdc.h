//==========================================================================
// USB CDC (C++ <-> Java interface)
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
#ifndef _INC_USB_CDC_H
#define _INC_USB_CDC_H

//==========================================================================
//==========================================================================
#include <QObject>
#if defined(Q_OS_ANDROID)
    #include <QtAndroidExtras/QAndroidJniObject>
#else
    #include <QtSerialPort/QSerialPort>
#endif

//==========================================================================
//==========================================================================
struct TCdcInfo
{
    QString name;
    int productId;
    int vendorId;
};
Q_DECLARE_METATYPE(TCdcInfo)

//==========================================================================
//==========================================================================
class CUsbCdc : public QObject
{
    Q_OBJECT

public:
    CUsbCdc (QObject *parent = 0);
    ~CUsbCdc (void);


    int init (void);
    int checkDevice (void);
    int openDevice (void);
    int checkConnection (void);
    void closeDevice (void);

    int write (unsigned char *aSrc, int aLen);
    int read (unsigned char *aDest, int aMaxLen);
    void flush (void);

    struct TCdcInfo info;
    int txMaxPacketSize;
    int rxMaxPacketSize;
    bool isOpen;

signals:
    void logMessage (const QString &s);

private:
#if defined(Q_OS_ANDROID)
    QAndroidJniObject activity;
#else
    QSerialPort serialPort;
#endif

};

//==========================================================================
//==========================================================================
#endif
