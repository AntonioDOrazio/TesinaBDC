all:
	gcc -g src/*.c -o applicazione `mariadb_config --cflags --include --libs` -lncurses
clean:
	-rm applicazione
