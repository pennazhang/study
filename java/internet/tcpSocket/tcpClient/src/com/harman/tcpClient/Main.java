package com.harman.tcpClient;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        String serverAddress = "10.82.166.11";
        int port = 5023;
        SocketThread clientThrad = null;
        try
        {
            System.out.println("Connecting to: " + serverAddress + ":" + port);
            Socket socket = new Socket(serverAddress, port);
            clientThrad = new SocketThread(socket);
            clientThrad.start();
        }
        catch (UnknownHostException ex)
        {
            System.out.println("Server not found: " + ex.getMessage());
            return;
        } catch (IOException ex) {
            System.out.println("I/O error: " + ex.getMessage());
            return;
        }

        /* Let's wait command to quit... */
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
                else
                {
                    clientThrad.sendMessage(command + "\n");
                }
            }
        }

        System.out.println("quit...");
        clientThrad.quit();
        try {
            clientThrad.join();
        }
        catch (InterruptedException e)
        {
            // e.printStackTrace();
        }
        System.out.println("Bye...");
    }
}
