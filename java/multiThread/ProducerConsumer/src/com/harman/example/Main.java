package com.harman.example;

public class Main {
    public static void main(String[] args) throws InterruptedException {

        ShareData shareData = new ShareData(new int[]{1, 2, 3, 4, 5});
        PVSemaphore empty = new PVSemaphore(0);
        PVSemaphore full = new PVSemaphore(ShareData.TOTAL_COUNT - shareData.size() );

        ProducerThread producer1 = new ProducerThread(empty, full, shareData, 1);
        ProducerThread producer2 = new ProducerThread(empty, full, shareData, 2);
        ConsumerThread consumer = new ConsumerThread(empty, full, shareData);

        producer1.start();
        producer2.start();
        consumer.start();

        try
        {
            Thread.sleep((long) (10000));
        } catch (InterruptedException e) {
        }

        System.out.println("I am leaving...");

        /* We must quit a producer first to avoid deadlock. */
        producer1.quit();
        producer1.join();

        producer2.quit();
        consumer.quit();
        producer2.join();
        consumer.join();

        System.out.println("Bye...");
    }
}
