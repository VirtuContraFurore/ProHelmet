package com.example.prohelmetapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
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

    /*
     * Contains the last event for each endpoint
     */
    private Map<Integer, Integer> events;

    public BluetoothService(MainActivity context, BluetoothAdapter adapter, BluetoothDevice device){
        this.context = context;
        this.adapter = adapter;
        this.device = device;
        this.gps = context.gps;
        this.events = new HashMap<Integer, Integer>();
    }

    @Override
    public void run() {
        if(!connectTo(device))
            return;

        if(!openStreams())
            return;

        context.setStatus(R.string.status_connected_str);

        try {
            loop();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Exeption in loop: " + e.getMessage());
            this.context.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(context, "I/O Error, aborting", Toast.LENGTH_SHORT).show();
                }
            });
        }

        /* Clean */
        try {
            input.close();
            output.close();
            socket.close();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error closing resources exiting bluetooth service");
        }

        this.context.setStatus(R.string.status_failed_str);
    }

    /*
     * Tries to open a connection to the desired device
     */
    protected boolean connectTo(BluetoothDevice device) {
        try {
            socket =  device.createRfcommSocketToServiceRecord(UUID.fromString(Constants.UUID));
            //adapter.cancelDiscovery();
            socket.connect();
        } catch (IOException e) {
            Log.d(Constants.LOG, "Error opening socket: " + e.getMessage());
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

    protected void loop() throws IOException {
        while(true){
            /* process inputs */
            this.processInput();

            long time = System.currentTimeMillis();
                    
            /* gps location */
            synchronized (gps.lock) {
                context.setSpeed(gps.getSpeed());
            }

            /*
             * Let's rest a bit to redraw screen :-)
             */
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                Log.d(Constants.LOG, "main loop sleep was interrupted: " + e.getMessage());
            }
        }
    }

    public void processInput() throws IOException {
        if(this.input.available() > 0){
            int data = input.read();
            switch (data){
                case Constants.EP_CTL_ACK_DATA:
                case Constants.EP_CTL_EP_NOTEXISTS:
                case Constants.EP_CTL_EP_NOTREADY:
                case Constants.EP_CTL_REQ_DATA:
                    while(this.input.available() < 1);
                    int ep = input.read();
                    this.events.put(ep, data);
                    break;
            }
        }

        /*
         * Ping it back!
         */
        if(events.containsKey(Constants.EP_PING) && events.get(Constants.EP_PING) == Constants.EP_CTL_REQ_DATA){
            byte msg[] = new byte[3];
            msg[0] = Constants.EP_PING;
            msg[1] = 'O';
            msg[2] = 'K';

            this.output.write(msg);
            events.remove(Constants.EP_PING);

            this.context.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(context, "Ping signal sent", Toast.LENGTH_SHORT).show();
                }
            });
        }

        if(events.containsKey(Constants.EP_TIME) && events.get(Constants.EP_TIME) == Constants.EP_CTL_REQ_DATA){
            byte msg[] = new byte[3];
            msg[0] = Constants.EP_TIME;
            msg[1] = (byte) Calendar.getInstance().get(Calendar.MINUTE);
            msg[2] = (byte) Calendar.getInstance().get(Calendar.HOUR);

            this.output.write(msg);
            events.remove(Constants.EP_TIME);
        }
    }
}
