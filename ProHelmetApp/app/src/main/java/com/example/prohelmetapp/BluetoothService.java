package com.example.prohelmetapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import javax.xml.transform.sax.TemplatesHandler;

public class BluetoothService extends Thread {

    private MainActivity context;
    private BluetoothAdapter adapter;
    private BluetoothDevice device;
    private BluetoothSocket socket;

    private InputStream input;
    private OutputStream output;

    private Gps gps;

    public BluetoothService(MainActivity context, BluetoothAdapter adapter, BluetoothDevice device){
        this.context = context;
        this.adapter = adapter;
        this.device = device;
        this.gps = context.gps;
    }

    @Override
    public void run() {
        loop();

        if(!connectTo(device))
            return;

        if(!openStreams())
            return;

        context.setStatus(R.string.status_connected_str);

        loop();

        /* Clean */
        try {
            input.close();
            output.close();
            socket.close();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error closing resources exiting bluetooth service");
        }
    }

    protected void loop(){
        while(true){
            //check received
            long time = System.currentTimeMillis();
            while(System.currentTimeMillis() - time < 500);
            synchronized (gps.lock) {
                context.setSpeed(gps.getSpeed());
            }
        }
    }

    /*
     * Tries to open a connection to the desired device
     */
    protected boolean connectTo(BluetoothDevice device) {
        try {
            socket =  device.createRfcommSocketToServiceRecord(UUID.fromString(Constants.UUID));
            adapter.cancelDiscovery();
            socket.connect();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error opening socket: " + e.getCause());
            context.setStatus(R.string.status_failed_str);
            if(socket != null) {
                try {
                    socket.close();
                } catch (IOException ex) {
                    Log.d(Constants.LOG, "Error closing socket: " + ex.getCause());
                }
            }
            return false;
        }
        return true;
    }

    /*
     * Open streams
     */
    protected boolean openStreams(){
        try {
            input = socket.getInputStream();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error opening input stream");
            try {
                input.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
            return false;
        }

        try {
            output = socket.getOutputStream();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error opening output stream");
            try {
                output.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
            return false;
        }

        return true;
    }

    int cnt = 0;

    public void buttonCallback(int id){
        this.context.setTestText("Pressed button id:" + id + ", "+ ++cnt + " times.");
    }

}
