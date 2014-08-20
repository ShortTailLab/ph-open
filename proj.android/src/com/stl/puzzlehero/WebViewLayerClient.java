package com.stl.puzzlehero;

import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.os.Build;
import android.util.Log;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.net.http.SslError;
import android.webkit.SslErrorHandler;
import android.widget.Toast;
import android.app.AlertDialog;

/**
 * Created with IntelliJ IDEA.
 * User: colprog
 * Date: 12-11-28
 * Time: 下午1:39
 * To change this template use File | Settings | File Templates.
 */
public class WebViewLayerClient extends WebViewClient {
    public NativeCallback shouldOverrideUrlLoadingNative;
    public boolean shouldOverrideUrlLoadingNativeRet;

    private String lastURL = null;

    @Override
    public boolean shouldOverrideUrlLoading(WebView view, java.lang.String url)
    {
        if (lastURL == null || !lastURL.equals(url))
        {
            shouldOverrideUrlLoadingNative.run(url, "");
            lastURL = url;
        }
        return shouldOverrideUrlLoadingNativeRet;
    }

    @Override
    public void onPageStarted(WebView view, String url, Bitmap favicon)
    {
        super.onPageStarted(view, url, favicon);

        Log.d("puzzlehero", "onPageStarted: " + url);

        // 11 = Build.VERSION_CODES.HONEYCOMB (Android 3.0)
        if (Build.VERSION.SDK_INT < 11)
        {
            // According to this page:
            //
            // http://www.catchingtales.com/android-webview-shouldoverrideurlloading-and-redirect/416/
            //
            // shouldOverrideUrlLoading() is not called for redirects on
            // Android earlier than 3.0, so call the method manually.

            Log.d("puzzlehero", "SDK version < 11");
            if (shouldOverrideUrlLoading(view, url)) {
                Log.d("puzzlehero", "stopped loading");
                view.stopLoading();
            }
        }
    }

    @Override
    public void onReceivedError(WebView view, int errorCode, String description, String failingUrl)
    {
        super.onReceivedError(view, errorCode, description, failingUrl);

        Log.d("puzzlehero", "onReceivedError: " + errorCode + ", " + description + " / " + failingUrl);
        Toast.makeText(RootActivity.getContext(), description, 3);
    }

    @Override
    public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error)
    {
        super.onReceivedSslError(view, handler, error);
        Log.d("puzzlehero", "onReceivedSslError: " + error.toString());

        new AlertDialog.Builder(RootActivity.getContext())
                .setTitle("SSL错误")
                .setMessage(error.toString())
                .setPositiveButton("确定", null)
                .show();
    }
}
