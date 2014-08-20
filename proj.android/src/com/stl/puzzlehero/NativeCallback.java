package com.stl.puzzlehero;

import android.util.Log;

/**
 * Created with IntelliJ IDEA.
 * User: colprog
 * Date: 12-11-27
 * Time: 下午9:10
 * To change this template use File | Settings | File Templates.
 */
public class NativeCallback implements Runnable {
    public int nativeCallbackId;

    public NativeCallback(int cbId){
        this.nativeCallbackId = cbId;
    }

    @Override
    public void run() {
        try
        {
            call(nativeCallbackId, null);
        }
        catch (Exception e)
        {
            Log.d("puzzlehero", e.toString());
        }
    }

    public void run(String arg0, String arg1)
    {
        try
        {
            call(nativeCallbackId, new Object[]{arg0, arg1});
        }
        catch (Exception e)
        {
            Log.d("puzzlehero", e.toString());
        }
    }

    public void run(int arg0, String arg1)
    {
        try
        {
            call(nativeCallbackId, new Object[]{arg0, arg1});
        }
        catch (Exception e)
        {
            Log.d("puzzlehero", e.toString());
        }
    }

    private native void call(int id, Object[] args);
}
