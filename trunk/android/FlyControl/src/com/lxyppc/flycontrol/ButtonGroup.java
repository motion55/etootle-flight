package com.lxyppc.flycontrol;

import org.andengine.entity.scene.Scene;
import org.andengine.entity.sprite.AnimatedSprite;
import org.andengine.entity.text.Text;
import org.andengine.input.touch.TouchEvent;
import org.andengine.opengl.font.Font;
import org.andengine.opengl.texture.atlas.bitmap.BitmapTextureAtlasTextureRegionFactory;
import org.andengine.opengl.texture.atlas.bitmap.BuildableBitmapTextureAtlas;
import org.andengine.opengl.texture.region.ITiledTextureRegion;
import org.andengine.opengl.texture.region.TiledTextureRegion;
import org.andengine.opengl.vbo.VertexBufferObjectManager;

import android.content.Context;

public class ButtonGroup {
	private TiledTextureRegion mButtonTextureRegion;
	private ButtonSettingActivity.ButtonSetting mSetting;
	private AnimatedButton[] mBtnSprites = null;
	private Text[] mTexts = null;
	private float mTextOffsetY = -16.0f;
	private int mValue = 0;
	private Context context = null;
	private ButtonClicked buttonListener = null;
	public static final int STOP_BTN_ID = 0;
	public static final int READY_BTN_ID = 1;
	
	public ButtonGroup(Context ctx){
		mSetting = new ButtonSettingActivity.ButtonSetting();
		mBtnSprites = new AnimatedButton[mSetting.total_count()];
		mTexts = new Text[mSetting.total_count()];
		context = ctx;
	}
	
	public void restore(Context context, String preferencesName){
		mSetting.restore(context, preferencesName);
		mValue = 0;
		for(int i=0;i<mSetting.mButtonSettings.length;i++){
			int state = 0;
			if(mSetting.mButtonSettings[i].mEnable && (mSetting.mButtonSettings[i].state != 0) )
				state = 1;
			mValue |= (state << i);
		}
	}
	
	public void setButtonListener(ButtonClicked listener){
		buttonListener = listener;
	}
	
	public void loadResources(BuildableBitmapTextureAtlas atlas, Context context){
		mButtonTextureRegion = BitmapTextureAtlasTextureRegionFactory.createTiledFromAsset(atlas, context, "button_blue.png", 2, 1);
	}
	
	public void onCreateScene(Scene scene, int width, int height, Font font, VertexBufferObjectManager manager){
		float y = 40.f;
		float dy = mButtonTextureRegion.getHeight() + 16.0f;;
		float dx = mButtonTextureRegion.getWidth() + 16.0f;
		float x = 10.0f;
		float cur_x = x;
		float cur_y = y;
		for(int i=0;i<mSetting.total_count();i++){
			float tx = mSetting.mButtonSettings[i].x;
			float ty = mSetting.mButtonSettings[i].y;
			if(mSetting.mButtonSettings[i].is_float()){
				if(cur_x + dx > width){
					cur_y += dy;
					cur_x = x;
				}
				tx = cur_x;
				ty = cur_y;
				cur_x += dx;
			}
			mTexts[i] = new Text(tx,ty+mTextOffsetY,font,mSetting.mButtonSettings[i].value,64,manager);
			mTexts[i].setVisible(mSetting.mButtonSettings[i].value.length()>0 && mSetting.mButtonSettings[i].mEnable);
			mBtnSprites[i] = new AnimatedButton(i,tx,ty,mButtonTextureRegion,manager);
			mBtnSprites[i].setVisible(mSetting.mButtonSettings[i].mEnable);
			mBtnSprites[i].stopAnimation(mSetting.mButtonSettings[i].state);
			scene.attachChild(mTexts[i]);
			scene.attachChild(mBtnSprites[i]);
			scene.registerTouchArea(mBtnSprites[i]);
		}
	}
	
	public class AnimatedButton extends AnimatedSprite{
		private int mIdx;
		public AnimatedButton(int idx, float pX, float pY,
				ITiledTextureRegion pTiledTextureRegion,
				VertexBufferObjectManager pVertexBufferObjectManager) {
			super(pX, pY, pTiledTextureRegion, pVertexBufferObjectManager);
			mIdx = idx;
		}
		
		public boolean onAreaTouched(final TouchEvent pSceneTouchEvent, final float pTouchAreaLocalX, final float pTouchAreaLocalY) {
			if(!mBtnSprites[mIdx].isVisible()) return true;
			if(mSetting.moveable()){
				float x = pSceneTouchEvent.getX() - this.getWidth() / 2;
				float y = pSceneTouchEvent.getY() - this.getHeight() / 2;
				this.setPosition(x, y);
				mTexts[mIdx].setPosition(x, y + mTextOffsetY);
				mSetting.mButtonSettings[mIdx].x = (int)x;
				mSetting.mButtonSettings[mIdx].y = (int)y;
			}else{
				if(pSceneTouchEvent.isActionDown()){
					mSetting.mButtonSettings[mIdx].toggle();
					mValue &= (~(1<<mIdx));
					mValue |= (mSetting.mButtonSettings[mIdx].state<<mIdx);
					stopAnimation(mSetting.mButtonSettings[mIdx].state);
					
					if( (buttonListener!=null) && (mSetting.mButtonSettings[mIdx].state==1) ){
						buttonListener.buttonClicked(mIdx);
					}
				}
			}
			return true;
		}
	}
	
	public ButtonSettingActivity.ButtonSetting getSetting(){
		return mSetting;
	}

	public void update(boolean lockAttitude){
		int start = 0;
		if(lockAttitude){
			start = 2;
			mTexts[STOP_BTN_ID].setText(context.getString(R.string.btn_stop));
			mTexts[STOP_BTN_ID].setVisible(true);
			mBtnSprites[STOP_BTN_ID].setVisible(true);
			mTexts[READY_BTN_ID].setText(context.getString(R.string.btn_ready));
			mTexts[READY_BTN_ID].setVisible(true);
			mBtnSprites[READY_BTN_ID].setVisible(true);
		}
		for(int i=start;i<mTexts.length;i++){
			mTexts[i].setText(mSetting.mButtonSettings[i].value);
			mTexts[i].setVisible(mSetting.mButtonSettings[i].value.length()>0 && mSetting.mButtonSettings[i].mEnable);
			mBtnSprites[i].setVisible(mSetting.mButtonSettings[i].mEnable);
		}
	}
	
	public int value(){
		return mValue;
	}
	
	public static interface ButtonClicked{
		public void buttonClicked(int idx); 
	}
}
