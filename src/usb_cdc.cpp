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
#include "usb_cdc.h"

#include <QDebug>

#if defined(Q_OS_ANDROID)
    #include <QAndroidJniEnvironment>
#else
    #include <QtSerialPort/QSerialPortInfo>
#endif

//==========================================================================
// Constants
//==========================================================================
#if defined(Q_OS_ANDROID)
static const char *KJavaCdcName = "com/dogratian/JavaCdc";
#endif

//==========================================================================
// Java byte[] <-> C unsigned char[]
//==========================================================================
#if defined(Q_OS_ANDROID)
static jbyteArray CreateJbyteFromU8 (unsigned char *aSrc, int aLen)
{
    QAndroidJniEnvironment env;

    jbyteArray array = env->NewByteArray (aLen);
    env->SetByteArrayRegion (array, 0, aLen, reinterpret_cast<jbyte*>(aSrc));
    return array;
}
static int CopyJbyteToU8 (jbyteArray aArray, unsigned char *aDest, int aMaxLen)
{
    QAndroidJniEnvironment env;

    int len = env->GetArrayLength (aArray);

    memset (aDest, 0, aMaxLen);
    if (len > aMaxLen)
        len = aMaxLen;

    env->GetByteArrayRegion (aArray, 0, len, reinterpret_cast<jbyte*>(aDest));
    return len;
}
#endif

//==========================================================================
// Constructor
//==========================================================================
CUsbCdc::CUsbCdc(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TCdcInfo>();

    info.name.clear ();
    info.productId = 0;
    info.vendorId = 0;
    isOpen = false;
    txMaxPacketSize = 0;
    rxMaxPacketSize = 0;
}

//==========================================================================
// Destroyer
//==========================================================================
CUsbCdc::~CUsbCdc (void)
{
#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return;

    //
    QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "releaseAll", "()V");
#else
    serialPort.close();
#endif
}

//==========================================================================
// Initialization
//==========================================================================
int CUsbCdc::init (void)
{
    int ret;
    QString str;

    //
    isOpen = false;
    info.name.clear ();
    info.productId = 0;
    info.vendorId = 0;
    txMaxPacketSize = 0;
    rxMaxPacketSize = 0;

    //
#if defined(Q_OS_ANDROID)
    activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");

    if (activity.isValid() == false)
    {
        qDebug () << __func__ << ": Activity is not valid.";

        emit logMessage ("Activity is not valid.");
        return -1;
    }

    // Init CDC
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName,
                                               "initCdc",
                                               "(Landroid/content/Context;)I",
                                               activity.object<jobject>());
#else
    serialPort.close();
    ret = 0;
#endif
    str.sprintf ("initCdc() ret=%d", ret);
    qDebug () << str;

    //
    return ret;
}

//==========================================================================
// Check Device (Return Device Count)
//==========================================================================
int CUsbCdc::checkDevice (void)
{
    int ret;
    QString str;

#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return -1;

    //
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "checkDevice", "()I");

    if (ret < 0)
    {
        isOpen = false;
        info.name.clear ();
        info.productId = 0;
        info.vendorId = 0;
    }
    else
    {
        QAndroidJniObject java_ret;

        java_ret = QAndroidJniObject::callStaticObjectMethod (KJavaCdcName, "getDeviceName", "()Ljava/lang/String;");
        info.name = java_ret.toString();
        info.productId = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "getPid", "()I");
        info.vendorId = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "getVid", "()I");
    }

#else
    // Add serial port name to Combobox
    ret = 0;
    info.name = "";
    info.productId = 0;
    info.vendorId = 0;
    foreach (const QSerialPortInfo &port_info, QSerialPortInfo::availablePorts())
    {
        if (!port_info.description().contains("ASF"))
            continue;
        if (!port_info.manufacturer().contains("ATMEL"))
            continue;

        ret = 1;
        info.name = port_info.portName();
        break;

//        if (info.manufacturer().left(1).compare("(") == 0)
//            str = QString ("%1: %2").arg(info.portName()).arg(info.description());
//        else
//            str = QString ("%1: %2 (%3)").arg(info.portName()).arg(info.description()).arg(info.manufacturer());

//        emit addSerialPort(str);
    }
#endif

    str.sprintf ("checkDevice() ret=%d", ret);
    qDebug () << str;

    return ret;
}

