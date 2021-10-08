import com.harman.example.FileLogger;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.FileHandler;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class Main {
    private static final String LOG_FOLDER_NAME = "MyLoggerFile";
    private static final String LOG_FILE_SUFFIX = ".log";
    private synchronized static String getLogFilePath() {
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
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
        logFilePath.append(sdf.format(new Date()));
        logFilePath.append(LOG_FILE_SUFFIX);

        return logFilePath.toString();
    }

    public static void main(String[] args) {
        fileLogTest1();
        fileLogTest2();
    }
    static void fileLogTest1()
    {
        // Create a Logger with class name GFG
        Logger logger = Logger.getLogger("com.harman.test");

        FileHandler fileHandler = null;
        String filePath = getLogFilePath();
        System.out.println("FilePath1: " + filePath);
        try {
            // 文件日志内容标记为可追加
            fileHandler = new FileHandler(filePath, true);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        //以文本的形式输出, 缺省是以XMLFormatter格式输出。
        fileHandler.setFormatter(new SimpleFormatter());
        logger.addHandler(fileHandler);

        // Call info method
        logger.info("Message 1");
        logger.info("Message 2");
    }

    static void fileLogTest2()
    {
        // Default fileName: YYYY-MM-DD.log
        FileLogger.setFileName("test.log");
        FileLogger.setAppendAble(false);

        Logger logger = FileLogger.getInstance();
        System.out.println("FilePath2: " + FileLogger.getLogFilePath());
        // Call info method
        logger.info("Message 1");
        logger.info("Message 2");
    }
}
