ifeq ($(OS),Windows_NT)
MAIN = windows-main.c
CFLAGS = -fno-strict-aliasing
LDFLAGS = -mwindows -lopengl32 -lm -lpng
else
MAIN = glx-main.c
CFLAGS = -fno-strict-aliasing $(shell pkg-config --cflags x11 gl glu)
LDFLAGS = -lm -lpng $(shell pkg-config --libs x11 gl glu)
endif

all: a.exe bomb.bin

clean :
	$(RM) a.exe bomb.bin

bomb.bin: bomb.c
	gcc -c -o bomb.o bomb.c
	objcopy -j.data bomb.o bomb.bin -O binary

a.exe: $(MAIN) model.c png.c gl.c gfx.c entity.c game.c
	gcc -o $@ $^ -W -Wall -Og -ggdb -fno-strict-aliasing $(CFLAGS) $(LDFLAGS)
