package com.tempsensor;

import java.math.BigDecimal;
import java.math.RoundingMode;

import android.content.Context;
import android.location.Location;
import android.location.LocationManager;
import android.text.format.Time;

public class Utils 
{
	public static Location getLocation(Context context)
	{
		boolean gps_enabled = false;
		boolean network_enabled = false;

		LocationManager lm = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

		gps_enabled = lm.isProviderEnabled(LocationManager.GPS_PROVIDER);
		network_enabled = lm.isProviderEnabled(LocationManager.NETWORK_PROVIDER);

		Location net_loc = null, gps_loc = null, finalLoc = null;

		if (gps_enabled)
		    gps_loc = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
		if (network_enabled)
		    net_loc = lm.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);

		if (gps_loc != null && net_loc != null) 
		{
		    if (gps_loc.getAccuracy() >= net_loc.getAccuracy())
		        finalLoc = gps_loc;
		    else
		        finalLoc = net_loc;

		   // I used this just to get an idea (if both avail, its upto you which you want to take as I taken location with more accuracy)

		} 
		else 
		{
		    if (gps_loc != null) 
		    {
		        finalLoc = gps_loc;
		    } 
		    else if (net_loc != null) 
		    {
		        finalLoc = net_loc;
		    }
		}
		
		return finalLoc;
	}

	
	final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
	public static String bytesToHex(byte[] bytes) 
	{
		String hexChars = "";
	    
	    for ( int j = 0; j < bytes.length; j++ ) 
	    {
	        int v = bytes[j] & 0xFF;
	        hexChars += hexArray[v >>> 4];
	        hexChars += hexArray[v & 0x0F];
	        hexChars += " ";
	    }
	
	    return hexChars;
	}	
	
	public static double round(double value, int places) 
	{
	    BigDecimal bd = new BigDecimal(value);
	    bd = bd.setScale(places, RoundingMode.HALF_UP);
	    return bd.doubleValue();
	}
	
	public static String getTimeStamp(Time start) 
	{
        Time now = new Time();
        Time diff = new Time();
        
        now.setToNow();
        diff.set(now.toMillis(false) - start.toMillis(false) - 3600 * 1000);
        
        String sTime = diff.format("%H:%M:%S");
        
        return sTime;
    }	

	public static String getTimeStamp(Time start, Time now) 
	{
        Time diff = new Time();
        diff.set(now.toMillis(false) - start.toMillis(false) - 3600 * 1000);
        String sTime = diff.format("%H:%M:%S");
        return sTime;
    }	
	
}
