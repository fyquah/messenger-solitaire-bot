#include "robot.h"

static JavaVM *jvm;
static JNIEnv *env;  /* it is fine to cache this - we are only using one java
                        thread.
                        */


static int start_jvm()
{
        const int n_options = 1;
        JavaVMInitArgs vm_args;
        JavaVMOption* options = calloc(sizeof(JavaVMOption), n_options);
        jint rc;

        options[0].optionString = "-Djava.class.path=.";
        vm_args.version = JNI_VERSION_1_6;
        vm_args.nOptions = n_options;
        vm_args.options = options;
        vm_args.ignoreUnrecognized = 0;

        rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);  // YES !!
        free(options);
        if (rc != JNI_OK) {
                return -1;
        }

        jint ver = (*env)->GetVersion(env);
        printf("JVM load succeeded: Version %d.%d", ((ver>>16)&0x0f), (ver&0x0f));

        return 0;
}


robot_h robot_init()
{
        jclass klass;
        jmethodID constructor;
        int ret;

        if (jvm == NULL) {
                puts("No active jvm detected - starting JVM!");
                ret = start_jvm();

                if (ret) {
                        puts("robot_init");
                        return NULL;
                }
        }

        klass = (*env)->FindClass(env, "java/awt/Robot");
        constructor = (*env)->GetMethodID(env, klass, "<init>", "()V");

        return (*env)->NewObject(env, klass, constructor);
}


void robot_mouse_move(robot_h robot, int x, int y)
{
        jclass klass = (*env)->FindClass(env, "java/awt/Robot");
        jmethodID method = (*env)->GetMethodID(env, klass, "mouseMove", "(II)V");


        if (method == 0) {
                printf("Failed\n");
        }

        (*env)->CallVoidMethod(env, robot, method, (jint) x, (jint) y);
}


void robot_free(robot_h robot)
{
        // dealloc object?
}
