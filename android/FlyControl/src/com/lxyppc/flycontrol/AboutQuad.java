package com.lxyppc.flycontrol;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;

public class AboutQuad extends Activity{
/**
}
* Standard Android on create method that gets called when the activity initialized.
*/
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.about_web_view);
		WebView wv = (WebView)findViewById(R.id.about_web_view);
		wv.loadUrl("http://www.etootle.com/product/4xaircraft/4x_version_info.html");
	}
}
