#include "entry_point.h"
#include "robot.h"


JNIEXPORT void JNICALL Java_Main_entry_1point
  (JNIEnv * env, jclass kls, jobjectArray arr)
{
  robot_jvm_init(env);
  entry_point(0, NULL);
}
