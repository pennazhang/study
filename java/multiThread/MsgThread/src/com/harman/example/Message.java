package com.harman.example;

import java.sql.Time;
import java.time.LocalDateTime;

public class Message {
    String msgID;

    String getMsgID()
    {
        return (msgID);
    }

    Message(String msgID)
    {
        this.msgID = msgID;
    }
}

class ExitMessage extends Message {
    final static String s_msgID = "MSGID_EXIT";
    ExitMessage() {
        super(s_msgID);
    }
}


