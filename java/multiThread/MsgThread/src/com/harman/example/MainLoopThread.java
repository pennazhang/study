package com.harman.example;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class MainLoopThread extends MsgThread {
    int idleCount = 0;
    int msgCount = 0;
    void onIdle()
    {
        System.out.println("idleCount = " + idleCount);
        idleCount++;
    }

    boolean onDispatch(Message msg)
    {
        switch (msg.getMsgID())
        {
            case StartMessage.s_msgID:
                msgCount++;
                StartMessage startMessage = (StartMessage)msg;
                DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy/MM/dd HH:mm:ss");
                LocalDateTime now = startMessage.getTime();
                System.out.println("msgCount = " + msgCount + ", Time = " + dtf.format(now));
                return true;
            default:
                break;
        }
        return false;
    }
}
