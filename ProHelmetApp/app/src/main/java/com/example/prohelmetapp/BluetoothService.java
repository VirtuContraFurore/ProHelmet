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
import java.util.Stack;
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

    private Stack<Message> msgs;
    private Stack<Direction> dirs;

    public class Direction {
        byte sign;
        short distance;
        String str;
        static final int icon_Turnbout_Left = 1;
        static final int icon_Exit_Right = 2;
    };

    public class Message {
        String msg;
        int icon;
        static final int icon_Info = 0;
        static final int icon_Call = 1;
        static final int icon_Telegram = 2;
    }

    /*
     * Contains the last event for each endpoint
     */
    private Map<Integer, Integer> events;

    private int pings = 0;

    private int speed_rand = 0;

    public BluetoothService(MainActivity context, BluetoothAdapter adapter, BluetoothDevice device){
        this.context = context;
        this.adapter = adapter;
        this.device = device;
        this.gps = context.gps;
        this.events = new HashMap<Integer, Integer>();
        this.msgs = new Stack<Message>();
        this.dirs = new Stack<Direction>();
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
        switch (id){
            case 1:
                Message m1 = new Message();
                m1.msg = "Paul: See you in 5 minutes!";
                m1.icon = Message.icon_Telegram;
                this.msgs.push(m1);
                break;
            case 2:
                Message m2 = new Message();
                m2.msg = "Joe: Never mind, I'm late...";
                m2.icon = Message.icon_Telegram;
                this.msgs.push(m2);
                break;
            case 3:
                Direction d1 = new Direction();
                d1.sign = Direction.icon_Turnbout_Left;
                d1.distance = 1200;
                d1.str = "Turn left at the roundabout";
                this.dirs.push(d1);
                break;
            case 6:
                speed_rand ^= 1;
                break;
            case 5:
                Direction d2 = new Direction();
                d2.sign = Direction.icon_Exit_Right;
                d2.distance = 9500;
                d2.str = "Exit the highway at \"Pisa Nord\"";
                this.dirs.push(d2);
                break;
            case 4:
                Message m3 = new Message();
                m3.msg = "Anne: Incoming Call...";
                m3.icon = Message.icon_Call;
                this.msgs.push(m3);
                break;
        }
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

            if((pings % 10) == 0)
                this.context.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(context, "Ping signal sent: "+pings, Toast.LENGTH_SHORT).show();
                    }
                });
            pings++;
        }

        if(events.containsKey(Constants.EP_TIME) && events.get(Constants.EP_TIME) == Constants.EP_CTL_REQ_DATA){
            byte msg[] = new byte[3];
            msg[0] = Constants.EP_TIME;
            msg[1] = (byte) Calendar.getInstance().get(Calendar.MINUTE);
            msg[2] = (byte) Calendar.getInstance().get(Calendar.HOUR);

            this.output.write(msg);
            events.remove(Constants.EP_TIME);
        }

        if(events.containsKey(Constants.EP_SPEED) && events.get(Constants.EP_SPEED) == Constants.EP_CTL_REQ_DATA){
            byte msg[] = new byte[5];
            msg[0] = Constants.EP_SPEED;

            float speed;
            if(speed_rand == 1)
                speed = (float) (200*Math.random())/3.6f;
            else {
                synchronized (gps.lock) {
                    speed = (float) (gps.getSpeed());
                }
            }

            if(!(speed < 0)) {
                int bits = Float.floatToIntBits(speed);
                msg[1] = (byte) ((bits >> 0) & 0xFF);
                msg[2] = (byte) ((bits >> 8) & 0xFF);
                msg[3] = (byte) ((bits >> 16) & 0xFF);
                msg[4] = (byte) ((bits >> 24) & 0xFF);
            }

            this.output.write(msg);
            events.remove(Constants.EP_SPEED);
        }

        if(!this.msgs.empty()){
            Message m = this.msgs.pop();
            byte ch[] = m.msg.getBytes();

            this.output.write(Constants.EP_NOTIFICATION);

            this.output.write(m.icon);

            for(int i = 0; i < Constants.EP_NOTIFICATION_SIZE - 1; i++){
                if(i < ch.length)
                    this.output.write(ch[i]);
                else
                    this.output.write(0);
            }
            this.output.write(0);

            this.context.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(context, "Sent msg", Toast.LENGTH_SHORT).show();
                }
            });
        }

        if(!this.dirs.isEmpty()){
            Direction dir =this.dirs.pop();
            this.output.write(Constants.EP_SATNAV);
            this.output.write(dir.sign);
            this.output.write(dir.distance & 0xFF);
            this.output.write((dir.distance >> 8 )& 0xFF);
            for(int i = 0; i < Constants.EP_SATNAV_SIZE - 3 - 1; i++){
                if(i < dir.str.length())
                    this.output.write(dir.str.charAt(i));
                else
                    this.output.write(0);
            }
            this.output.write(0);
        }
    }
}
