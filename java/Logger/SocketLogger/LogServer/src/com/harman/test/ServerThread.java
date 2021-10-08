package com.harman.test;


import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

class WorkerThread extends Thread {
    static int s_id = 0;

    int id;
    Socket socket = null;
    InputStream inputStream;
    InputStreamReader inputStreamReader;
    BufferedReader bufferedReader;

    OutputStream outputStream;
    PrintWriter printWriter;

    boolean exitFlag = false;

    public WorkerThread(Socket socket) {
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
        id = s_id;
        s_id++;
        System.out.println("WorkerThread:" + id + " start...");

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

        System.out.println("Bye, WorkerThread" + id + "...");
    }

    void onRecvMessage(String msg)
    {
        System.out.println("onRecv" + id + ": "  + msg);
        sendMessage(msg + "\n");
    }

    synchronized void sendMessage(String msg)
    {
        printWriter.print(msg);
        printWriter.flush();

        if (msg.indexOf("exit") == 0)
        {
            exitFlag = true;
        }
    }
}

public class ServerThread extends Thread {

    ArrayList<WorkerThread> WorkerThreads = new ArrayList<WorkerThread>();

    enum STATUS
    {
        STATUS_OK,
        STATUS_FAIL,
    };

    ServerSocket serverSocket = null;
    boolean quit = false;

    public ServerThread(ServerSocket serverSocket) {
        this.serverSocket = serverSocket;
    }

    public void run()
    {
        while (true)
        {
            try {
                Socket socket = serverSocket.accept();
                WorkerThread socketThread = new WorkerThread(socket);
                socketThread.start();
                WorkerThreads.add(socketThread);
            } catch (IOException e) {
                System.out.println("Quit listening thread.");
//                e.printStackTrace();
                break;
            }
        }
        onQuit();
    }

    public void quit()
    {
        quit = true;
        try {
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public STATUS getStatus()
    {
        return (STATUS.STATUS_OK);
    }

    protected void onQuit() {
        for (WorkerThread childThread: WorkerThreads)
        {
            if (childThread.isAlive())
            {
                childThread.quit();
                try {
                    childThread.join();
                } catch (InterruptedException e) {
//                    e.printStackTrace();
                }
            }
        }
    }
}