//==========================================================================
// Open Device
//==========================================================================
int CUsbCdc::openDevice (void)
{
    int ret;
    int tx_size;
    int rx_size;
    QString str;

    //

#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return -1;

    //
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "openDevice", "()I");
    tx_size = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "getTxMaxPacketSize", "()I");
    rx_size = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "getRxMaxPacketSize", "()I");

#else
    if (info.name.length() <= 0)
        return -1;

    serialPort.setPortName (info.name);
    if (!serialPort.open (QIODevice::ReadWrite))
    {
        str = QString ("%1 ERROR: %1").arg(info.name).arg(serialPort.errorString ());
        emit logMessage (str);
        return -1;
    }
    serialPort.setBaudRate (QSerialPort::Baud115200);
    serialPort.setDataBits (QSerialPort::Data8);
    serialPort.setParity (QSerialPort::NoParity);
    serialPort.setStopBits (QSerialPort::OneStop);
    serialPort.setFlowControl (QSerialPort::NoFlowControl);
    serialPort.setDataTerminalReady(true);
    ret = 0;
    tx_size = 4;
    rx_size = 4;
#endif

    str.sprintf ("openDevice() ret=%d,(%d,%d)", ret, tx_size, rx_size);
    qDebug () << str;
    emit logMessage (str);

    if ((ret < 0) || (tx_size < 0) || (rx_size < 0))
        return -1;

    // Success open
    isOpen = true;
    txMaxPacketSize = tx_size;
    rxMaxPacketSize = rx_size;

    return 0;
}

//==========================================================================
// Check Connection
//==========================================================================
int CUsbCdc::checkConnection (void)
{
#if defined(Q_OS_ANDROID)
    int ret;

    if (activity.isValid () == false)
        return -1;

    //
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "checkConnection", "()I");
    if (ret < 0)
    {
        qDebug () << "checkConnection fail, close device.";
        if (isOpen)
            closeDevice ();
        return -1;
    }
#else
    if (serialPort.error() != QSerialPort::NoError)
        serialPort.close ();
    if (!serialPort.isOpen())
        return -1;
#endif

    return 0;
}

//==========================================================================
// Close Device
//==========================================================================
void CUsbCdc::closeDevice (void)
{
    //

#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return;

    //
    QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "closeDevice", "()V");
#else
    serialPort.close();
#endif
    qDebug () << __PRETTY_FUNCTION__;
    isOpen = false;
    txMaxPacketSize = 0;
    rxMaxPacketSize = 0;
}

//==========================================================================
// Write data to device
//==========================================================================
int CUsbCdc::write (unsigned char *aSrc, int aLen)
{
    int ret;

    //
    if (!isOpen)
    {
        qDebug () << "write() failed, device not open.";
        return -1;
    }

#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return -1;

    //
    QAndroidJniEnvironment env;

    jbyteArray j_buf = CreateJbyteFromU8 (aSrc, aLen);
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "write", "([BI)I", j_buf, aLen);
    env->DeleteLocalRef (j_buf);

#else
    ret = serialPort.write((char *)aSrc, aLen);
#endif

    if (ret)
    {
        QString str;
        str.sprintf ("write() ret=%d", ret);
        qDebug () << str;
    }

    return ret;
}


//==========================================================================
// Read data from device
//==========================================================================
int CUsbCdc::read (unsigned char *aDest, int aMaxLen)
{
    int ret;

    //
    if (!isOpen)
        return -1;

#if defined(Q_OS_ANDROID)
    if (activity.isValid () == false)
        return -1;

    //
    QAndroidJniEnvironment env;

    jbyteArray j_buf = env->NewByteArray (aMaxLen);
    ret = QAndroidJniObject::callStaticMethod<jint> (KJavaCdcName, "read", "([BI)I", j_buf, aMaxLen);
    CopyJbyteToU8 (j_buf, aDest, aMaxLen);
    env->DeleteLocalRef (j_buf);


#else
    serialPort.waitForReadyRead (0);
    serialPort.clearError();
    ret = serialPort.read ((char *)aDest, aMaxLen);
#endif

    if (ret)
    {
        QString str;
        str.sprintf ("read() ret=%d", ret);
        qDebug () << str;
    }

    return ret;
}

//==========================================================================
// Flush
//==========================================================================
void CUsbCdc::flush (void)
{
#if defined(Q_OS_ANDROID)
#else
    serialPort.clear ();
#endif
}
