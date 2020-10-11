package com.example.prohelmetapp;

public interface Constants {

    public static final String LOG = "ProHelmet";
    public static final String UUID = "00001101-0000-1000-8000-00805F9B34FB";
    public static final String DEVICE_NAME = "ProHelmet";

    public static final long GPS_MS = 200;
    public static final float GPS_METERS = 0.1f;

    /*
     * Bluetooth definitions
     */
    public static final int EP_CTL_REQ_DATA =       (int) 'A';
    public static final int EP_CTL_ACK_DATA =       (int) 'B';
    public static final int EP_CTL_EP_NOTREADY =    (int) 'C';
    public static final int EP_CTL_EP_NOTEXISTS =   (int) 'D';

    public static final int EP_CTL_IDLE_RESYNCH =   0xFF;

    public static final int EP_PING =       (int) 'a';
    public static final int EP_SPEED =      (int) 'b';
    public static final int EP_TIME =      (int) 'c';


}
