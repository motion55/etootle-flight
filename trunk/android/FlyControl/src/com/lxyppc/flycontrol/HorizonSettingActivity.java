package com.lxyppc.flycontrol;

import com.lxyppc.flycontrol.PIDSettingActivity.PIDParam;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.InputType;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;
public class HorizonSettingActivity extends Activity {
	public class HorizonParam{
		HorizonParam(Context ctx){
			w = new EditText(ctx);
			x = new EditText(ctx);
			y = new EditText(ctx);
			z = new EditText(ctx);
			w.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			x.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			y.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			z.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
		}
		EditText w;
		EditText x;
		EditText y;
		EditText z;
	}
	private HorizonParam[] params = new HorizonParam[3];
	private EditText[] edits = new EditText[4];
	private QuadParamSettingActivity.ParamButton buttons;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_horizon_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.horizon_param_layout);
		String[] name = new String[]{
			getString(R.string.quad_horizon),
		};
		
		{
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			TextView v2 = new TextView(this);
			TextView v3 = new TextView(this);
			TextView v4 = new TextView(this);
			TextView v5 = new TextView(this);
			v1.setText(R.string.quad_horizon);
			v2.setText(R.string.quad_horizon_w);
			v3.setText(R.string.quad_horizon_x);
			v4.setText(R.string.quad_horizon_y);
			v4.setText(R.string.quad_horizon_z);
			ll.addView(v1,lp);
			ll.addView(v2,lp);
			ll.addView(v3,lp);
			ll.addView(v4,lp);
			ll.addView(v5,lp);
			tl.addView(ll);
		}
		for(int i=0;i<name.length;i++){
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			v1.setText(name[i]);
			params[i] = new HorizonParam(this);
			ll.addView(v1,lp);
			ll.addView(params[i].w,lp);
			ll.addView(params[i].x,lp);
			ll.addView(params[i].y,lp);
			ll.addView(params[i].z,lp);
			tl.addView(ll);
			
			edits[i*4 + 0] = params[i].w;
			edits[i*4 + 1] = params[i].x;
			edits[i*4 + 2] = params[i].y;
			edits[i*4 + 3] = params[i].z;
		}
		buttons = new QuadParamSettingActivity.ParamButton(this, 19, 4, edits);
		buttons.append2layout(tl);
	}
	
	protected void onResume(){
		buttons.install_listener();
		super.onResume();
	}
	
	protected void onDestroy(){
		buttons.remove_listener();
		super.onDestroy();
	}
}
