package com.stl.puzzlehero;

import android.app.Activity;
import android.content.*;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.pm.Signature;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.*;
import android.widget.Toast;
import android.app.AlertDialog;
import android.text.TextUtils;
import android.util.Log;
import android.net.Uri;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import org.apache.commons.io.FileExistsException;
import org.apache.commons.io.FileUtils;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.lang.Runnable;

import android.content.res.AssetManager;
import java.io.InputStream;
import java.io.OutputStream;

import android.util.DisplayMetrics;
import android.app.ActivityManager.MemoryInfo;
import android.app.ActivityManager;

import com.sina.sso.RemoteSSO;
import com.weibo.sdk.android.Oauth2AccessToken;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created with IntelliJ IDEA.
 * User: colprog
 * Date: 12-11-25
 * Time: 下午9:41
 * To change this template use File | Settings | File Templates.
 */
public class CCNative {

    public static native void freeUnusedTextures();
    public static native void endDirector();

    public static String getLocalIpAddress() {
        try {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements(); ) {
                NetworkInterface intf = en.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if (!inetAddress.isLoopbackAddress()) {
                        return inetAddress.getHostAddress();
                    }
                }
            }
        } catch (Exception ex) {
            Log.d("puzzlehero", ex.toString());
        }
        return "127.0.0.1";
    }

    public static String getOSVersion()
    {
        return Build.VERSION.RELEASE;
    }

    public static String getLocalMACAddress() {
        try {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements(); )
            {
                NetworkInterface intf = en.nextElement();
                if (!intf.isLoopback())
                {
                    try
                    {
                        byte[] address = intf.getHardwareAddress();
                        if (address != null)
                        {
                            StringBuilder builder = new StringBuilder();
                            for (int i = 0; i < address.length; i++)
                            {
                                builder.append(String.format("%02X", address[i]));
                                if (i < address.length - 1)
                                    builder.append("-");
                            }
                            return builder.toString();
                        }
                    }
                    catch (SocketException e)
                    {
                    }
                }
            }
        } catch (Exception ex) {
            Log.d("puzzlehero", ex.toString());
        }
        return "";
    }

    public static boolean isNetworkAvailable() {
        Context context = RootActivity.ctx;

        boolean haveConnectedWifi = false;
        boolean haveConnectedMobile = false;
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo[] netInfo = cm.getAllNetworkInfo();
        for (NetworkInfo ni : netInfo) {
            if (ni.getTypeName().equalsIgnoreCase("WIFI"))
                haveConnectedWifi = ni.isConnected();
            if (ni.getTypeName().equalsIgnoreCase("MOBILE"))
                haveConnectedMobile = ni.isConnected();
        }
        return haveConnectedWifi || haveConnectedMobile;
    }

    public static int getResolutionWidth()
    {
        DisplayMetrics dm = new DisplayMetrics();
        RootActivity.ctx.getWindowManager().getDefaultDisplay().getMetrics(dm);
        return dm.widthPixels;
    }

    public static int getResolutionHeight()
    {
        DisplayMetrics dm = new DisplayMetrics();
        RootActivity.ctx.getWindowManager().getDefaultDisplay().getMetrics(dm);
        return dm.heightPixels;
    }

    public static boolean isWIFIAvailable() {
        Context context = RootActivity.ctx;

        boolean haveConnectedWifi = false;
        boolean haveConnectedMobile = false;
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo[] netInfo = cm.getAllNetworkInfo();
        for (NetworkInfo ni : netInfo) {
            if (ni.getTypeName().equalsIgnoreCase("WIFI"))
                haveConnectedWifi = ni.isConnected();
            if (ni.getTypeName().equalsIgnoreCase("MOBILE"))
                haveConnectedMobile = ni.isConnected();
        }
        return haveConnectedWifi;
    }

    public static void moveFile(String srcPath, String dstPath)
    {
        File fileSrc = new File(srcPath);
        File fileTarget = new File(dstPath);

        try
        {
            fileTarget.delete();
            FileUtils.moveFile(fileSrc, fileTarget);
        }
        catch (IOException e)
        {
            Log.d("puzzlehero", "Move file exception: " + e.toString());
        }
    }

    public static void copyAssetToPath(String srcPath, String dstPath)
    {
        try
        {
            //complete path to target file
            File fileTarget = new File(dstPath);

            //data source stream
            AssetManager assetManager = RootActivity.ctx.getAssets();
            InputStream istr = assetManager.open(srcPath);

            FileUtils.copyInputStreamToFile(istr, fileTarget);
        }
        catch(Exception e)
        {
            Toast.makeText(RootActivity.ctx.getContext(),
                    "File-Copy Error, from " + srcPath+ "to" + dstPath,
                    Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }

    public static void openURL(String url)
    {
        RootActivity.getContext().startActivity(
                new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
    }

    public static boolean isExternalStorageUsable()
    {
        boolean externalStorageAvailable = false;
        boolean externalStorageWriteable = false;
        String state = Environment.getExternalStorageState();

        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // We can read and write the media
            externalStorageAvailable = externalStorageWriteable = true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            // We can only read the media
            externalStorageAvailable = true;
            externalStorageWriteable = false;
        } else {
            // Something else is wrong. It may be one of many other states, but all we need
            //  to know is we can neither read nor write
            externalStorageAvailable = externalStorageWriteable = false;
        }

        return externalStorageAvailable && externalStorageWriteable;
    }

    public static boolean isInstalledOnExternalStorage()
    {
        Context context = RootActivity.getContext();
        PackageManager pm = context.getPackageManager();
        try {
            PackageInfo pi = pm.getPackageInfo(context.getPackageName(), 0);
            ApplicationInfo ai = pi.applicationInfo;
            return (ai.flags & ApplicationInfo.FLAG_EXTERNAL_STORAGE) == ApplicationInfo.FLAG_EXTERNAL_STORAGE;
        } catch (NameNotFoundException e) {
            Log.d("puzzlehero", "Failed to : " + e.getMessage());
        }
        return false;
    }

    public static String getExternalDocPath()
    {
        File f = RootActivity.getContext().getExternalFilesDir(null);
        return f == null ? "" : f.getAbsolutePath() + "/";
    }

    public static String getExternalCachePath()
    {
        File f = RootActivity.getContext().getExternalCacheDir();
        return f == null ? "" : f.getAbsolutePath() + "/";
    }

    public static String getInternalDocPath()
    {
        File f = RootActivity.getContext().getFilesDir();
        return f.getAbsolutePath() + "/";
    }

    public static boolean fileExists(String path)
    {
        if(path.startsWith("assets/"))
        {
            try
            {
                return RootActivity.getContext().getAssets().open(path.substring(7)) != null;
            }
            catch (IOException e)
            {
                return false;
            }
        }
        else
        {
            File f = new File(path);
            return f != null && f.exists();
        }
    }

    public static String getInternalCachePath()
    {
        File f = RootActivity.getContext().getCacheDir();
        return f.getAbsolutePath() + "/";
    }

    public static void reportMemoryUsage()
    {
        ActivityManager am = (ActivityManager) RootActivity.getContext().getSystemService(RootActivity.ACTIVITY_SERVICE);
        MemoryInfo info = new MemoryInfo();
        am.getMemoryInfo(info);

        float M = 1024 * 1024;
        Log.d("puzzlehero", "available memory: " + info.availMem/M + ", threshold: " + info.threshold/M + ", is low: " + info.lowMemory);
    }

    public static void createAlert(final String title, final String message, final String buttonText)
    {
        RootActivity.ctx.runOnUiThread(new Runnable()
        {
            public void run() {
                AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(RootActivity.getContext());

                // set title
                alertDialogBuilder.setTitle(title);

                // set dialog message
                alertDialogBuilder
                        .setMessage(message)
                        .setCancelable(false)
                        .setPositiveButton(buttonText, new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                RootActivity.ctx.finish();
                            }
                        });

                // create alert dialog
                AlertDialog alertDialog = alertDialogBuilder.create();

                // show it
                alertDialog.show();
            }
        });
    }

    public static void promptForExit()
    {
        RootActivity.ctx.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.d("puzzlehero", "prompt for exit");
                AlertDialog alertDialog = new AlertDialog.Builder(RootActivity.ctx).create();
                alertDialog.setTitle("退出游戏");
                alertDialog.setMessage("是否退出游戏？战斗中的进度会遗失噢。");

                alertDialog.setButton(DialogInterface.BUTTON_POSITIVE, "确定", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        //RootActivity.ctx.finish();     // this does not release NDK stuff
                        //System.exits(0)   // this does not exit cleanly
                        if(RootActivity.ctx.onExitRun != null)
                            RootActivity.ctx.onExitRun.run();
                        else
                        {
                            RootActivity.ctx.onDestroy();
                            CCNative.endDirector();
                        }
                    }
                });

                alertDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "取消", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                    }
                });

                alertDialog.show();
            }
        });
    }

    public static void focusGLView()
    {
        Log.d("puzzlehero", "focus GL view");
        Cocos2dxGLSurfaceView.getInstance().setFocusable(true);
        Cocos2dxGLSurfaceView.getInstance().requestFocus();
    }

    public static void exitGame()
    {
        RootActivity.ctx.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(RootActivity.ctx.onExitRun != null)
                    RootActivity.ctx.onExitRun.run();
                else
                {
                    RootActivity.ctx.onDestroy();
                    CCNative.endDirector();
                }
            }
        });
    }

    public static long getInternalFreeSpaceInMB()
    {
        //StatFs statFs = new StatFs(Environment.getRootDirectory().getAbsolutePath());
        StatFs statFs = new StatFs(getInternalDocPath());
        long Free  = (statFs.getAvailableBlocks() *  (long) statFs.getBlockSize()) / 1048576;
        return Free;
    }

    public static long getExternalFreeSpaceInMB()
    {
        if(getExternalDocPath() == "")
            return 0;
        else
        {
            StatFs statFs = new StatFs(getExternalDocPath());
            long Free  = (statFs.getAvailableBlocks() *  (long) statFs.getBlockSize()) / 1048576;
            return Free;
        }
    }

    //--------------------------
    // SSO Weibo Login
    //--------------------------
    public static boolean ssoWeiboLogin(String appKey, String appRedirect, NativeCallback nativeCB)
    {
        if (mSSOHandler == null)
        {
            mSSOCB = nativeCB;
            mSSOHandler = new SSOHandler(RootActivity.ctx, appKey, appRedirect);
            return mSSOHandler.authorize();
        }
        else
            return false;
    }
    private static NativeCallback mSSOCB = null;
    public static String mSSORet = null;
    public static SSOHandler mSSOHandler = null;

    private static void ssoCBWithResult(String result)
    {
        if (mSSOCB != null)
        {
            mSSORet = result;
            mSSOCB.run();

            mSSOCB = null;
            mSSORet = null;
        }
    }

    private static void ssoCBWithCancel()
    {
        JSONObject obj = new JSONObject();
        String ret = "";
        try
        {
            obj.put("sso_error_user_cancelled", true);
            ret = obj.toString();
        }
        catch (JSONException e)
        {

        }
        ssoCBWithResult(ret);
    }

    private static void ssoCBWithError(int errorCode, String error, String description)
    {
        JSONObject obj = new JSONObject();
        String ret = "";
        try
        {
            obj.put("error_code", errorCode);
            obj.put("error", error);
            obj.put("desc", description);
            ret = obj.toString();
        }
        catch (JSONException e)
        {

        }
        ssoCBWithResult(ret);
    }

    private static void ssoCBWithSucceeded(Bundle values)
    {
        JSONObject obj = new JSONObject();
        String ret = "";
        try
        {
            obj.put("access_token", values.getString("access_token"));
            obj.put("expires_in", values.getString("expires_in"));
            obj.put("uid", values.getString("uid"));
            ret = obj.toString();
        }
        catch (JSONException e)
        {

        }
        ssoCBWithResult(ret);
    }

    private static void ssoCBWithAttemptFailed()
    {
        JSONObject obj = new JSONObject();
        String ret = "";
        try
        {
            obj.put("sso_error_attempt_failed", true);
            ret = obj.toString();
        }
        catch (JSONException e)
        {

        }
        ssoCBWithResult(ret);
    }

    protected static class SSOHandler
    {
        private static final int DEFAULT_AUTH_ACTIVITY_CODE = 32973;
        private static final String WEIBO_SIGNATURE = "30820295308201fea00302010202044b4ef1bf300d"
                + "06092a864886f70d010105050030818d310b300906035504061302434e3110300e0603550408130"
                + "74265694a696e673110300e060355040713074265694a696e67312c302a060355040a132353696e"
                + "612e436f6d20546563686e6f6c6f677920284368696e612920436f2e204c7464312c302a0603550"
                + "40b132353696e612e436f6d20546563686e6f6c6f677920284368696e612920436f2e204c746430"
                + "20170d3130303131343130323831355a180f32303630303130323130323831355a30818d310b300"
                + "906035504061302434e3110300e060355040813074265694a696e673110300e0603550407130742"
                + "65694a696e67312c302a060355040a132353696e612e436f6d20546563686e6f6c6f67792028436"
                + "8696e612920436f2e204c7464312c302a060355040b132353696e612e436f6d20546563686e6f6c"
                + "6f677920284368696e612920436f2e204c746430819f300d06092a864886f70d010101050003818"
                + "d00308189028181009d367115bc206c86c237bb56c8e9033111889b5691f051b28d1aa8e42b66b7"
                + "413657635b44786ea7e85d451a12a82a331fced99c48717922170b7fc9bc1040753c0d38b4cf2b2"
                + "2094b1df7c55705b0989441e75913a1a8bd2bc591aa729a1013c277c01c98cbec7da5ad7778b2fa"
                + "d62b85ac29ca28ced588638c98d6b7df5a130203010001300d06092a864886f70d0101050500038"
                + "181000ad4b4c4dec800bd8fd2991adfd70676fce8ba9692ae50475f60ec468d1b758a665e961a3a"
                + "edbece9fd4d7ce9295cd83f5f19dc441a065689d9820faedbb7c4a4c4635f5ba1293f6da4b72ed3"
                + "2fb8795f736a20c95cda776402099054fccefb4a1a558664ab8d637288feceba9508aa907fc1fe2"
                + "b1ae5a0dec954ed831c0bea4";
        public static final String KEY_TOKEN = "access_token";
        public static final String KEY_EXPIRES = "expires_in";
        public static final String KEY_REFRESHTOKEN = "refresh_token";

        private ServiceConnection conn = null;
        private Activity mAuthActivity;
        private String mAppKey = null;
        private String mRedirectURL = null;
        private Oauth2AccessToken mAccessToken = null;
        private int mAuthActivityCode;
        private static String ssoPackageName = "";// "com.sina.weibo";
        private static String ssoActivityName = "";// "com.sina.weibo.MainTabActivity";

        public SSOHandler(Activity activity, String appKey, String redirectURL) {
            mAuthActivity = activity;
            mAuthActivityCode = 32973;
            mAppKey = appKey;
            mRedirectURL = redirectURL;
            conn = new ServiceConnection() {
                @Override
                public void onServiceDisconnected(ComponentName name)
                {
                    Log.d("puzzlehero", "onServiceDisconnected");
//                    mWeibo.startAuthDialog(mAuthActivity, mAuthDialogListener);
                    CCNative.ssoCBWithAttemptFailed();
                }

                @Override
                public void onServiceConnected(ComponentName name, IBinder service)
                {
                    RemoteSSO remoteSSOservice = RemoteSSO.Stub.asInterface(service);
                    try
                    {
                        ssoPackageName = remoteSSOservice.getPackageName();
                        ssoActivityName = remoteSSOservice.getActivityName();
                        boolean singleSignOnStarted = startSingleSignOn(
                                mAuthActivity, mAppKey, new String[]{},
                                mAuthActivityCode);
                        if (!singleSignOnStarted)
                        {
                            Log.d("puzzlehero", "singleSignOn not started");
                            CCNative.ssoCBWithAttemptFailed();
//                        startDialogAuth(mAuthActivity, new String[]{});
//                            mWeibo.startAuthDialog(mAuthActivity, mAuthDialogListener);
                        }
                    }
                    catch (RemoteException e)
                    {
                        e.printStackTrace();
                    }
                }
            };
        }

        public boolean authorize()
        {
            return bindRemoteSSOService(mAuthActivity);
        }

        private boolean bindRemoteSSOService(Activity activity) {
            Context context = activity.getApplicationContext();
            Intent intent = new Intent("com.sina.weibo.remotessoservice");
            return context.bindService(intent, conn, Context.BIND_AUTO_CREATE);
        }

        private boolean startSingleSignOn(Activity activity, String applicationId,
                                          String[] permissions, int activityCode)
        {
            boolean didSucceed = true;
            Intent intent = new Intent();
            intent.setClassName(ssoPackageName, ssoActivityName);
            intent.putExtra("appKey", applicationId);// applicationId //"2745207810"
            intent.putExtra("redirectUri", mRedirectURL);

            if (permissions.length > 0) {
                intent.putExtra("scope", TextUtils.join(",", permissions));
            }

            // validate Signature
            if (!validateAppSignatureForIntent(activity, intent)) {
                return false;
            }

            try {
                activity.startActivityForResult(intent, activityCode);
            } catch (ActivityNotFoundException e) {
                didSucceed = false;
            }

            activity.getApplication().unbindService(conn);
            return didSucceed;
        }

        private boolean validateAppSignatureForIntent(Activity activity, Intent intent)
        {
            ResolveInfo resolveInfo = activity.getPackageManager().resolveActivity(intent, 0);
            if (resolveInfo == null) {
                return false;
            }

            String packageName = resolveInfo.activityInfo.packageName;
            try
            {
                PackageInfo packageInfo = activity.getPackageManager()
                        .getPackageInfo(packageName, PackageManager.GET_SIGNATURES);
                for (Signature signature : packageInfo.signatures)
                {
                    if (WEIBO_SIGNATURE.equals(signature.toCharsString()))
                    {
                        return true;
                    }
                }
            }
            catch (PackageManager.NameNotFoundException e)
            {
                return false;
            }

            return false;
        }

        public void authorizeCallBack(int requestCode, int resultCode, Intent data)
        {
            if (requestCode == mAuthActivityCode)
            {
                // Successfully redirected.
                if (resultCode == Activity.RESULT_OK)
                {
                    // Check OAuth 2.0/2.10 error code.
                    String error = data.getStringExtra("error");
                    if (error == null)
                    {
                        error = data.getStringExtra("error_type");
                    }

                    // error occurred.
                    if (error != null)
                    {
                        if (error.equals("access_denied") || error.equals("OAuthAccessDeniedException"))
                        {
                            Log.d("puzzlehero", "Login canceled by user.");
//                            mAuthDialogListener.onCancel();
                            CCNative.ssoCBWithCancel();
                        }
                        else
                        {
                            String description = data.getStringExtra("error_description");
                            if (description != null)
                            {
                                error = error + ":" + description;
                            }
                            Log.d("puzzlehero", "Login failed: " + error);
                            CCNative.ssoCBWithError(resultCode, error, description);
//                            mAuthDialogListener.onError(new WeiboDialogError(error, resultCode, description));
                        }

                        // No errors.
                    }
                    else
                    {
                        if (null == mAccessToken)
                        {
                            mAccessToken = new Oauth2AccessToken();
                        }
                        mAccessToken.setToken(data.getStringExtra(KEY_TOKEN));
                        mAccessToken.setExpiresIn(data.getStringExtra(KEY_EXPIRES));
                        mAccessToken.setRefreshToken(data.getStringExtra(KEY_REFRESHTOKEN));
                        if (mAccessToken.isSessionValid())
                        {
                            Log.d("puzzlehero",
                                    "Login Success! access_token="
                                            + mAccessToken.getToken() + " expires="
                                            + mAccessToken.getExpiresTime()
                                            + "refresh_token="
                                            + mAccessToken.getRefreshToken());
                            CCNative.ssoCBWithSucceeded(data.getExtras());
//                            mAuthDialogListener.onComplete(data.getExtras());
                        }
                        else
                        {
                            Log.d("puzzlehero", "Failed to receive access token by SSO");
//                        startDialogAuth(mAuthActivity, mAuthPermissions);
//                            mWeibo.startAuthDialog(mAuthActivity, mAuthDialogListener);
                            CCNative.ssoCBWithAttemptFailed();
                        }
                    }
                    // An error occurred before we could be redirected.
                }
                else if (resultCode == Activity.RESULT_CANCELED)
                {
                    // An Android error occured.
                    if (data != null)
                    {
                        Log.d("puzzlehero", "Login failed: " + data.getStringExtra("error"));
                        CCNative.ssoCBWithError(
                                data.getIntExtra("error_code", -1),
                                data.getStringExtra("error"),
                                data.getStringExtra("failing_url"));
//                        mAuthDialogListener.onError(new WeiboDialogError(data
//                                .getStringExtra("error"), data.getIntExtra(
//                                "error_code", -1), data
//                                .getStringExtra("failing_url")));

                        // User pressed the 'back' button.
                    }
                    else
                    {
                        Log.d("puzzlehero", "Login canceled by user.");
//                        mAuthDialogListener.onCancel();
                        CCNative.ssoCBWithCancel();
                    }
                }
            }
        }
    }
}
