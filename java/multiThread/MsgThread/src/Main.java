import com.harman.example.MainLoopThread;
import com.harman.example.StartMessage;

public class Main {
    public static void main(String[] args) throws InterruptedException {

        MainLoopThread mainThread = new MainLoopThread();
        mainThread.start();

        for (int i = 0; i < 10; i++)
        {
            StartMessage msg = new StartMessage();
            mainThread.sendMessage(msg);
            Thread.sleep(10);
        }

        mainThread.quit();
    }
}
