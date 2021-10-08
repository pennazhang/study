package com.harman.study;

import java.io.IOException;
import java.util.logging.Logger;
import java.util.logging.SocketHandler;

public class Main {
    public static void main(String[] args) {
        String host = "10.82.166.11";
        int port = 5023;
        Logger logger = Logger.getLogger("my.logger.SocketHandlerTest");
        try {
            SocketHandler handler = new SocketHandler(host, port);
            logger.addHandler(handler);
            logger.info("SocketHandler is running now......");
        } catch (IOException e) {
            logger.severe("Can't find the logServer: " + host + ":" + port);
            StringBuilder sb = new StringBuilder();
            sb.append(e.toString()).append("\n");
            for (StackTraceElement elem : e.getStackTrace()) {
                sb.append("\tat ").append(elem).append("\n");
            }
            logger.severe(sb.toString());
        }
    }
}
