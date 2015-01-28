/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.lib;

import org.cocos2dx.lib.Cocos2dxHelper.Cocos2dxHelperListener;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.os.PowerManager;
import android.view.ViewGroup;
import android.util.Log;
import android.widget.FrameLayout;
import android.preference.PreferenceManager.OnActivityResultListener;

public abstract class Cocos2dxActivity extends Activity implements Cocos2dxHelperListener {
	// ===========================================================
	// Constants
	// ===========================================================
    protected PowerManager pm;
	private final static String TAG = Cocos2dxActivity.class.getSimpleName();

	// ===========================================================
	// Fields
	// ===========================================================
	
	private Cocos2dxGLSurfaceView mGLSurfaceView;
    private int[] glContextAttrs;
	private Cocos2dxHandler mHandler;	
	private static Cocos2dxActivity sContext = null;
	private Cocos2dxVideoHelper mVideoHelper = null;

	protected boolean mHiddenCycle = false;
    protected BroadcastReceiver mScreenReceiver = null;
    protected boolean mScreenWasOn = false;

	public static Context getContext() {
		return sContext;
	}
	
	protected void onLoadNativeLibraries() {
		try {
			ApplicationInfo ai = getPackageManager().getApplicationInfo(getPackageName(), PackageManager.GET_META_DATA);
			Bundle bundle = ai.metaData;
			String libName = bundle.getString("android.app.lib_name");
    		System.loadLibrary(libName);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	// ===========================================================
	// Constructors
	// ===========================================================
	
	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

        // initialize receiver
        final IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        mScreenReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(final Context context, final Intent intent) {
                if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF)) {
                    Log.e(TAG,"onScreenActionReceived OFF");
                    // do whatever you need to do here
                    mScreenWasOn = false;
                    Cocos2dxHelper.onEnterBackground();
                } else if (intent.getAction().equals(Intent.ACTION_SCREEN_ON)) {
                    Log.e(TAG,"onScreenActionReceived ON");
                    // and do whatever you need to do here
                    mScreenWasOn = true;
                    //we should do it on resume to make sure the activity is visible ( not behind a lock screen )
                    //Cocos2dxHelper.onEnterForeground();
                }
            }
        };

        registerReceiver(mScreenReceiver, filter);

        //we want to initialize cocos only if screen is On
        pm = (PowerManager)getSystemService(POWER_SERVICE);

        onLoadNativeLibraries();

        sContext = this;
        this.mHandler = new Cocos2dxHandler(this);

        Cocos2dxHelper.init(this);

        if(pm.isScreenOn()) {
            this.glContextAttrs = getGLContextAttrs();
            this.init();
            if (mVideoHelper == null) {
                mVideoHelper = new Cocos2dxVideoHelper(this, mFrameLayout);
            }

            mHiddenCycle = false; // we assume the app cannot start when the screen is off.
        }
        else
        {
            mHiddenCycle = true;
        }
	}

    //native method,call GLViewImpl::getGLContextAttrs() to get the OpenGL ES context attributions
    private static native int[] getGLContextAttrs();

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	protected void onResume() {
		super.onResume();

        if (! mHiddenCycle) {
            Cocos2dxHelper.onResume();
            Cocos2dxHelper.onEnterForeground();
        }
	}

	@Override
	protected void onPause() {
        if ( !mHiddenCycle) {
            Cocos2dxHelper.onPause();
        }
        super.onPause();
	}

    @Override
    public void onWindowFocusChanged(boolean focus){
        Log.d(TAG,"onWindowFocusChanged");
        if ( ! mHiddenCycle) {
            if (focus) {
                this.mGLSurfaceView.onResume();
            } else {
                this.mGLSurfaceView.onPause();
            }
        }
    }
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	public void showDialog(final String pTitle, final String pMessage) {
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_SHOW_DIALOG;
		msg.obj = new Cocos2dxHandler.DialogMessage(pTitle, pMessage);
		this.mHandler.sendMessage(msg);
	}

	@Override
	public void showEditTextDialog(final String pTitle, final String pContent, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength) { 
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_SHOW_EDITBOX_DIALOG;
		msg.obj = new Cocos2dxHandler.EditBoxMessage(pTitle, pContent, pInputMode, pInputFlag, pReturnType, pMaxLength);
		this.mHandler.sendMessage(msg);
	}
	
	@Override
	public void runOnGLThread(final Runnable pRunnable) {
		this.mGLSurfaceView.queueEvent(pRunnable);
	}
	
	@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        for (OnActivityResultListener listener : Cocos2dxHelper.getOnActivityResultListeners()) {
            listener.onActivityResult(requestCode, resultCode, data);
        }

        super.onActivityResult(requestCode, resultCode, data);
    }


	protected FrameLayout mFrameLayout = null;
	// ===========================================================
	// Methods
	// ===========================================================
	public void init() {
		
    	// FrameLayout
        ViewGroup.LayoutParams framelayout_params =
            new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                       ViewGroup.LayoutParams.MATCH_PARENT);
        mFrameLayout = new FrameLayout(this);
        mFrameLayout.setLayoutParams(framelayout_params);

        // Cocos2dxEditText layout
        ViewGroup.LayoutParams edittext_layout_params =
            new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                       ViewGroup.LayoutParams.WRAP_CONTENT);
        Cocos2dxEditText edittext = new Cocos2dxEditText(this);
        edittext.setLayoutParams(edittext_layout_params);

        // ...add to FrameLayout
        mFrameLayout.addView(edittext);

        // Cocos2dxGLSurfaceView
        this.mGLSurfaceView = this.onCreateView();

        // ...add to FrameLayout
        mFrameLayout.addView(this.mGLSurfaceView);

        // Switch to supported OpenGL (ARGB888) mode on emulator
        if (isAndroidEmulator())
           this.mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        Cocos2dxRenderer renderer = new Cocos2dxRenderer();
        renderer.setPowerManager((PowerManager)getSystemService(POWER_SERVICE));
        this.mGLSurfaceView.setCocos2dxRenderer(renderer);
        this.mGLSurfaceView.setCocos2dxEditText(edittext);



        // Set framelayout as the content view
		setContentView(mFrameLayout);
	}
	
    public Cocos2dxGLSurfaceView onCreateView() {

        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
        glSurfaceView.setEGLConfigChooser(this.glContextAttrs[0], this.glContextAttrs[1],this.glContextAttrs[2],
            this.glContextAttrs[3],this.glContextAttrs[4],this.glContextAttrs[5]);

        return glSurfaceView;
    }

   private final static boolean isAndroidEmulator() {
      String model = Build.MODEL;
      Log.d(TAG, "model=" + model);
      String product = Build.PRODUCT;
      Log.d(TAG, "product=" + product);
      boolean isEmulator = false;
      if (product != null) {
         isEmulator = product.equals("sdk") || product.contains("_sdk") || product.contains("sdk_");
      }
      Log.d(TAG, "isEmulator=" + isEmulator);
      return isEmulator;
   }

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
