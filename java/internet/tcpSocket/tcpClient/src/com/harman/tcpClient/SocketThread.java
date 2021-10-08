package com.harman.tcpClient;

import java.io.*;
import java.net.Socket;

public class SocketThread extends Thread {
    Socket socket = null;
    InputStream inputStream;
    InputStreamReader inputStreamReader;
    BufferedReader bufferedReader;

    OutputStream outputStream;
    PrintWriter printWriter;

    boolean exitFlag = false;

    public SocketThread(Socket socket) {
        this.socket = socket;
    }

    public void quit(){
        try {
            socket.close();
        } catch (IOException e) {
//            e.printStackTrace();
        }
    }

    public void run()
    {
        System.out.println("ClientThread start...");

        try{
            inputStream = socket.getInputStream();
            inputStreamReader = new InputStreamReader(inputStream);
            bufferedReader = new BufferedReader(inputStreamReader);

            outputStream = socket.getOutputStream();
            printWriter = new PrintWriter(outputStream);

            String msg = "";
            while ((msg = bufferedReader.readLine()) != null)
            {
                onRecvMessage(msg);
                if (exitFlag == true)
                {
                    break;
                }
            }

        } catch (IOException e)
        {
//            e.printStackTrace();
        }

        try {
            bufferedReader.close();
            inputStreamReader.close();
            inputStream.close();

            printWriter.close();
            outputStream.close();

            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        System.out.println("ClientThread end.");
    }

    void onRecvMessage(String msg)
    {
        System.out.println("onRecv: "  + msg);
        sendMessage(msg + "\n");
    }

    synchronized void sendMessage(String msg)
    {
        printWriter.print(msg + "\n");
        printWriter.flush();

        if (msg.indexOf("exit") == 0)
        {
            exitFlag = true;
        }
    }
}
