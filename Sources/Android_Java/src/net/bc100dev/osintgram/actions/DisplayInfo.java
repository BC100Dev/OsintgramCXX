package net.bc100dev.osintgram.actions;

import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.Method;

public class DisplayInfo extends JAction {

    public DisplayInfo(String[] args) {
        super(args);
    }

    @Override
    public void executeAction() {
        int width = -1;
        int height = -1;
        int dpi = -1;

        try {
            Class<?> dmgClass = Class.forName("android.hardware.display.DisplayManagerGlobal");

            Method getInstance = dmgClass.getDeclaredMethod("getInstance");
            Object dmg = getInstance.invoke(null);

            Method getRealDisplay = dmgClass.getDeclaredMethod("getRealDisplay", int.class);
            Object display = getRealDisplay.invoke(dmg, 0);

            // Get DisplayMetrics via reflection too
            Class<?> metricsClass = Class.forName("android.util.DisplayMetrics");
            Object metrics = metricsClass.getDeclaredConstructor().newInstance();

            Class<?> displayClass = display.getClass();
            Method getRealMetrics = displayClass.getDeclaredMethod("getRealMetrics", metricsClass);
            getRealMetrics.invoke(display, metrics);

            width = metricsClass.getField("widthPixels").getInt(metrics);
            height = metricsClass.getField("heightPixels").getInt(metrics);
            dpi = metricsClass.getField("densityDpi").getInt(metrics);
        } catch (Exception e) {
            System.err.println("Failed: " + e.getMessage());
            e.printStackTrace(System.err);
        }

        if (width == -1 && height == -1 && dpi == -1) {
            System.err.println("Failed to obtain screen dimension set");
            System.exit(1);
        }

        String jsonOut = "{\"width\": $WIDTH, \"height\": $HEIGHT, \"dpi\": $DPI}"
                .replace("$WIDTH", String.valueOf(width))
                .replace("$HEIGHT", String.valueOf(height))
                .replace("$DPI", String.valueOf(dpi));

        if (args.length == 0) {
            System.out.println(jsonOut);
            return;
        }

        try (FileWriter fw = new FileWriter(args[0])) {
            fw.write(jsonOut);
        } catch (IOException ex) {
            System.err.println("Failed to write screen dimension set");
            System.out.println("WIDTH=" + width);
            System.out.println("HEIGHT=" + height);
            System.out.println("DPI=" + dpi);
            System.exit(1);
        }
    }
}
