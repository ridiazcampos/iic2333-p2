FILES := $(wildcard src/*.c)

program: $(FILES)
	gcc -o server -ggdb3 $(FILES) -lm -pthread -I./src/
