package com.harman.test;

import java.net.ServerSocket;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        try {
            int serverPort = 5023;
            ServerSocket serverSocket = new ServerSocket(serverPort);
            System.out.println("TCP Server Listening at port:" + serverPort);
            ServerThread serverSocketThread= new ServerThread(serverSocket);
            serverSocketThread.start();
            if (serverSocketThread.getStatus() == ServerThread.STATUS.STATUS_OK)
            {
                /* Let's wait for 60 seconds. */
                {
                    System.out.println("Type \"exit\" to quit...");
                    Scanner scanner = new Scanner(System.in);
                    for (;;)
                    {
                        String command = scanner.next();
                        if (command.equals("bye") || command.equals("exit"))
                        {
                            break;
                        }
                    }
                }
                System.out.println("quit...");
                serverSocketThread.quit();
                serverSocketThread.join();
            }
            else
            {
                System.out.println("Failed to start server socket at port: serverPort");
            }
            serverSocket.close();
        }
        catch(Exception e){
            e.printStackTrace();
        }

        System.out.println("Bye...");
    }
}
