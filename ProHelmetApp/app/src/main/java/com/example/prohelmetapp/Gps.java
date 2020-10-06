package com.example.prohelmetapp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.Uri;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import java.util.List;

public class Gps implements LocationListener {

    protected LocationManager manager;
    protected Context context;
    private boolean first;

    public Object lock;

    Gps(Context context) {
        this.context = context;
        this.lock = new Object();

        manager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions((Activity) context, new String[]{android.Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 101);
        }

        first = true;

        manager.requestLocationUpdates(LocationManager.GPS_PROVIDER, Constants.GPS_MS, Constants.GPS_METERS, this);
        //manager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, Constants.GPS_MS, Constants.GPS_METERS, this);

    }

    private double lastLA, lastLO;
    private long lastT;

    private volatile double speed;

    @Override
    public void onLocationChanged(@NonNull Location location) {
        Log.d(Constants.LOG, "gps update");
        synchronized (this.lock) {
            this.speed = location.getSpeed()*3.6f;
        }

        /*
        if(first){
            lastLA = location.getLatitude();
            lastLO = location.getLongitude();
            lastT = System.currentTimeMillis();
            first = false;
        } else {
            double curLA = location.getLatitude();
            double curLO = location.getLongitude();
            long curT = System.currentTimeMillis();

            double speed = Math.abs(getDistance(curLA, curLO, lastLA, lastLO)) / (1000 * (curT - lastT));

            speed *= 3.6;

            lastLA = curLA;
            lastLO = curLO;
            lastT = curT;

            synchronized (this.lock) {
                this.speed = speed;
            }
        }*/
    }

    public double getSpeed(){
        return (double) speed;
    }

    private static double getDistance(double lat1, double lon1, double lat2, double lon2) {
        double R = 6371000;
        double dLat = lat2 - lat1;
        double dLon = lon2 - lon1;
        double a = Math.sin(dLat / 2) * Math.sin(dLat / 2)
                + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon / 2) * Math.sin(dLon / 2);
        return 2 * R * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
    }

}
