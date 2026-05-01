package net.bc100dev.osintgram;

import net.bc100dev.osintgram.actions.DisplayInfo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Main {

    public static void main(String[] argv) {
        if (argv == null || argv.length == 0) {
            System.err.println("Usage: app_process ... classes.dex <action> <args>");
            System.err.println("Actions:");
            System.err.println("\tdisplay-info -> obtains display info and writes to a file, if provided");
            System.exit(1);
            return;
        }

        List<String> argL = new ArrayList<>(Arrays.asList(argv).subList(1, argv.length));
        String[] args = new String[argL.size()];
        for (int i = 0; i < argL.size(); i++)
            args[i] = argL.get(i);

        switch (argv[0]) {
            case "display-info":
                DisplayInfo dI = new DisplayInfo(args);
                dI.executeAction();
                break;
            case "battery-info":
                // for a future library
                break;
            default:
                System.err.println("\"" + argv[0] + "\": Unknown action provided");
                System.exit(1);
                break;
        }
    }

}
