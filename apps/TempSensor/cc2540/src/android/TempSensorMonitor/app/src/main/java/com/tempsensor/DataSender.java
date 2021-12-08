package com.tempsensor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpParams;

import android.app.Service;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.os.StrictMode;
import android.util.Base64;
import android.util.Log;
import android.widget.Toast;
 
public class DataSender implements Runnable
{
	private class QueueItem
	{
		public String mKey;
		public double mTemp;
		public double mVbat;
		public int mRssi;
		
		QueueItem(String key, double temp, double vbat, int rssi)
		{
			mKey = key;
			mTemp = temp;
			mVbat = vbat;
			mRssi = rssi;
		}
		
	};
	
	
    private static final String TAG = DataSender.class.getSimpleName();
	
    private final BlockingQueue<QueueItem> mDataQueue;
    private volatile boolean mRunning;
    private Thread mThread;
    private DeviceScanActivity mActivity;
	
	public DataSender(DeviceScanActivity activity)
	{
		mActivity = activity;
		
		StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
		StrictMode.setThreadPolicy(policy); 	
		
        mDataQueue = new LinkedBlockingQueue<QueueItem>();
        
        // Create and start working thread
        mThread = new Thread(this);        
        mRunning = true;
        mThread.start();        
	}
	
	public void sendDataToServer(String key, AdvTemperaturePacket packet, int rssi) throws Exception
	{
		QueueItem qi = new QueueItem(key, packet.mTemp, packet.mVbat, rssi);
		mDataQueue.put(qi);
	}

    //
    // Working thread loop
    //
    
	@Override
	public void run() 
	{
        Log.d(TAG, "Thread started");
        
        while(mRunning) 
        { 
            try 
            {
                // Get file to send from queue
            	QueueItem qi = mDataQueue.take();
            	
        		try 
        		{
        			String url = Constants.WRITE_URL;
        			url += "?key=" + qi.mKey;
        			url += "&field1=" + qi.mTemp;
        			url += "&field2=" + qi.mVbat;
        			url += "&field3=" + qi.mRssi;
        			
    	        	HttpClient httpclient = new DefaultHttpClient();
        			HttpGet httpGet = new HttpGet(url);

    	        	HttpResponse response = httpclient.execute(httpGet);
   	        	
    	        	if (response.getStatusLine().getStatusCode() != HttpStatus.SC_OK)
    	        	{
    	        		throw new Exception("Server response code = " + response.getStatusLine().getStatusCode());
    	        	}

    	        	// Get hold of the response entity
    	            HttpEntity entity = response.getEntity();
    	            // If the response does not enclose an entity, there is no need
    	            // to worry about connection release

    	            if (entity != null) {

    	                // A Simple JSON Response Read
    	                InputStream instream = entity.getContent();

    	                String result= convertStreamToString(instream);
    	                
    	                // now you have the string representation of the HTML request
    	                instream.close();

    	                mActivity.showToast(result);
    	            }    	        	
        		} 
        		catch (Exception e) 
        		{
        			// Add datafile for send to tail of the queue for repeated sending
        			//mDataFilesQueue.put(dataFileName);
        			
        			mActivity.showToast("ERROR: " + e.getMessage());

        			// Sleep sometime
        			Thread.sleep(5000);
        		}
            } 
            catch(InterruptedException e) 
            {
                Log.e(TAG, "Thread interrupted: " + e.getMessage());
                mRunning = false;
            }   
        }
        
        Log.d(TAG, "Thread stopped");
	}

	private static String convertStreamToString(InputStream is) {
	    /*
	     * To convert the InputStream to String we use the BufferedReader.readLine()
	     * method. We iterate until the BufferedReader return null which means
	     * there's no more data to read. Each line will appended to a StringBuilder
	     * and returned as String.
	     */
	    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
	    StringBuilder sb = new StringBuilder();

	    String line = null;
	    try {
	        while ((line = reader.readLine()) != null) {
	            sb.append(line + "\n");
	        }
	    } catch (IOException e) {
	        e.printStackTrace();
	    } finally {
	        try {
	            is.close();
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	    }
	    return sb.toString();
	}
	
}
