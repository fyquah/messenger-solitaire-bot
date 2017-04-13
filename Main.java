public class Main {
    static {
        System.loadLibrary("robot");
        System.loadLibrary("program");
    }

    public static native void entry_point(String[] args);

    public static void main(String[] args) {
        entry_point(args);
    }
}
