UNAME_S := $(shell uname -s)

INC = -Iinclude/ -I../opencv/build/_install/usr/local/include/ -L../opencv/build/_install/usr/local/lib/

ifeq ($(UNAME_S),Darwin)
	INC += -I/System/Library/Frameworks/JavaVM.framework/Headers
	CFLAGS += -DDARWIN -framework JavaVM $(INC)
	CXXFLAGS += $(CFLAGS) -std=c++11
	ROBOT_LIB=librobot.dylib
	PROGRAM_LIB=libprogram.dylib
	CC=clang
	OS=mac
endif
ifeq ($(UNAME_S),Linux)
	INC += -I/usr/lib64/java/include/ -I/usr/lib64/java/include/linux
	CFLAGS += -DLINUX $(INC) -fpic -g -ggdb
	CXXFLAGS += $(CFLAGS) -std=c++11
	ROBOT_LIB=librobot.so
	PROGRAM_LIB=libprogram.so
	CC=gcc
	OS=linux
endif



ENTRY_POINT=Main.class


all: $(ROBOT_LIB) $(PROGRAM_LIB) $(ENTRY_POINT)


$(ENTRY_POINT): Main.java
	javac $<


$(ROBOT_LIB): src/robot.o include/robot.h
	${CC} $< -o $@ $(CFLAGS) -shared


$(PROGRAM_LIB): test/main.o src/entry_point.o $(ROBOT_LIB)
	$(CXX) $^ -o $@ $(CFLAGS) -L. -lrobot -lopencv_core -lopencv_highgui -shared


run: $(PROGRAM_LIB) $(ROBOT_LIB) $(ENTRY_POINT)
	java Main


clean:
	rm -f $(PROGRAM_LIB) $(ROBOT_LIB) src/robot.o src/entry_point.o test/main.o
