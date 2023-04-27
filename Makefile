CFLAGS = -g -Wall -Wno-nullability-completeness

ttts: ttts.c
	gcc $(CFLAGS) ttts.c -o ttts

ttt: ttt.c
	gcc $(CFLAGS) ttt.c -o ttt

clean:
	@echo "Removing everything but the source files"
	@rm ttt.o ttts.o ttt ttts