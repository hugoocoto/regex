test: test.c regex.c regex.h
	gcc test.c regex.c -Wall -Wextra -ggdb -o test
	./test
