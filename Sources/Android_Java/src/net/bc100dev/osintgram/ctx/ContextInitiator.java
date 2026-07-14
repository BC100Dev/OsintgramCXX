package net.bc100dev.osintgram.ctx;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.ContextThemeWrapper;

public class ContextInitiator {

    @SuppressLint("StaticFieldLeak")
    private static ContextInitiator initiator;

    private Context ctx;

    protected ContextInitiator() {
        try {
            @SuppressLint("PrivateApi")
            Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
            Object activityThread = activityThreadClass.getMethod("currentActivityThread").invoke(null);
            Object context = activityThreadClass.getMethod("getSystemContext").invoke(activityThread);

            ctx = (Context) context;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public Context getContext() {
        return ctx;
    }

    public Context getThemedContext() {
        return new ContextThemeWrapper(ctx, android.R.style.Theme_DeviceDefault);
    }

    public static ContextInitiator getInstance() {
        if (initiator == null)
            initiator = new ContextInitiator();

        return initiator;
    }

}
