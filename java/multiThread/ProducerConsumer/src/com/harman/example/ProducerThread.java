package com.harman.example;

public class ProducerThread extends Thread
{
    PVSemaphore emptySemaphore, fullSemaphore;
    ShareData shareData;
    int index;
    boolean runFlag = true;

    public ProducerThread(PVSemaphore emptySemaphore, PVSemaphore fullSemaphore, ShareData shareData, int index) {
        this.emptySemaphore = emptySemaphore;
        this.fullSemaphore = fullSemaphore;
        this.shareData = shareData;
        this.index = index;
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
                fullSemaphore.P();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            shareData.putIn(index);
            System.out.println("Producer: " + index );
            index += 10;
            emptySemaphore.V();
        }
    }

    public void quit() {
        runFlag = false;
    }
}
