
PROGS = fs_cli fs_serv 

OBJS = fs_cli.o fs_serv.o

all:	${PROGS}

fs_cli: fs_cli.o 	
		cc -o $@ fs_cli.o -lunp -lpthread
fs_serv: fs_serv.o	rand.h	rand.c	list.h	list.c 		
		cc -o $@ $^ -lunp -lpthread 
clean:
		rm -f ${PROGS} ${OBJS} ${CLEANFILES}
