package com.harman.example;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class FileLogger {
    static Logger logger = null;
    private static final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    private static final String LOG_FOLDER_NAME = "Log";
    private static final String LOG_FILE_SUFFIX = ".log";
    private static String defaultFileName = "";
    private static boolean appendFlag = true;
    public static void setFileName(String fileName)
    {
        defaultFileName = fileName;
    }
    public static void setAppendAble(boolean appendAble)
    {
        appendFlag = appendAble;
    }
    public synchronized static String getLogFilePath() {
        StringBuffer logFilePath = new StringBuffer();
        logFilePath.append(System.getProperty("user.home"));
        logFilePath.append(File.separator);
        logFilePath.append(LOG_FOLDER_NAME);

        File file = new File(logFilePath.toString());
        if (!file.exists())
        {
            file.mkdir();
        }

        logFilePath.append(File.separator);
        if (defaultFileName.isEmpty()) {
            logFilePath.append(sdf.format(new Date()));
            logFilePath.append(LOG_FILE_SUFFIX);
        }
        else
        {
            logFilePath.append(defaultFileName);
        }

        return logFilePath.toString();
    }

    public synchronized static Logger getInstance()
    {
        if (logger == null) {
            logger = Logger.getLogger("com.harman.example");

            FileHandler fileHandler = null;
            try {
                fileHandler = new FileHandler(getLogFilePath(), appendFlag);
            } catch (IOException e) {
                e.printStackTrace();
            }
            fileHandler.setFormatter(new SimpleFormatter());
            logger.addHandler(fileHandler);
        }

        return logger;
    }
}
