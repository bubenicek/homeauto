package com.tempsensor;

import java.util.Arrays;

import android.text.format.Time;

public class AdvTemperaturePacket 
{
	//
	// Constants
	//
	public static final int ADV_MANUFACTER_OFFSET = 16;	
	public static final int ADV_DATA_OFFSET = 21;	
	public static final int MANUFACTURE_ID = 0xCAFE;	
	
	public byte [] mRawData;
	
	public int mLength;
	public int mManufactureId;
	public int mSeqn;
	
	public int mVbatRaw;
	public double mVbat;
	
	public double mTemp;

	public int mTempRaw;
	public Time mRecvTime;

	
	public AdvTemperaturePacket(byte[] rawData) throws Exception
	{
		mRecvTime = new Time();
		mRecvTime.setToNow();
		
		// Parse advertising header size
		mLength = rawData.length;

    	mManufactureId = ((rawData[ADV_MANUFACTER_OFFSET] << 8) | (rawData[ADV_MANUFACTER_OFFSET + 1] & 0xFF)) & 0xFFFF;
		
		mSeqn = (rawData[ADV_DATA_OFFSET] & 0xFF);
    	mTempRaw = ((rawData[ADV_DATA_OFFSET + 1] << 8) | (rawData[ADV_DATA_OFFSET + 2] & 0xFF)) & 0xFFFF;
    	
    	mVbatRaw = ((rawData[ADV_DATA_OFFSET + 3] << 8) | (rawData[ADV_DATA_OFFSET + 4] & 0xFF)) & 0xFFFF;
    	mVbat = Utils.round((double) (1.15 / 511) * mVbatRaw * 3, 2);    	
    	
    	int SignBit, Whole, Fract;    	
    	
    	 SignBit = mTempRaw & 0x8000;  // test most sig bit
    	 if (SignBit != 0) // negative
    		 mTempRaw = (mTempRaw ^ 0xffff) + 1; // 2's comp

    	Whole = mTempRaw >> 4;  // separate off the whole and fractional portions
    	Fract = (mTempRaw & 0xf) * 100 / 16;

    	String tempStr;
    	
    	if (SignBit != 0)
    		tempStr = String.format("-%d.%d", Whole, Fract < 10 ? 0 : Fract);
    	else
    		tempStr = String.format("%d.%d", Whole, Fract < 10 ? 0 : Fract);

    	mTemp = Double.parseDouble(tempStr);
    		
		// Copy raw data
		mRawData = Arrays.copyOf(rawData, mLength + ADV_DATA_OFFSET);
	}
	
	/** Parse raw data and when data are valid then return new instance of packet */
	static AdvTemperaturePacket parse(byte[] rawData)
	{
		try
		{
			return new AdvTemperaturePacket(rawData);
		}
		catch(Exception e)
		{
			return null;	
		}
	}
	
}
