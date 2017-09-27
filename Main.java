import java.awt.Robot;
import java.awt.AWTException;

public class Main {
    static {
        System.loadLibrary("robot");
        System.loadLibrary("program");
    }

    public static native void entry_point(String[] args);

    public static void main(String[] args) {
        if (args.length >= 1 && args[0].equals("undo")) {
          try {
            Robot robot = new Robot();
            for (int i = 0 ; i < 300 ; i++) {
              robot.mousePress(16);
              robot.mouseRelease(16);
              Thread.sleep(10);
            }
          } catch (AWTException e) {

          } catch (InterruptedException e) {

          }
          return;
        }

        entry_point(args);
    }
}
