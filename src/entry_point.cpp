#include "entry_point.h"
#include "robot.h"


JNIEXPORT void JNICALL Java_Main_entry_1point
  (JNIEnv * env, jclass kls, jobjectArray arr)
{
  int length = env->GetArrayLength(arr);
  const char **argv = new const char*[length + 1];

  argv[0] = "Main";
  for (int i = 0 ; i < length ; i++) {
    const char *str = env->GetStringUTFChars(
        (jstring) env->GetObjectArrayElement(arr, i),
        NULL);
    argv[i + 1] = str;
  }

  robot_jvm_init(env);
  entry_point(length + 1, argv);
  delete[] argv;
}
