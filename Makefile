all: a.exe bomb.bin

bomb.bin: bomb.c
	gcc -c -o bomb.o bomb.c
	objcopy -j.data bomb.o bomb.bin -O binary

a.exe: main.c model.c png.c gl.c gfx.c entity.c game.c
	gcc main.c model.c png.c gl.c game.c entity.c gfx.c -W -Wall -Og -ggdb -fno-strict-aliasing -mwindows -lopengl32 -lm -lpng
