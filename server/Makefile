FILES := $(wildcard src/*.c)

program: $(FILES)
	gcc -o server $(FILES) -std=c11 -Wall -lm -pthread -I./src/
