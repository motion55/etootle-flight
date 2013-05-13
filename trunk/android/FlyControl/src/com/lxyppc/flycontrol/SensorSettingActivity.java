package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.InputType;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;
public class SensorSettingActivity extends Activity {
	public class SensorParam{
		SensorParam(Context ctx){
			x = new EditText(ctx);
			y = new EditText(ctx);
			z = new EditText(ctx);
			x.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			y.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			z.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
		}
		EditText x;
		EditText y;
		EditText z;
	}
	public class ParamButton{
		public static final int GET_RAM = 0;
		public static final int SET_RAM = 1;
		public static final int GET_FLASH = 2;
		public static final int SET_FLASH = 3;
		public static final int RESET_FLASH = 4;
		ParamButton(Context ctx, int startPos, int count){
			mCtx = ctx;
			getRam = new Button(ctx);
			getRam.setText(R.string.quad_get_ram);
			setRam = new Button(ctx);
			setRam.setText(R.string.quad_set_ram);
			
			getFlash = new Button(ctx);
			getFlash.setText(R.string.quad_get_flash);
			setFlash = new Button(ctx);
			setFlash.setText(R.string.quad_set_flash);
			resetFlash = new Button(ctx);
			resetFlash.setText(R.string.quad_reset_flash);
			
			getRam.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { if(listener!=null)listener.onCLicked(GET_RAM); }});
			setRam.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { if(listener!=null)listener.onCLicked(SET_RAM); }});
			getFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { if(listener!=null)listener.onCLicked(GET_FLASH); }});
			setFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { if(listener!=null)listener.onCLicked(SET_FLASH); }});
			resetFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { if(listener!=null)listener.onCLicked(RESET_FLASH); }});
			
		}
		void enable(boolean enable){
			getRam.setEnabled(enable);
			setRam.setEnabled(enable);
			getFlash.setEnabled(enable);
			setFlash.setEnabled(enable);
			resetFlash.setEnabled(enable);
		}
		void append2layout(LinearLayout tl){
			LinearLayout ll = new LinearLayout(mCtx);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			ll.addView(buttons.getRam,lp);
			ll.addView(buttons.setRam,lp);
			tl.addView(ll);
			
			ll = new LinearLayout(mCtx);
			ll.addView(buttons.getFlash,lp);
			ll.addView(buttons.setFlash,lp);
			ll.addView(buttons.resetFlash,lp);
			tl.addView(ll);
		}
		void setListener(ClickListener l){
			listener = l;
		}
		private Context mCtx;
		Button getRam;
		Button setRam;
		Button getFlash;
		Button setFlash;
		Button resetFlash;
		private ClickListener listener = null;
		public class ClickListener{
			void onCLicked(int reason){
				
			}
		}
	}
	private SensorParam[] params = new SensorParam[6];
	private ParamButton buttons;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sensor_setting);
		
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.sensor_param_layout);
		tl.removeAllViews();
		String[] name = new String[]{
			getString(R.string.quad_acc_offset),
			getString(R.string.quad_acc_gain),
			getString(R.string.quad_gyro_offset),
			getString(R.string.quad_gyro_gain),
			getString(R.string.quad_mag_offset),
			getString(R.string.quad_mag_gain),
		};
		{
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			TextView v2 = new TextView(this);
			TextView v3 = new TextView(this);
			TextView v4 = new TextView(this);
			v1.setText(R.string.quad_sensor_cal);
			v2.setText(R.string.quad_sensor_x);
			v3.setText(R.string.quad_sensor_y);
			v4.setText(R.string.quad_sensor_z);
			ll.addView(v1,lp);
			ll.addView(v2,lp);
			ll.addView(v3,lp);
			ll.addView(v4,lp);
			tl.addView(ll);
		}
		for(int i=0;i<name.length;i++){
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			v1.setText(name[i]);
			params[i] = new SensorParam(this);
			ll.addView(v1,lp);
			ll.addView(params[i].x,lp);
			ll.addView(params[i].y,lp);
			ll.addView(params[i].z,lp);
			tl.addView(ll);
		}
		buttons = new ParamButton(this, 1, 18);
		buttons.append2layout(tl);
		buttons.setListener(buttons.new ClickListener(){
			void onCLicked(int reason){
				switch(reason){
				case ParamButton.GET_RAM:
					break;
				case ParamButton.SET_RAM:
					break;
				case ParamButton.GET_FLASH:
					break;
				case ParamButton.SET_FLASH:
					break;
				case ParamButton.RESET_FLASH:
					break;
				}
			}
		});
	}
}
