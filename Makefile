CFLAGS = -g -Wall -Wno-nullability-completeness

ttts.o: ttts.c
	gcc $(CFLAGS) -c ttts.c

ttt.o: ttt.c
	gcc $(CFLAGS) -c ttt.c