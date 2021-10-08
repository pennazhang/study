package com.harman.example;

import java.util.LinkedList;

public class MsgThread extends Thread {
    ShareMsgList shareMsgList;
    boolean exitFlag = false;

    MsgThread() {
        shareMsgList = new ShareMsgList();
    }

    public void sendMessage(Message msg)
    {
        shareMsgList.putIn(msg);
    }

    Message getMessage()
    {
        return shareMsgList.getOut();
    }

    public void run()
    {
        for (;;)
        {
            if (exitFlag == true)
            {
                break;
            }
            Message msg = getMessage();
            if (msg == null)
            {
                onIdle();
                try {
                    sleep(1);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    break;
                }
            }
            else
            {
                boolean dispatchedFlag = onDispatch(msg);
                if (dispatchedFlag == false)
                {
                    System.out.println("Unknown msgID: " + msg.getMsgID());
                }
            }
        }

        System.out.println("Exited!");
    }

    boolean onDispatch(Message msg)
    {
        switch (msg.getMsgID())
        {
            case "MSGID_EXIT":
                System.out.println("Exit...");
                quit();
                break;
            default:
                return (false);
        }
        return (true);
    }

    void onIdle() {
    }

    public void quit()
    {
        exitFlag = true;

        shareMsgList.clearAll();
        Message msg = new ExitMessage();
        sendMessage(msg);
    }
}
