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


void robot_mouse_move(robot_h robot, int x, int y)
{
        JNIEnv *env;
        jclass klass;
        jmethodID method;

        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
        klass = (*env)->FindClass(env, "java/awt/Robot");
        method = (*env)->GetMethodID(env, klass, "mouseMove", "(II)V");

        if (method == 0) {
                printf("Failed\n");
        }

        (*env)->CallVoidMethod(env, robot, method, (jint) x, (jint) y);
}


void robot_mouse_press(robot_h robot, int button)
{
        JNIEnv *env;
        jclass klass;
        jmethodID method;

        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
        klass = (*env)->FindClass(env, "java/awt/Robot");
        method = (*env)->GetMethodID(env, klass, "mousePress", "(I)V");

        if (method == 0) {
                printf("Failed\n");
        }

        (*env)->CallVoidMethod(env, robot, method, (jint) button);
}


void robot_mouse_release(robot_h robot, int button)
{
        JNIEnv *env;
        jclass klass;
        jmethodID method;

        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
        klass = (*env)->FindClass(env, "java/awt/Robot");
        method = (*env)->GetMethodID(env, klass, "mouseRelease", "(I)V");

        if (method == 0) {
                printf("Failed\n");
        }

        (*env)->CallVoidMethod(env, robot, method, (jint) button);
}


void robot_free(robot_h robot)
{
        // dealloc object?
}
