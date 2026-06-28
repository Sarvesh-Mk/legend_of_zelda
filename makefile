DIR_SOURCES = $(wildcard *.c include/*.h arrays/*.c)
build: $(DIR_SOURCES)
	gcc main.c map.c entities.c logic.c arrays/*.c -o build/play -I include -lGL -lglut -lm  -lGLU -lGLEW
	./build/play



