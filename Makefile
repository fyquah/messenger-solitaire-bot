UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	INC = -Iinclude/ -I/System/Library/Frameworks/JavaVM.framework/Headers
	CFLAGS += -DDARWIN -framework JavaVM $(INC)
	CXXFLAGS += $(CFLAGS) -std=c++11
	ROBOT_LIB=librobot.dylib
	PROGRAM_LIB=libprogram.dylib
	CC=clang
	OS=mac
endif


ENTRY_POINT=Main.class


all: $(ROBOT_LIB) $(PROGRAM_LIB) $(ENTRY_POINT)


$(ENTRY_POINT): Main.java
	javac $<


$(ROBOT_LIB): src/robot.o
	${CC} $^ -o $@ $(CFLAGS) -shared


$(PROGRAM_LIB): test/main.o src/entry_point.o $(ROBOT_LIB)
	$(CXX) $^ -o $@ $(CFLAGS) -L. -lrobot -shared


run: $(PROGRAM_LIB) $(ROBOT_LIB) $(ENTRY_POINT)
	java Main


clean:
	rm -f $(PROGRAM_LIB) $(ROBOT_LIB) src/robot.o src/entry_point.o test/main.o
