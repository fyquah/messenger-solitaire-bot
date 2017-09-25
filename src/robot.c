#include "robot.h"

static JavaVM *jvm;


void robot_jvm_init(JNIEnv *env)
{
	(*env)->GetJavaVM(env, &jvm);
}


robot_h robot_init()
{
        jclass klass;
        jmethodID constructor;
        JNIEnv *env;
        int ret;

        if (jvm == NULL) {
                puts("No active jvm detected - Please call robot_jvm_init first!");
		return NULL;
        }

        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
        klass = (*env)->FindClass(env, "java/awt/Robot");
        constructor = (*env)->GetMethodID(env, klass, "<init>", "()V");

        return (*env)->NewObject(env, klass, constructor);
}


#define SETUP_JAVA_ENV(name, signature)                                     \
        JNIEnv *env;                                                        \
        static jmethodID method = 0;                                        \
                                                                            \
        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);             \
                                                                            \
	if (method == 0) {                                                  \
                jclass klass = (*env)->FindClass(env, "java/awt/Robot");    \
                method = (*env)->GetMethodID(env, klass, name, signature);  \
                                                                            \
                if (method == 0) {                                          \
                        printf("Robot method init %s :: %s failed\n",       \
                            name, signature);                               \
                        fflush(stdout);                                     \
                }                                                           \
	}                                                                   \


void robot_key_press(robot_h robot, int keycode)
{
        SETUP_JAVA_ENV("keyPress", "(I)V");
        (*env)->CallVoidMethod(env, robot, method, (jint) keycode);
}


void robot_key_release(robot_h robot, int keycode)
{
        SETUP_JAVA_ENV("keyRelease", "(I)V");
        (*env)->CallVoidMethod(env, robot, method, (jint) keycode);
}


void robot_mouse_move(robot_h robot, int x, int y)
{
        SETUP_JAVA_ENV("mouseMove", "(II)V");
        (*env)->CallVoidMethod(env, robot, method, (jint) x, (jint) y);
}


void robot_mouse_press(robot_h robot, int button)
{
        SETUP_JAVA_ENV("mousePress", "(I)V");
        (*env)->CallVoidMethod(env, robot, method, (jint) button);
}


void robot_mouse_release(robot_h robot, int button)
{
        SETUP_JAVA_ENV("mouseRelease", "(I)V");
        (*env)->CallVoidMethod(env, robot, method, (jint) button);
}

static jobject java_rectangle_of_rectangle_t(
    JNIEnv *env, const rectangle_t rectangle
)
{
        jclass klass;
        jmethodID constructor;
        int ret;

        if (jvm == NULL) {
                puts("No active jvm detected - Please call robot_jvm_init first!");
		return NULL;
        }

        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
        klass = (*env)->FindClass(env, "java/awt/Rectangle");
        constructor = (*env)->GetMethodID(env, klass, "<init>", "(IIII)V");

        return (*env)->NewObject(
            env, klass, constructor,
            rectangle.x, rectangle.y, rectangle.width, rectangle.height
        );
}

static void copy_buffered_image_to_carray(
    JNIEnv *env,
    jobject buffered_image,
    uint32_t height,
    uint32_t width,
    /* output */ uint32_t *dest
)
{
        static jmethodID method = 0;
        const char *method_name = "getRGB";
        const char *method_signature = "(II)I";
                                                                            
        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);             
                                                                            
	if (method == 0) {                                                  
                jclass klass = (*env)->FindClass(env, "java/awt/image/BufferedImage");
                method = (*env)->GetMethodID(
                    env, klass, method_name, method_signature);

                if (method == 0) {
                        printf("Failed\n");
                        fflush(stdout);
                }
	}

        uint32_t pos = 0;
        for (uint32_t y = 0 ; y < height; y++) {
          for (uint32_t x = 0 ; x < width ; x++) {
            dest[pos] = (*env)->CallIntMethod(
                  env, buffered_image, method, x, y);
            pos++;
          }
        }
}


void robot_screenshot(robot_h robot, const rectangle_t rect, uint32_t *dest)
{
        SETUP_JAVA_ENV(
            "createScreenCapture",
            "(Ljava/awt/Rectangle;)Ljava/awt/image/BufferedImage;"
        );
        jobject rectangle_object = java_rectangle_of_rectangle_t(env, rect);
        jobject buffered_image = (*env)->CallObjectMethod(
            env, robot, method, rectangle_object);
        copy_buffered_image_to_carray(
            env, buffered_image, rect.height, rect.width, dest);
}


void robot_free(robot_h robot)
{
        // dealloc object?
}
