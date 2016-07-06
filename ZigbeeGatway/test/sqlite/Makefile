test:test.c JIP_Sqlite.o
	gcc -o  test test.c JIP_Sqlite.o -lsqlite3
JIP_Sqlite.o:JIP_Sqlite.c JIP_Sqlite.h
	gcc -c JIP_Sqlite.c
clean:
	rm test
