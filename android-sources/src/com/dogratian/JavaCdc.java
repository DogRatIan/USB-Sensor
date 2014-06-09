package com.dogratian;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.content.Context;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbConstants;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Arrays;
import java.io.UnsupportedEncodingException;
import android.util.Log;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.app.PendingIntent;

public class JavaCdc extends QtActivity
{
    // Singleton Object
    private static JavaCdc m_instance;

    private static final String TAG = "JavaCdc";

    private static UsbManager manager = null;
    private static Context m_parentContext = null;
    private static UsbDevice deviceCdc = null;
    private static UsbDeviceConnection connection = null;
    private static UsbInterface intf_data = null;
    private static UsbEndpoint ep_input, ep_output;

    private static boolean registered = false;

    private static final String ACTION_USB_PERMISSION = "cc.dogratian.JavaCdc.action.USB_PERMISSION";

    private static PowerManager powerManager = null;
    private static WakeLock wakeLock = null;

    public JavaCdc ()
    {
        m_instance = this;
    }

    // Release
    public static void releaseAll ()
    {
        if (m_parentContext != null)
        {
            Log.d (TAG, "releaseAll ()");
            m_parentContext.unregisterReceiver(mUsbReceiver);
            registered = false;
        }

        if (connection != null)
        {
            synchronized (connection)
            {
                if (intf_data != null)
                    connection.releaseInterface (intf_data);
                connection.close ();
            }
            connection = null;
            intf_data = null;
            ep_input = null;
            ep_output = null;
        }
    }

    // Initialization
    public static int initCdc (Context parentContext)
    {
        Log.d (TAG, "initCdc (), " + m_instance);

        m_parentContext = parentContext;

        if (manager == null)
        {
            Log.d (TAG, "getSystemService()");
            manager = (UsbManager) m_parentContext.getSystemService(Context.USB_SERVICE);
        }

        if (powerManager == null)
        {
            powerManager = (PowerManager) m_parentContext.getSystemService(Context.POWER_SERVICE);
        }

        if (wakeLock == null)
        {
            wakeLock = powerManager.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "JavaCdc");
        }



        //
        if (manager == null)
        {
            Log.d (TAG, "No manager");
            return -1;
        }

        //
        if (!registered)
        {
            IntentFilter filter = new IntentFilter();
            filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
            filter.setPriority(500);
            m_parentContext.registerReceiver(mUsbReceiver, filter);
            Log.d (TAG, "Register USB Receiver()");
            registered = true;
        }

