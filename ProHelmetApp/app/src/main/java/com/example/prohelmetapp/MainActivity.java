package com.example.prohelmetapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private final static int REQUEST_ENABLE_BT = 1;

    private BluetoothService service;
    protected Gps gps;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        gps = new Gps(this);
    }

    /*
     * Connect Button callback
     */
    public void connectButtonOnClick(View view){
        if(service != null && service.isAlive()) {
            Toast.makeText(this, "Service already started", Toast.LENGTH_SHORT).show();
            return;
        }

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            return;
        }

        if(!bluetoothAdapter.isEnabled()){
            showInfo("Bluetooth is disabled, you must grant Bluetooth access rights to the application.");
            return;
        }

        BluetoothDevice helmet = findHelmetDevice();

        if(helmet == null){
            showInfo("Can't find a paired Bluetooth device with name \"ProHelmet\"");
            return;
        }

        setStatus(R.string.status_connecting_str);

        /* Creates base service */
        service = new BluetoothService(this, BluetoothAdapter.getDefaultAdapter(), helmet);
        service.start();
    }

    protected BluetoothDevice findHelmetDevice(){
        Set<BluetoothDevice> pairedDevices = BluetoothAdapter.getDefaultAdapter().getBondedDevices();
        BluetoothDevice target = null;

        for(BluetoothDevice bt : pairedDevices) {
            if (bt.getName().equals(Constants.DEVICE_NAME))
                target = bt;
        }

        return target;
    }

    protected void showInfo(String msg){
        AlertDialog.Builder builder1 = new AlertDialog.Builder(this);
        builder1.setMessage(msg);
        builder1.setCancelable(true);

        builder1.setPositiveButton(
                "Got it",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        AlertDialog alert11 = builder1.create();
        alert11.show();
    }

    protected void setStatus(int string_id){
        TextView connect_status = (TextView) findViewById(R.id.connect_status);
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                connect_status.setText(string_id);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == REQUEST_ENABLE_BT && resultCode == -1)
            this.connectButtonOnClick(this.getCurrentFocus());
    }

}
