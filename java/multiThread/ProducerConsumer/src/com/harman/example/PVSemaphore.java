package com.harman.example;

import javax.management.remote.SubjectDelegationPermission;
import java.util.LinkedList;
import java.util.concurrent.Semaphore;

public class PVSemaphore extends Semaphore {
    PVSemaphore(int initCount) {
        super(initCount);
    }

    void P() throws InterruptedException {
        acquire();
    }

    synchronized  void V()
    {
        release();
    }
}

class ShareData
{
    static final int TOTAL_COUNT = 10;
    LinkedList<Integer> linkList = new LinkedList<>();

    public ShareData(int a[])
    {
        for (int i = 0; i < a.length; i++)
        {
            linkList.addLast(a[i]);
            System.out.println("ShareData " + a[i] + " - left size = " + linkList.size());
        }
    }

    synchronized int getOut()
    {
        if (!linkList.isEmpty())
        {
            int value = linkList.removeFirst();
            System.out.println("remove - left size = " + linkList.size());
            return value;
        }
        else
        {
            assert(false);
            return (-1);
        }
    }

    synchronized void putIn(int value)
    {
        linkList.addLast(value);
        System.out.println("putIn - left size = " + linkList.size());
    }

    synchronized int size()
    {
        return (linkList.size());
    }
}
