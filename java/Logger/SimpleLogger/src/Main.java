import java.util.logging.Logger;

public class Main {
    public static void main(String[] args) {
        // Create a Logger with class name GFG
        Logger logger = Logger.getLogger(Main.class.getName());

        // Call info method
        logger.info("Message 1");
        logger.info("Message 2");
    }
}
