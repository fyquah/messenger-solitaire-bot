UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	INC = -Iinclude/ -I/System/Library/Frameworks/JavaVM.framework/Headers
	CFLAGS += -DDARWIN -framework JavaVM $(INC)
	CXXFLAGS += $(CFLAGS) -std=c++11
	TARGET_LIB=librobot.dylib
	CC=clang
	OS=mac
endif


all: $(TARGET_LIB)


$(TARGET_LIB): src/robot.o
	${CC} $^ -o $@ $(CFLAGS) -shared


main: test/main.o $(TARGET_LIB)
	$(CXX) $< -o $@ -L. -lrobot
