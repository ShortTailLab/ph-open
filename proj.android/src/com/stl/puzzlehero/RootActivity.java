/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org

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
package com.stl.puzzlehero;

import android.content.Intent;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.view.KeyEvent;
import android.media.AudioManager;

import com.umeng.analytics.MobclickAgent;

public class RootActivity extends Cocos2dxActivity
{
    public static RootActivity ctx;
    public Runnable onPauseRun, onResumeRun, onDestroyRun, onExitRun;

    public static boolean UMENG_ENABLED = true;

    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        ctx = this;

        Log.d("puzzlehero", "* PuzzleHero RootActivity onCreate.");

        try
        {
            ApplicationInfo ai = this.getPackageManager().getApplicationInfo(this.getPackageName(), PackageManager.GET_META_DATA);
        }
        catch(Exception e)
        {
            Log.d("puzzlehero", "Cannot read metadata info: " + e.getMessage());
        }
    }

    public static RootActivity getContext()
    {
        return ctx;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        AudioManager mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        int currentVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);

        switch (keyCode) {
            case KeyEvent.KEYCODE_VOLUME_UP:// 音量增大
                mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, currentVolume+1, 1);
                break;
            case KeyEvent.KEYCODE_VOLUME_DOWN:// 音量减小
                mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, currentVolume-1, 1);
                break;
            default:
                return super.onKeyDown(keyCode, event);
        }

        return true;
    }

    @Override
    public void onBackPressed()
    {
        CCNative.promptForExit();
    }

    public Cocos2dxGLSurfaceView onCreateView()
    {
        Cocos2dxGLSurfaceView glView = super.onCreateView();
        glView.setEGLContextClientVersion(2);
        return glView;
    }

    @Override
    public void onLowMemory()
    {
        Log.d("puzzlehero", "!!! Received low memory warning!");
        super.onLowMemory();
        CCNative.freeUnusedTextures();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        if (CCNative.mSSOHandler != null)
        {
            CCNative.mSSOHandler.authorizeCallBack(requestCode, resultCode, data);
        }
    }

    @Override
    protected void onPause()
    {
        Log.d("puzzlehero", "on pause called");
        super.onPause();

        if(UMENG_ENABLED)
            MobclickAgent.onPause(this);

        if (onPauseRun != null)
            onPauseRun.run();
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        if(UMENG_ENABLED)
            MobclickAgent.onResume(this);

        if (onResumeRun != null)
            onResumeRun.run();

        Cocos2dxGLSurfaceView.getInstance().setFocusable(true);
        Cocos2dxGLSurfaceView.getInstance().requestFocus();
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        Log.d("puzzlehero", "RootActivity onDestory...");

        Cocos2dxGLSurfaceView.clearSurfaceReferences();
        if (onDestroyRun != null)
            onDestroyRun.run();
    }

    static
    {
        System.loadLibrary("game");
    }
}
