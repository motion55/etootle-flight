package com.lxyppc.flycontrol;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.UUID;

import org.andengine.engine.camera.Camera; 
import org.andengine.engine.camera.hud.controls.AnalogOnScreenControl;
import org.andengine.engine.camera.hud.controls.BaseOnScreenControl;
import org.andengine.engine.camera.hud.controls.AnalogOnScreenControl.IAnalogOnScreenControlListener;
import org.andengine.engine.handler.timer.ITimerCallback;
import org.andengine.engine.handler.timer.TimerHandler;
import org.andengine.engine.options.EngineOptions; 
import org.andengine.engine.options.ScreenOrientation; 
import org.andengine.engine.options.resolutionpolicy.RatioResolutionPolicy; 
import org.andengine.entity.scene.Scene; 
import org.andengine.entity.scene.background.Background;
import org.andengine.entity.sprite.AnimatedSprite; 
import org.andengine.entity.text.Text;
import org.andengine.entity.util.FPSCounter;
import org.andengine.entity.util.FPSLogger;
import org.andengine.extension.multiplayer.protocol.adt.message.client.IClientMessage;
import org.andengine.extension.multiplayer.protocol.adt.message.server.IServerMessage;
import org.andengine.extension.multiplayer.protocol.client.IServerMessageReader;
import org.andengine.extension.multiplayer.protocol.client.connector.BluetoothSocketConnectionServerConnector;
import org.andengine.extension.multiplayer.protocol.client.connector.ServerConnector;
import org.andengine.extension.multiplayer.protocol.client.connector.SocketConnectionServerConnector;
import org.andengine.extension.multiplayer.protocol.client.connector.BluetoothSocketConnectionServerConnector.IBluetoothSocketConnectionServerConnectorListener;
import org.andengine.extension.multiplayer.protocol.client.connector.SocketConnectionServerConnector.ISocketConnectionServerConnectorListener;
import org.andengine.extension.multiplayer.protocol.shared.BluetoothSocketConnection;
import org.andengine.extension.multiplayer.protocol.shared.Connection;
import org.andengine.extension.multiplayer.protocol.shared.IMessageHandler;
import org.andengine.extension.multiplayer.protocol.shared.SocketConnection;
import org.andengine.input.touch.TouchEvent;
import org.andengine.input.touch.controller.MultiTouch;
import org.andengine.opengl.font.Font;
import org.andengine.opengl.font.FontFactory;
import org.andengine.opengl.texture.TextureOptions; 
import org.andengine.opengl.texture.atlas.bitmap.BitmapTextureAtlas; 
import org.andengine.opengl.texture.atlas.bitmap.BitmapTextureAtlasTextureRegionFactory; 
import org.andengine.opengl.texture.atlas.bitmap.BuildableBitmapTextureAtlas;
import org.andengine.opengl.texture.atlas.bitmap.source.IBitmapTextureAtlasSource;
import org.andengine.opengl.texture.atlas.buildable.builder.BlackPawnTextureAtlasBuilder;
import org.andengine.opengl.texture.atlas.buildable.builder.ITextureAtlasBuilder.TextureAtlasBuilderException;
import org.andengine.opengl.texture.region.ITextureRegion;
import org.andengine.opengl.texture.region.TiledTextureRegion; 
import org.andengine.ui.activity.SimpleBaseGameActivity;
import org.andengine.util.debug.Debug;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Typeface;
import android.net.Uri;
import android.opengl.GLES20;
import android.os.Build;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class MainActivity extends SimpleBaseGameActivity implements ButtonGroup.ButtonClicked{
	private static final String PRE_NAME = "flycontrol";
	public static final String TAG = "FLYC";
	
	ViewSettingActivity.ViewSetting mViewSetting = new ViewSettingActivity.ViewSetting();
	NetSettingActivity.NetSetting mNetSetting = new NetSettingActivity.NetSetting();
	BlueSettingActivity.BlueSetting mBlueSetting = new BlueSettingActivity.BlueSetting();
	AxisSettingActivity.AxisSetting mAxisSetting = new AxisSettingActivity.AxisSetting();
	
	private Camera mCamera;

	private BitmapTextureAtlas mBitmapTextureAtlas;
	//private ITextureRegion mFaceTextureRegion;

	private BitmapTextureAtlas mOnScreenControlTexture;
	private ITextureRegion mOnScreenControlBaseTextureRegion;
	private ITextureRegion mOnScreenControlKnobTextureRegion;
	
	private AnalogOnScreenControl mVelocityOnScreenControl;
	private AnalogOnScreenControl mRotationOnScreenControl;
	
	private AnimatedSprite mWifiState;
	private AnimatedSprite mBlueState;

	private Font mFont;
	private Text mLeftText;
	private Text mRightText;
	private Font[] mInfoFont = new Font[6];
	private Text[] mInfoText = new Text[6];
	private float mInitFontSize = 14f;
	private float mDeltaFontSize = 1.2f;
	
	private boolean mPlaceOnScreenControlsAtDifferentVerticalLocations = false;
	
	private BuildableBitmapTextureAtlas mBitmapTextureAtlasAni;
	private TiledTextureRegion mWifiStateTextureRegion;
	private TiledTextureRegion mBlueStateTextureRegion;
	
	private ButtonGroup mBtnGroup = new ButtonGroup(this);

	private BluetoothAdapter mBluetoothAdapter = null;
	
	private float mX1 = 0f;
	private float mY1 = 0f;
	private float mX2 = 0f;
	private float mY2 = 0f;
	
	private BinaryParser mBinaryParser = null;
	
	@Override
	public EngineOptions onCreateEngineOptions() {
		mViewSetting.restore(this, PRE_NAME);
		mNetSetting.restore(this, PRE_NAME);
		mBlueSetting.restore(this, PRE_NAME);
		mBtnGroup.restore(this, PRE_NAME);
		mAxisSetting.restore(this, PRE_NAME);
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		this.mCamera = new Camera(0, 0, mViewSetting.mWidth, mViewSetting.mHeight);
		final EngineOptions engineOptions = new EngineOptions(true, ScreenOrientation.LANDSCAPE_FIXED, new RatioResolutionPolicy(mViewSetting.mWidth, mViewSetting.mHeight), this.mCamera);
		engineOptions.getTouchOptions().setNeedsMultiTouch(true);

		if(MultiTouch.isSupported(this)) {
			if(MultiTouch.isSupportedDistinct(this)) {
				Toast.makeText(this, getString(R.string.touch_both), Toast.LENGTH_SHORT).show();
			} else {
				this.mPlaceOnScreenControlsAtDifferentVerticalLocations = true;
				Toast.makeText(this, getString(R.string.touch_single), Toast.LENGTH_LONG).show();
			}
		} else {
			Toast.makeText(this, getString(R.string.touch_none), Toast.LENGTH_LONG).show();
		}
		mBinaryParser = new BinaryParser(new ControlSignal());
		
		mBtnGroup.setButtonListener(this);
		return engineOptions;
	}
    
	@Override
	public void onCreateResources() {
		BitmapTextureAtlasTextureRegionFactory.setAssetBasePath("gfx/");

		this.mBitmapTextureAtlas = new BitmapTextureAtlas(this.getTextureManager(), 32, 32, TextureOptions.BILINEAR);
		//this.mFaceTextureRegion = BitmapTextureAtlasTextureRegionFactory.createFromAsset(this.mBitmapTextureAtlas, this, "face_box.png", 0, 0);
		this.mBitmapTextureAtlas.load();

		this.mOnScreenControlTexture = new BitmapTextureAtlas(this.getTextureManager(), 256, 128, TextureOptions.BILINEAR);
		this.mOnScreenControlBaseTextureRegion = BitmapTextureAtlasTextureRegionFactory.createFromAsset(this.mOnScreenControlTexture, this, "onscreen_control_base.png", 0, 0);
		this.mOnScreenControlKnobTextureRegion = BitmapTextureAtlasTextureRegionFactory.createFromAsset(this.mOnScreenControlTexture, this, "onscreen_control_knob.png", 128, 0);
		this.mOnScreenControlTexture.load();
		
		this.mFont = FontFactory.create(this.getFontManager(), this.getTextureManager(), 256, 256, TextureOptions.BILINEAR, Typeface.create(Typeface.DEFAULT, Typeface.BOLD), 16);
		this.mFont.load();
		
		for(int i=0;i<mInfoFont.length;i++){
		this.mInfoFont[i] = FontFactory.create(this.getFontManager(), this.getTextureManager(), 256, 256, TextureOptions.BILINEAR, Typeface.create(Typeface.DEFAULT, Typeface.BOLD), 
				mInitFontSize - mDeltaFontSize*i);
		this.mInfoFont[i].load();
		}
		
		
		this.mBitmapTextureAtlasAni = new BuildableBitmapTextureAtlas(this.getTextureManager(), 512, 256, TextureOptions.NEAREST);
		
		this.mWifiStateTextureRegion = BitmapTextureAtlasTextureRegionFactory.createTiledFromAsset(this.mBitmapTextureAtlasAni, this, "wifi_sig.png", 3, 2);
		this.mBlueStateTextureRegion = BitmapTextureAtlasTextureRegionFactory.createTiledFromAsset(this.mBitmapTextureAtlasAni, this, "blue_sig.png", 3, 2);

		mBtnGroup.loadResources(mBitmapTextureAtlasAni, this);
		try {
			this.mBitmapTextureAtlasAni.build(new BlackPawnTextureAtlasBuilder<IBitmapTextureAtlasSource, BitmapTextureAtlas>(0, 0, 1));
			this.mBitmapTextureAtlasAni.load();
		} catch (TextureAtlasBuilderException e) {
			Debug.e(e);
		}
		mDataProcessor.start();
	}
    
    
	@Override
	public Scene onCreateScene() {
		final FPSCounter fpsCounter = new FPSCounter();
		this.mEngine.registerUpdateHandler(fpsCounter);
		this.mEngine.registerUpdateHandler(new FPSLogger());

		final Scene scene = new Scene();
		scene.setBackground(new Background(0.09804f, 0.6274f, 0.8784f));

		//final float centerX = (mViewSetting.mWidth - this.mFaceTextureRegion.getWidth()) / 2;
		//final float centerY = (mViewSetting.mHeight - this.mFaceTextureRegion.getHeight()) / 2;
		//final Sprite face = new Sprite(centerX, centerY, this.mFaceTextureRegion, this.getVertexBufferObjectManager()){
		//	public boolean onAreaTouched(final TouchEvent pSceneTouchEvent, final float pTouchAreaLocalX, final float pTouchAreaLocalY) {
		//		this.setPosition(pSceneTouchEvent.getX() - this.getWidth() / 2, pSceneTouchEvent.getY() - this.getHeight() / 2);
		//		return true;
		//	}
		//};
		//final PhysicsHandler physicsHandler = new PhysicsHandler(face);
		//face.registerUpdateHandler(physicsHandler);

		//scene.attachChild(face);
		//scene.registerTouchArea(face);
		
		
		mWifiState = new AnimatedSprite(
				mViewSetting.mWidth-this.mWifiStateTextureRegion.getWidth(), 0, this.mWifiStateTextureRegion, this.getVertexBufferObjectManager()){
			public boolean onAreaTouched(final TouchEvent pSceneTouchEvent, final float pTouchAreaLocalX, final float pTouchAreaLocalY) {
				if(pSceneTouchEvent.isActionDown()){
					if(mWifiState.isVisible()){
						if(mWifiState.getCurrentTileIndex() == FAIL_INDEX){
							dealConnection();
						}
					}
				}
				return true;
			}
		};
		scene.attachChild(mWifiState);
		if(mNetSetting.mConnectServer){
			setWifiState(WIFI_CONNECT_FAIL);
		}else{
			setWifiState(WIFI_NONE);
		}
		
		mBlueState = new AnimatedSprite(
				mViewSetting.mWidth-this.mWifiStateTextureRegion.getWidth()-this.mBlueStateTextureRegion.getWidth(), 0, this.mBlueStateTextureRegion, this.getVertexBufferObjectManager()){
			public boolean onAreaTouched(final TouchEvent pSceneTouchEvent, final float pTouchAreaLocalX, final float pTouchAreaLocalY) {
				if(pSceneTouchEvent.isActionDown()){
					if(mBlueState.isVisible()){
						if(mBlueState.getCurrentTileIndex() == FAIL_INDEX){
							dealBlueConnection();
						}
					}
				}
				return true;
			}
		};
		scene.attachChild(mBlueState);
		if(mBlueSetting.mConnect){
			setBlueState(BLUE_CONNECT_FAIL);
		}else{
			setBlueState(BLUE_NONE);
		}
		scene.registerTouchArea(mWifiState);
		scene.registerTouchArea(mBlueState);

		/* Velocity control (left). */
		final float x1 = 0;
		final float y1 = mViewSetting.mHeight - this.mOnScreenControlBaseTextureRegion.getHeight() - mViewSetting.mPosition;
		mVelocityOnScreenControl = new AnalogOnScreenControl(x1, y1, this.mCamera, this.mOnScreenControlBaseTextureRegion, this.mOnScreenControlKnobTextureRegion, 0.1f, this.getVertexBufferObjectManager(), new IAnalogOnScreenControlListener() {
			@Override
			public void onControlChange(final BaseOnScreenControl pBaseOnScreenControl, final float pValueX, final float pValueY) {
				//physicsHandler.setVelocity(pValueX * 100, pValueY * 100);
				mLeftText.setText(String.format("(%.2f,%.2f)", pValueX, pValueY));
				mX1 = pValueX;
				mY1 = pValueY;
				//sendControlState(pValueX, pValueY, mRotationOnScreenControl.getX(), mRotationOnScreenControl.getY());
			}

			@Override
			public void onControlClick(final AnalogOnScreenControl pAnalogOnScreenControl) {
				/* Nothing. */
			}
		});
		
		mVelocityOnScreenControl.getControlBase().setBlendFunction(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
		mVelocityOnScreenControl.getControlBase().setAlpha(0.5f);
		
		mViewSetting.mCtrlHeight = (int)mOnScreenControlBaseTextureRegion.getHeight();

		scene.setChildScene(mVelocityOnScreenControl);


		/* Rotation control (right). */
		final float y2 = (this.mPlaceOnScreenControlsAtDifferentVerticalLocations) ? 0 : y1;
		final float x2 = mViewSetting.mWidth - this.mOnScreenControlBaseTextureRegion.getWidth();
		mRotationOnScreenControl = new AnalogOnScreenControl(x2, y2, this.mCamera, this.mOnScreenControlBaseTextureRegion, this.mOnScreenControlKnobTextureRegion, 0.1f, this.getVertexBufferObjectManager(), new IAnalogOnScreenControlListener() {
			@Override
			public void onControlChange(final BaseOnScreenControl pBaseOnScreenControl, final float pValueX, final float pValueY) {
				//if(pValueX == x1 && pValueY == x1) {
				//	face.setRotation(x1);
				//} else {
				//	face.setRotation(MathUtils.radToDeg((float)Math.atan2(pValueX, -pValueY)));
				//}
				mRightText.setText(String.format(getString(R.string.axis_format), pValueX, pValueY));
				mX2 = pValueX;
				mY2 = pValueY;
				//sendControlState(mVelocityOnScreenControl.getX(), mVelocityOnScreenControl.getY(),pValueX, pValueY);
			}

			@Override
			public void onControlClick(final AnalogOnScreenControl pAnalogOnScreenControl) {
				/* Nothing. */
			}
		});
		mRotationOnScreenControl.getControlBase().setBlendFunction(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
		mRotationOnScreenControl.getControlBase().setAlpha(0.5f);

		mVelocityOnScreenControl.setChildScene(mRotationOnScreenControl);
		
		
		final Text fpsText = new Text(0, 0, this.mFont, getString(R.string.fps)+":", 64, this.getVertexBufferObjectManager());
		mLeftText = new Text(0, 20, this.mFont, "(0.00,0.00)", 64, this.getVertexBufferObjectManager());
		mRightText = new Text(mViewSetting.mWidth/2, 20, this.mFont, "(0.00,0.00)", 64, this.getVertexBufferObjectManager());
		float init_text_pos = mViewSetting.mHeight - mInitFontSize - 1;
		float dXpos = (mViewSetting.mWidth/3 - mViewSetting.mWidth/6)/mInfoText.length/2;
		for(int i=0;i<mInfoText.length;i++){
			float y = init_text_pos;
			mInfoText[i] = new Text(mViewSetting.mWidth/3 + dXpos*i, y, this.mInfoFont[i], "", 64, this.getVertexBufferObjectManager());
			scene.attachChild(mInfoText[i]);
			init_text_pos -= mInitFontSize - i*mDeltaFontSize;
		}
		
		
		if(mViewSetting.mTextInfo){
			scene.attachChild(fpsText);
			scene.attachChild(mLeftText);
			scene.attachChild(mRightText);
		}
		
		scene.registerUpdateHandler(new TimerHandler(1 / (float)mViewSetting.mFreq, true, new ITimerCallback() {
			@Override
			public void onTimePassed(final TimerHandler pTimerHandler) {
				if(mViewSetting.mTextInfo){
					fpsText.setText( getString(R.string.fps)+ ": " + (int)fpsCounter.getFPS());
				}
				updateControlState();
				//sendControlState(mX1,mY1,mX2,mY2);
			}
		}));
		
		mBtnGroup.onCreateScene(scene, mViewSetting.mWidth, mViewSetting.mHeight, this.mFont, this.getVertexBufferObjectManager());
		
		mBtnGroup.update(this.mViewSetting.mLockAttitude);
		return scene;
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public class ControlSignal extends BinaryParser.Signals{
		void printMessage(String msg){
			for(int i=mInfoText.length-1;i>0;i--){
				mInfoText[i].setText(mInfoText[i-1].getText());
			}
			mInfoText[0].setText(msg);
		}
		void send(byte[] data){
			sendMessge(new FrameMessage(data));
		}
		void onReturnMessage(String msg){ 
			printMessage(msg);
		}
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		Log.d("XXXX", "item: " + item.getItemId() + item.getTitle());
	    switch (item.getItemId()) {
			case R.id.action_view_settings:
			{
				Intent i = new Intent(this, ViewSettingActivity.class);
				mViewSetting.toIntent(i);
	            startActivityForResult(i, R.id.action_view_settings);
			}
				break;
			case R.id.action_net_settings:
			{
				Intent i = new Intent(this, NetSettingActivity.class);
				mNetSetting.toIntent(i);
	            startActivityForResult(i, R.id.action_net_settings);
			}
			    break;
			case R.id.action_blue_settings:
			{
				Intent i = new Intent(this, BlueSettingActivity.class);
				mBlueSetting.toIntent(i);
	            startActivityForResult(i, R.id.action_blue_settings);
			}
			    break;
			case R.id.action_button_settings:
			{
				Intent i = new Intent(this, ButtonSettingActivity.class);
				mBtnGroup.getSetting().toIntent(i);
				startActivityForResult(i, R.id.action_button_settings);
			}
				break;
			case R.id.action_axis_settings:
			{
				Intent i = new Intent(this, AxisSettingActivity.class);
				mAxisSetting.toIntent(i);
				startActivityForResult(i, R.id.action_axis_settings);
			}	
				break;
            case R.id.action_quad_param:
            {
            	Intent i = new Intent(this, QuadParamSettingActivity.class);
				this.startActivity(i);
            }
            	break;
			case R.id.action_about:
			{
				AboutDialog about = new AboutDialog(this);
				about.setTitle(getString(R.string.about_flycontrol));
				about.show();
			}
				break;
			case R.id.action_help:
			{
				Intent it = new Intent(Intent.ACTION_VIEW, Uri.parse(getString(R.string.info_link)));
		        //it.setClassName("com.android.browser", "com.android.browser.BrowserActivity");
		        startActivity(it);
			}
				break;
			case R.id.action_quit:
			    this.finish();
			    return true;
			default:
			        //对没有处理的事件，交给父类来处理
			return super.onOptionsItemSelected(item);
	    }
		//返回true表示处理完菜单项的事件，不需要将该事件继续传播下去了
		return true;
	}
	
	
	public static final int WIFI_CONNECTING = 0;
	public static final int WIFI_CONNECTED = 1;
	public static final int WIFI_CONNECT_FAIL = 2;
	public static final int WIFI_NONE = 3;
	public static final int BLUE_CONNECTING = 0;
	public static final int BLUE_CONNECTED = 1;
	public static final int BLUE_CONNECT_FAIL = 2;
	public static final int BLUE_NONE = 3;
	public static final int FAIL_INDEX = 4;
	
	protected void setAnimateState(AnimatedSprite sprite, int state){
		switch(state){
		case WIFI_CONNECTING:
			sprite.setVisible(true);
			long[] dur = {200,200,200,200};
			int[] frames = {0,1,2,3};
			sprite.animate(dur, frames);
			break;
		case WIFI_CONNECTED:
			sprite.setVisible(true);
			sprite.stopAnimation(5);
			break;
		case WIFI_CONNECT_FAIL:
			sprite.setVisible(true);
			sprite.stopAnimation(FAIL_INDEX);
			break;
		case WIFI_NONE:
		default:
			sprite.setVisible(false);
			break;
		}
	}
	
	protected void setWifiState(int state){
		setAnimateState(mWifiState, state);
	}
	
	protected void setBlueState(int state){
		setAnimateState(mBlueState, state);
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch(requestCode){
            case R.id.action_view_settings:
            	if(resultCode == R.id.view_setting_ok){
            		mViewSetting.fromIntent(data);
            		mViewSetting.save(this, PRE_NAME);
            		mBtnGroup.update(this.mViewSetting.mLockAttitude);
            	}
            	break;
            case R.id.action_net_settings:
            	if(resultCode == R.id.net_setting_ok){
            		boolean lastStatus = mNetSetting.mConnectServer;
	            	mNetSetting.fromIntent(data);
	            	mNetSetting.save(this, PRE_NAME);
	            	if(lastStatus != mNetSetting.mConnectServer){
	            		dealConnection();
	            	}
            	}
            	break;
            case R.id.action_blue_settings:
            	if(resultCode == R.id.blue_setting_ok){
            		boolean lastStatus = mBlueSetting.mConnect;
            		mBlueSetting.fromIntent(data);
            		mBlueSetting.save(this, PRE_NAME);
	            	if(lastStatus != mBlueSetting.mConnect){
	            		dealBlueConnection();
	            	}
            	}
            	break;
            case R.id.action_button_settings:
            	if(resultCode == R.id.button_setting_ok){
            		mBtnGroup.getSetting().fromIntent(data);
            		mBtnGroup.getSetting().save(this, PRE_NAME);
            		mBtnGroup.update(this.mViewSetting.mLockAttitude);
            	}
            	break;
            case R.id.action_axis_settings:
            	if(resultCode == R.id.axis_setting_ok){
            		mAxisSetting.fromIntent(data);
            		mAxisSetting.save(this, PRE_NAME);
            	}
            	break;
         }
	}
	
	protected void dealConnection(){
		new Thread(){
			public void run(){
				dealConnection(mNetSetting.mConnectServer);
			}
		}.start();
	}
	
	protected void dealBlueConnection(){
		new Thread(){
			public void run(){
				dealBlueConnection(mBlueSetting.mConnect);
			}
		}.start();
	}
	
	private int mNewWifiState = WIFI_NONE;
	protected void update_wifi_state(int state){
		mNewWifiState = state;
		runOnUiThread(new Runnable(){
			@Override
			public void run() {
				setWifiState(mNewWifiState);
			}
		});
	}
	
	private int mNewBlueState = BLUE_NONE;
	protected void update_blue_state(int state){
		mNewBlueState = state;
		runOnUiThread(new Runnable(){
			@Override
			public void run() {
				setBlueState(mNewBlueState);
			}
		});
	}
	
	private String mToastString;
	protected void display_string(String state){
		mToastString = state;
		runOnUiThread(new Runnable(){
			@Override
			public void run() {
				Toast.makeText(MainActivity.this, mToastString, Toast.LENGTH_LONG).show();
			}
		});
	}
	
	public class ServerMessageReader<C extends Connection> implements IServerMessageReader<C>{

		@Override
		public void registerMessage(short pFlag,
				Class<? extends IServerMessage> pServerMessageClass) {}
		@Override
		public void registerMessage(
				short pFlag,
				Class<? extends IServerMessage> pServerMessageClass,
				IMessageHandler<C, ServerConnector<C>, IServerMessage> pServerMessageHandler) {}
		@Override
		public void registerMessageHandler(
				short pFlag,
				IMessageHandler<C, ServerConnector<C>, IServerMessage> pServerMessageHandler) {	}
		@Override
		public IServerMessage readMessage(DataInputStream pDataInputStream)
				throws IOException {
			byte[] buffer = new byte[64];
			int len = pDataInputStream.read(buffer);
			if(len>0 && mBinaryParser!=null){
				mBinaryParser.onReceivedData(buffer,len);
			}
			return null;
		}
		@Override
		public void handleMessage(ServerConnector<C> pServerConnector,
				IServerMessage pServerMessage) throws IOException {}
		@Override
		public void recycleMessage(IServerMessage pServerMessage) {	}
		
	}
	
	ServerConnector<SocketConnection> mWifiConnection = null;
	protected void dealConnection(boolean connect){
		if(connect){
			try {
				if(mWifiConnection != null){
					mWifiConnection.getConnection().terminate();
				}
				update_wifi_state(WIFI_CONNECTING);
				mWifiConnection = new SocketConnectionServerConnector(new SocketConnection(
						new Socket(mNetSetting.mIP, mNetSetting.mPort)),
						new ServerMessageReader<SocketConnection>(),
						new ISocketConnectionServerConnectorListener(){
							@Override
							public void onStarted(
									ServerConnector<SocketConnection> pServerConnector) {
								Log.i(TAG, "wifi started");
								update_wifi_state(WIFI_CONNECTED);
							}
	
							@Override
							public void onTerminated(
									ServerConnector<SocketConnection> pServerConnector) {
								Log.i(TAG, "wifi Terminated");
								display_string(getString(R.string.net_connect_fail));
								update_wifi_state(WIFI_CONNECT_FAIL);
								if(mWifiConnection != null){
									mWifiConnection.getConnection().terminate();
									mWifiConnection = null;
								}
							}
				});
				Log.i(TAG, "wifi start ...");
				mWifiConnection.getConnection().start();
				Log.i(TAG, "wifi start ..");
			} catch (final Throwable t) {
				Debug.e(t);
				Log.i(TAG, "wifi fail");
				display_string(getString(R.string.net_connect_fail)+"\r\n"+t.toString());
				update_wifi_state(WIFI_CONNECT_FAIL);
			}
		}else{
			if(mWifiConnection != null){
				mWifiConnection.getConnection().terminate();
				mWifiConnection = null;
			}
			update_wifi_state(WIFI_NONE);
		}
	}
	
	void incoming_data(byte[] data){
		Log.i(TAG, "got " + data.length + " bytes data");
	}
	
	ServerConnector<BluetoothSocketConnection> mBlueConnection = null;
	@TargetApi(Build.VERSION_CODES.GINGERBREAD_MR1)
	protected void dealBlueConnection(boolean connect){
		if(connect){
			
			if(mBluetoothAdapter == null){
				display_string(getString(R.string.blue_not_valid));
			}
			
			if(!mBluetoothAdapter.isEnabled()){
				Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				this.startActivityForResult(enableIntent, 0);
			}
			
			try {
				if(mBlueConnection != null){
					mBlueConnection.getConnection().terminate();
				}
				update_blue_state(BLUE_CONNECTING);
				BluetoothDevice blueDev = mBluetoothAdapter.getRemoteDevice(mBlueSetting.mMacAddress);
				if(blueDev.getBondState() == BluetoothDevice.BOND_NONE){
					//update_blue_state(BLUE_CONNECT_FAIL);
					//return;
				}
				BluetoothSocket blueSocket = blueDev.createInsecureRfcommSocketToServiceRecord(UUID.fromString(BlueSettingActivity.BlueSetting.UUID));
				mBluetoothAdapter.cancelDiscovery();
				blueSocket.connect();
				mBlueConnection = new BluetoothSocketConnectionServerConnector(
						new BluetoothSocketConnection(blueSocket),
						new ServerMessageReader<BluetoothSocketConnection>(),
						new IBluetoothSocketConnectionServerConnectorListener(){
							@Override
							public void onStarted(
									ServerConnector<BluetoothSocketConnection> pServerConnector) {
								Log.i(TAG, "blue started");
								update_blue_state(BLUE_CONNECTED);
							}
	
							@Override
							public void onTerminated(
									ServerConnector<BluetoothSocketConnection> pServerConnector) {
								Log.i(TAG, "blue Terminated");
								display_string(getString(R.string.blue_connect_fail));
								update_blue_state(BLUE_CONNECT_FAIL);
								if(mBlueConnection != null){
									mBlueConnection.getConnection().terminate();
									mBlueConnection = null;
								}
							}
				});
				Log.i(TAG, "blue start ...");
				mBlueConnection.getConnection().start();
				Log.i(TAG, "blue start ..");
			} catch (final Throwable t) {
				Debug.e(t);
				Log.i(TAG, "blue fail");
				display_string(getString(R.string.blue_connect_fail)+"\r\n"+t.toString());
				update_blue_state(BLUE_CONNECT_FAIL);
			}
		}else{
			if(mBlueConnection != null){
				mBlueConnection.getConnection().terminate();
				mBlueConnection = null;
			}
			update_blue_state(BLUE_NONE);
		}
	}
	
	public static class FrameMessage implements IClientMessage{
		public static final short FLAG = 13;
		byte[] message = null;
		public FrameMessage(byte[] data){
			message = data.clone();
		}
		@Override
		public short getFlag() {
			return FLAG;
		}
		@Override
		public void read(DataInputStream pDataInputStream) throws IOException {
			// we may not reach here
		}
		@Override
		public void write(DataOutputStream pDataOutputStream)
				throws IOException {
			pDataOutputStream.write(message);
		}
	}
	
	protected boolean mRunning = true;
	protected Object mDataSig = new Object();
	protected Thread mDataProcessor = new Thread(){
		@Override
		public void run() {
			while(mRunning){
				try {
					synchronized(mDataSig){
						mDataSig.wait();
					}
					sendControlState(mX1,mY1,mX2,mY2);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	};
	
	protected void updateControlState(){
		synchronized(mDataSig){
			mDataSig.notify();
		}
	}
	
	
	protected void sendMessge(IClientMessage message){
		if(mWifiConnection != null){
			try {
				mWifiConnection.sendClientMessage(message);
			} catch (IOException e) {
				mWifiConnection.terminate();
				e.printStackTrace();
			}
		}
		if(mBlueConnection != null){
			try {
				mBlueConnection.sendClientMessage(message);
			} catch (IOException e) {
				mBlueConnection.terminate();
				
				// lxyppc 
				// work around code for the AndEngine's blue connection
				// this should be down in the IBluetoothSocketConnectionServerConnectorListener
				// but BluetoothSocketConnection didn't call it
				Log.i(TAG, "blue Terminated");
				display_string(getString(R.string.blue_connect_fail));
				update_blue_state(BLUE_CONNECT_FAIL);
				if(mBlueConnection != null){
					mBlueConnection.getConnection().terminate();
					mBlueConnection = null;
				}
				
				e.printStackTrace();
			}
		}
	}
	
	protected void sendControlState(float x1, float y1, float x2, float y2){
		
		if(mViewSetting.mLockAttitude){
			float x = x1;
			float y = y1;
			float thro = y2;
			if(mViewSetting.mLeftThrottle){
				thro = y1;
				x = x2;
				y = y2;
			}
			x*=100f; y*=100f;
			if(thro>0) thro = 0;
	        else thro = thro*=-1;
			float m_horizontalSensitivity = mViewSetting.sensitivity();
			Quaternion control = new Quaternion();
			control.assign(1f, y*m_horizontalSensitivity, x*m_horizontalSensitivity, 0);
			control.nomalize();
			byte[] param = new byte[21];
			param[0] = Protocol.LockAttitude.CONTROL;
			System.arraycopy(MyMath.toByte(new float[]{thro}),0,param,1,4);
			System.arraycopy(MyMath.toByte(control.value()),0,param,5,16);
			mBinaryParser.control_lockAttitude(param);
		}else{
			mBinaryParser.cmd_raw_control(
					mBtnGroup.value(), 
					(int)(x1*mAxisSetting.mLeftXScale) + mAxisSetting.mLeftXOffset,
					(int)(y1*mAxisSetting.mLeftYScale) + mAxisSetting.mLeftYOffset,
					(int)(x2*mAxisSetting.mRightXScale) + mAxisSetting.mRightXOffset,
					(int)(y2*mAxisSetting.mRightYScale) + mAxisSetting.mRightYOffset);
		}
	}
	
	public void buttonClicked(int idx){
		if(!mViewSetting.mLockAttitude) return;
		switch(idx){
			case ButtonGroup.STOP_BTN_ID:
			{
				mBinaryParser.control_setMode(Protocol.ControlMode.HALT);
			}
			break;
			case ButtonGroup.READY_BTN_ID:
			{
				mBinaryParser.control_setMode(Protocol.ControlMode.LOCK_ATTITUDE);
				byte[] param = new byte[]{ Protocol.LockAttitude.GET_READY };
				mBinaryParser.control_lockAttitude(param);
			}
			break;
		}
	}
	
	@Override
	protected void onDestroy() {
		if(mWifiConnection != null){
			mWifiConnection.getConnection().terminate();
			mWifiConnection = null;
		}
		if(mBlueConnection != null){
			mBlueConnection.getConnection().terminate();
			mBlueConnection = null;
		}
		mRunning = false;
		synchronized(mDataSig){
			mDataSig.notify();
		}
		try {
			mDataProcessor.join(50);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		this.mBtnGroup.getSetting().save(this, PRE_NAME);
		super.onDestroy();
	}
}