        // Release previous connection
        if (connection != null)
        {
            synchronized (connection)
            {
                if (intf_data != null)
                    connection.releaseInterface (intf_data);
                connection.close ();
            }
            connection = null;
            intf_data = null;
            ep_input = null;
            ep_output = null;
        }
        return 0;
    }

    // Check Device (return 1 if found)
    public static int checkDevice ()
    {
        UsbDevice device_found = null;

        Log.d (TAG, "checkDevice ()");

        //
        if (m_parentContext == null)
            return -1;

        if (manager == null)
            return -1;

        HashMap<String, UsbDevice> map = manager.getDeviceList();
        Iterator<UsbDevice> it = map.values().iterator();

        while(it.hasNext())
        {

            UsbDevice device = it.next();

            // Check VID and PID
            if ((device.getVendorId () == 0x3eb) && (device.getProductId () == 0x2404))
            {
                device_found = device;
                break;
            }
        }

        if (device_found == null)
        {
            Log.d (TAG, "Device not found.");
            return 0;
        }

        //
        Log.d (TAG, "Device found, name="+ device_found.getDeviceName ());

        // Request permission
        if (!manager.hasPermission (device_found))
        {
            PendingIntent permission = PendingIntent.getBroadcast(m_parentContext, 0, new Intent(ACTION_USB_PERMISSION), 0);
            manager.requestPermission (device_found, permission);
            return 0;
        }
        else
        {
            Log.d (TAG, "Device permission grant.");
            deviceCdc = device_found;
            return 1;
        }
    }

    // Get Device Name
    public static String getDeviceName ()
    {
        if (m_parentContext == null)
            return "";

        if (manager == null)
            return "";

        if (deviceCdc == null)
            return "";

        return deviceCdc.getDeviceName ();
    }

    // Get VID
    public static int getVid ()
    {
        if (m_parentContext == null)
            return -1;

        if (manager == null)
            return -1;

        if (deviceCdc == null)
            return -1;

        return deviceCdc.getVendorId ();
    }

    // Get PID
    public static int getPid ()
    {
        if (m_parentContext == null)
            return -1;

        if (manager == null)
            return -1;

        if (deviceCdc == null)
            return -1;

        return deviceCdc.getProductId ();
    }

    // Open device
    public static int openDevice ()
    {
        Log.d (TAG, "openDevice ()");
        wakeLock.acquire();
        if (wakeLock.isHeld ())
            Log.d (TAG, "WakeLock is held.");
        else
            Log.d (TAG, "WakeLock is not held.");

        //
        if (m_parentContext == null)
            return -1;

        if (manager == null)
            return -1;

        if (deviceCdc == null)
            return -1;

        // Close previous connection
        if (connection != null)
        {
            synchronized (connection)
            {
                if (intf_data != null)
                    connection.releaseInterface (intf_data);
                connection.close ();
            }
            connection = null;
        }

        // find the right interface
        intf_data = null;
        ep_input = null;
        ep_output = null;
        for(int i = 0; i < deviceCdc.getInterfaceCount(); i++)
        {
            // communications device class (CDC) type device
            if(deviceCdc.getInterface(i).getInterfaceClass() == UsbConstants.USB_CLASS_CDC_DATA)
            {
                intf_data = deviceCdc.getInterface(i);

                // find the endpoints
                for(int j = 0; j < intf_data.getEndpointCount(); j++)
                {
                    if ((intf_data.getEndpoint(j).getDirection() == UsbConstants.USB_DIR_OUT)
                        && (intf_data.getEndpoint(j).getType() == UsbConstants.USB_ENDPOINT_XFER_BULK))
                    {
                        // from android to device
                        ep_output = intf_data.getEndpoint(j);
                        Log.d (TAG, "Output Endpoint found. max=" + ep_output.getMaxPacketSize ());
                    }

                    if ((intf_data.getEndpoint(j).getDirection() == UsbConstants.USB_DIR_IN)
                        && (intf_data.getEndpoint(j).getType() == UsbConstants.USB_ENDPOINT_XFER_BULK))
                    {
                        // from device to android
                        ep_input = intf_data.getEndpoint(j);
                        Log.d (TAG, "Input Endpoint found. max=" + ep_input.getMaxPacketSize ());
                    }
                }
            }
         }
         if ((intf_data == null) || (ep_input == null) || (ep_output == null))
         {
            Log.d (TAG, "Unable to find interface.");
            intf_data = null;
            ep_input = null;
            ep_output = null;
            return -1;
        }

        // Open Device
        connection = manager.openDevice (deviceCdc);
        if (connection == null)
        {
            Log.d (TAG, "openDevice() fail.");
            intf_data = null;
            ep_input = null;
            ep_output = null;
            return -1;
        }
        synchronized (connection)
        {
            if (!connection.claimInterface (intf_data, true))
            {
                Log.d (TAG, "claimInterface() fail.");
                intf_data = null;
                ep_input = null;
                ep_output = null;
                return -1;
            }
        }

        //
        Log.d (TAG, "Connection opened.");

        // Enable DTE
        if (connection.controlTransfer  (0x21, 0x22, 0x01, 0, null, 0x00, 1000) < 0)
            Log.d (TAG, "controlTransfer() fail.");
        else
            Log.d (TAG, "controlTransfer() done.");

        //
        return 0;
    }

    // Check Connection
    public static int checkConnection ()
    {
        if (connection == null)
            return -1;
        else
            return 0;
    }

    // Close device
    public static void closeDevice ()
    {
        Log.d (TAG, "closeDevice ()");

        if (connection != null)
        {
            synchronized (connection)
            {
                if (intf_data != null)
                    connection.releaseInterface (intf_data);

                connection.close ();
            }
        }
        connection = null;
        intf_data = null;
        ep_input = null;
        ep_output = null;

        if (wakeLock.isHeld())
            wakeLock.release();
    }

    //
    public static int getTxMaxPacketSize ()
    {
        if (connection == null)
            return -1;
        if (intf_data == null)
            return -1;
        if (ep_output == null)
            return -1;

        return ep_output.getMaxPacketSize ();
    }

    //
    public static int getRxMaxPacketSize ()
    {
        if (connection == null)
            return -1;
        if (intf_data == null)
            return -1;
        if (ep_input == null)
            return -1;

        return ep_input.getMaxPacketSize ();
    }

    //
    public static int write (byte[] aSrc, int aLen)
    {
        int ret = 0;
        Log.d (TAG, "write(): " + aLen + " bytes");

        if (connection == null)
        {
            Log.d (TAG, "write fail, device not open.");
            return -1;
        }

//        //
//        byte[] out_data = new byte[] {0x47, 0x50, 0x0a};

        synchronized (connection)
        {
            ret = connection.bulkTransfer (ep_output, aSrc, aLen, 100);
        }

        Log.d (TAG, ret + " bytes sent.");

        return ret;
    }

    //
    public static int read (byte[] aDest, int aMaxLen)
    {
        int ret = 0;
        Log.d (TAG, "read()");

        if (connection == null)
        {
            Log.d (TAG, "read fail, device not open.");
            return -1;
        }

        //
        synchronized (connection)
        {
            ret = connection.bulkTransfer (ep_input, aDest, aMaxLen, 10);
        }
        Log.d (TAG, ret + " bytes received.");

        if (ret < 0)
            return 0;
        else
            return ret;
    }


    /***********USB broadcast receiver*******************************************/
    private static BroadcastReceiver mUsbReceiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
        Log.i (TAG, "mUsbReceiver");
            String action = intent.getAction();
    
            if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action))
            {
                Log.i (TAG, "DETACHED...");


                if (connection != null)
                {
                    synchronized (connection)
                    {
                        if (intf_data != null)
                            connection.releaseInterface (intf_data);
                        connection.close ();
                    }
                }
                connection = null;
                intf_data = null;
                ep_input = null;
                ep_output = null;
            }
        }
    };

}
