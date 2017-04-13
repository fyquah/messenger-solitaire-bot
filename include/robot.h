#include <stdio.h>
#include <stdlib.h>

#include <jni.h>


typedef jobject robot_h;


#ifdef __cplusplus
extern "C" {
#endif


robot_h robot_init();
void robot_mouse_move(robot_h robot, int x, int y);
void robot_free(robot_h robot);


#ifdef __cplusplus
}
#endif
