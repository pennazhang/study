package com.harman.example;

public class ConsumerThread extends Thread {
    PVSemaphore emptySemaphore, fullSemaphore;
    ShareData shareData;
    boolean runFlag = true;

    public ConsumerThread(PVSemaphore emptySemaphore, PVSemaphore fullSemaphore, ShareData shareData) {
        this.emptySemaphore = emptySemaphore;
        this.fullSemaphore = fullSemaphore;
        this.shareData = shareData;
    }

    public void quit() {
        runFlag = false;
    }

    public void run()
    {
        for (;;)
        {
            if (runFlag == false)
            {
                break;
            }
            try
            {
                sleep((long) (Math.random() * 1000));
            } catch (InterruptedException e) {
            }

            try {
                emptySemaphore.P();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            int value = shareData.getOut();
            System.out.println("Consumer: " + value );
            fullSemaphore.V();
        }
    }
}
