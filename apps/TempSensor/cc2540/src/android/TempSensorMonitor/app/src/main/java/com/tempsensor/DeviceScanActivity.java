package com.tempsensor;

import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Typeface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.text.format.Time;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemLongClickListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.params.HttpParams;

import com.vitalitysensor.R;

/**
 * Activity for scanning and displaying available Bluetooth LE devices.
 */
public class DeviceScanActivity extends ListActivity
{
	private final static String TAG = DeviceScanActivity.class.getSimpleName();

	private LeDeviceListAdapter mLeDeviceListAdapter;
	private BluetoothAdapter mBluetoothAdapter;
	private boolean mScanning;
	private Handler mHandler;
	private ArrayList<BluetoothDevice> mDevices;

	private static final int REQUEST_ENABLE_BT = 1;

	private int mPacketSeq = 0xFF;
    private DataSender mDataSender; 
    private Time mStartTime;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		getActionBar().setTitle(R.string.title_devices);
		mHandler = new Handler();
		mDevices = new ArrayList<BluetoothDevice>();
		mStartTime = new Time();

		// Use this check to determine whether BLE is supported on the device.
		// Then you can
		// selectively disable BLE-related features.
		if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE))
		{
			Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
			finish();
		}

		// Initializes a Bluetooth adapter. For API level 18 and above, get a
		// reference to
		// BluetoothAdapter through BluetoothManager.
		final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
		mBluetoothAdapter = bluetoothManager.getAdapter();

		// Checks if Bluetooth is supported on the device.
		if (mBluetoothAdapter == null)
		{
			Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
			finish();
			return;
		}

		getListView().setLongClickable(true);
		getListView().setOnItemLongClickListener(new OnItemLongClickListener()
		{
			@Override
			public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id)
			{
				final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
				if (device == null)
					return false;

				try
				{
					// TODO: do some action on long click
				}
				catch (Exception e)
				{
					Toast.makeText(getBaseContext(), e.getMessage(), Toast.LENGTH_SHORT).show();
				}

				view.setSelected(true);
				return true;
			}
		});
		
		mDataSender = new DataSender(this); 
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.main, menu);
		if (!mScanning)
		{
			menu.findItem(R.id.menu_stop).setVisible(false);
			menu.findItem(R.id.menu_scan).setVisible(true);
			menu.findItem(R.id.menu_refresh).setActionView(null);
		}
		else
		{
			menu.findItem(R.id.menu_stop).setVisible(true);
			menu.findItem(R.id.menu_scan).setVisible(false);
			menu.findItem(R.id.menu_refresh).setActionView(R.layout.actionbar_indeterminate_progress);
		}
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
		case R.id.menu_scan:
			scanLeDevice(true);
			break;
		case R.id.menu_stop:
			scanLeDevice(false);
			break;
		}
		return true;
	}

	@Override
	protected void onResume()
	{
		super.onResume();

		// Ensures Bluetooth is enabled on the device. If Bluetooth is not
		// currently enabled,
		// fire an intent to display a dialog asking the user to grant
		// permission to enable it.
		if (!mBluetoothAdapter.isEnabled())
		{
			if (!mBluetoothAdapter.isEnabled())
			{
				Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
			}
		}

		// Initializes list view adapter.
		mLeDeviceListAdapter = new LeDeviceListAdapter();
		setListAdapter(mLeDeviceListAdapter);

		scanLeDevice(true);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		// User chose not to enable Bluetooth.
		if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED)
		{
			finish();
			return;
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	protected void onPause()
	{
		super.onPause();
		scanLeDevice(false);
		mLeDeviceListAdapter.clear();
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		// Get selected BT device
		final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
		if (device == null)
			return;

	}

	private void scanLeDevice(final boolean enable)
	{
		try
		{
			if (enable)
			{
				mDevices.clear();
				mLeDeviceListAdapter.clear();
				mLeDeviceListAdapter.notifyDataSetChanged();
				mStartTime.setToNow();
				mScanning = true;
				mBluetoothAdapter.startLeScan(mLeScanCallback);
			}
			else
			{
				mScanning = false;
				mBluetoothAdapter.stopLeScan(mLeScanCallback);
				mLeDeviceListAdapter.close();
			}
			invalidateOptionsMenu();
		}
		catch (Exception e)
		{
			Toast.makeText(getBaseContext(), e.getMessage(), Toast.LENGTH_SHORT).show();
		}
	}

	// Device scan callback.
	private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback()
	{
		@Override
		public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord)
		{
			runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					// Restart scan
					mBluetoothAdapter.stopLeScan(mLeScanCallback);
					mBluetoothAdapter.startLeScan(mLeScanCallback);
					
					try
					{
						// Parse incomming packet
						AdvTemperaturePacket packet = AdvTemperaturePacket.parse(scanRecord);
						if (packet != null && packet.mManufactureId == AdvTemperaturePacket.MANUFACTURE_ID)
						{
							mLeDeviceListAdapter.updateDevice(device, rssi, packet);
							mLeDeviceListAdapter.notifyDataSetChanged();
							
							if (packet.mSeqn != mPacketSeq)
								mDataSender.sendDataToServer(Constants.WRITE_KEY, packet, rssi);

							mPacketSeq = packet.mSeqn;
						}
					}
					catch (Exception e)
					{
						Toast.makeText(getBaseContext(), "ERROR: " + e.getMessage(), Toast.LENGTH_SHORT).show();
					}
				}
			});
		}
	};

	private class DeviceInfo
	{
		public BluetoothDevice mDevice;
		public int mRssi;
		public AdvTemperaturePacket mPacket;

		public DeviceInfo(BluetoothDevice device, int rssi, AdvTemperaturePacket packet) throws IOException
		{
			mDevice = device;
			mRssi = rssi;
			mPacket = packet;
		}

		public void update(int rssi, AdvTemperaturePacket packet)
		{
			mRssi = rssi;
			mPacket = packet;
		}
	}

	//
	// Adapter for holding devices found through scanning.
	//
	private class LeDeviceListAdapter extends BaseAdapter
	{
		private ArrayList<BluetoothDevice> mLeDevices;
		private ArrayList<DeviceInfo> mLeDevicesInfo;
		private LayoutInflater mInflator;

		public LeDeviceListAdapter()
		{
			super();
			mLeDevices = new ArrayList<BluetoothDevice>();
			mLeDevicesInfo = new ArrayList<DeviceInfo>();
			mInflator = DeviceScanActivity.this.getLayoutInflater();
		}

		public void updateDevice(BluetoothDevice device, int rssi, AdvTemperaturePacket packet) throws IOException
		{
			int index;

			index = mLeDevices.indexOf(device);
			if (index == -1)
			{
				mLeDevices.add(device);
				mLeDevicesInfo.add(new DeviceInfo(device, rssi, packet));
			}
			else
			{
				DeviceInfo devInfo = mLeDevicesInfo.get(index);
				devInfo.update(rssi, packet);
			}
		}

		public BluetoothDevice getDevice(int position)
		{
			return mLeDevices.get(position);
		}

		public void clear()
		{
			// Close all open devices data files
			for (int ix = 0; ix < mLeDevicesInfo.size(); ix++)
			{
				try
				{
					DeviceInfo di = mLeDevicesInfo.get(ix);
				}
				catch (Exception e)
				{
				}
			}

			mLeDevices.clear();
			mLeDevicesInfo.clear();
		}

		public void close()
		{
			// Close all open devices data files
			for (int ix = 0; ix < mLeDevicesInfo.size(); ix++)
			{
				try
				{
					DeviceInfo di = mLeDevicesInfo.get(ix);
				}
				catch (Exception e)
				{
				}
			}
		}

		@Override
		public int getCount()
		{
			return mLeDevices.size();
		}

		@Override
		public Object getItem(int i)
		{
			return mLeDevices.get(i);
		}

		@Override
		public long getItemId(int i)
		{
			return i;
		}

		@Override
		public View getView(int i, View view, ViewGroup viewGroup)
		{
			ViewHolder viewHolder;
			String txt;

			// General ListView optimization code.
			if (view == null)
			{
				view = mInflator.inflate(R.layout.listitem_device, null);
				viewHolder = new ViewHolder();
				viewHolder.deviceName = (TextView) view.findViewById(R.id.device_name);
				viewHolder.time = (TextView) view.findViewById(R.id.time);
				viewHolder.rssi = (TextView) view.findViewById(R.id.rssi);
				viewHolder.battery = (TextView) view.findViewById(R.id.battery);
				viewHolder.temp = (TextView) view.findViewById(R.id.temp);

				viewHolder.data = (TextView) view.findViewById(R.id.data);
				view.setTag(viewHolder);
			}
			else
			{
				viewHolder = (ViewHolder) view.getTag();
			}

			DeviceInfo devInfo = mLeDevicesInfo.get(i);

			viewHolder.deviceName.setText(devInfo.mDevice.getName() + " - " + devInfo.mDevice.getAddress());
			viewHolder.time.setText("Time / Seqn: " + Utils.getTimeStamp(mStartTime, devInfo.mPacket.mRecvTime) + " / " + devInfo.mPacket.mSeqn);
			viewHolder.rssi.setText("RSSI: " + devInfo.mRssi);
			viewHolder.battery.setText("Battery: " + devInfo.mPacket.mVbat);
			viewHolder.temp.setText("Temp: " + devInfo.mPacket.mTemp);
			viewHolder.data.setText("RawData:\n" + Utils.bytesToHex(devInfo.mPacket.mRawData));

			return view;
		}
	}

	static class ViewHolder
	{
		TextView deviceName;
		TextView time;
		TextView rssi;
		TextView battery;
		TextView temp;

		TextView data;
	}

	public void showToast(final String toast)
	{
	    runOnUiThread(new Runnable() {
	        public void run()
	        {
	            Toast.makeText(getBaseContext(), toast, Toast.LENGTH_SHORT).show();
	        }
	    });
	}


}
