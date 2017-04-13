#include <stdio.h>
#include <stdlib.h>

#include <jni.h>


typedef jobject robot_h;


#ifdef __cplusplus
extern "C" {
#endif


const int ROBOT_BUTTON1_DOWN_MASK = 1024;
const int ROBOT_BUTTON1_MASK = 16;
const int ROBOT_BUTTON2_DOWN_MASK = 2048;
const int ROBOT_BUTTON2_MASK = 8;
const int ROBOT_BUTTON3_DOWN_MASK = 4096;
const int ROBOT_BUTTON3_MASK = 4;


void robot_jvm_init(JNIEnv *env);

robot_h robot_init();

/* keyboard */
void robot_key_press(robot_h robot, int keycode);
void robot_key_release(robot_h robot, int keycode);

/* mouse */
void robot_mouse_move(robot_h robot, int x, int y);
void robot_mouse_press(robot_h robot, int button);
void robot_mouse_release(robot_h robot, int button);
void robot_free(robot_h robot);


#ifdef __cplusplus
}
#endif
