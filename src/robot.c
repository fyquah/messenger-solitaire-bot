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
                        printf("Failed\n");                                 \
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


void robot_free(robot_h robot)
{
        // dealloc object?
}
