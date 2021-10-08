package com.harman.example;

import java.util.LinkedList;
import java.util.concurrent.Semaphore;

public class ShareMsgList {
//    int totalCount;
    protected LinkedList<Message> msgList;

    ShareMsgList()
    {
        msgList = new LinkedList<Message>();
    }

    void putIn(Message msg) {
        synchronized(this) {
            msgList.addLast(msg);
        }
    }

    Message getOut() {
        synchronized(this) {
            if (msgList.isEmpty())
            {
                return null;
            }
            else {
                Message msg = msgList.removeFirst();
                return msg;
            }
        }
    }

    void clearAll()
    {
        synchronized(this) {
            msgList.clear();
        }
    }
}
