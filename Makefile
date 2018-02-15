all: a.exe monkey.bin

cube.bin: box.c
	gcc -c -o box.o box.c
	objcopy -j.data box.o box.bin -O binary

monkey.bin: monkey.h
	cp monkey.h monkey.c
	gcc -c -o monkey.o monkey.c
	objcopy -j.data monkey.o monkey.bin -O binary

bomb.bin: bomb.c
	gcc -c -o bomb.o bomb.c
	objcopy -j.data bomb.o bomb.bin -O binary

a.exe: main.c model.c png.c gl.c monkey.bin cube.bin bomb.bin
	gcc main.c model.c png.c gl.c -W -Wall -Og -ggdb -fno-strict-aliasing -mwindows -lopengl32 -lm -lpng
