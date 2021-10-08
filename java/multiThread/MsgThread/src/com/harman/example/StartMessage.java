package com.harman.example;

import java.time.LocalDateTime;

public class StartMessage extends Message {
    final static String s_msgID = "MSGID_START";
    public StartMessage() {
        super(s_msgID);
        time = LocalDateTime.now();
    }

    LocalDateTime getTime()
    {
        return time;
    }
    LocalDateTime time;
}
