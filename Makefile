CC = clang
CFLAGES = -O0
INC = -I.

socketTest: socketTest.o server.o client.o
	${CC} socketTest.o server.o client.o ${CFLAGS} ${INC} -o $@
socketTest.o: socketTest.c server.h client.h
	${CC} socketTest.c ${CGLAGS} ${INC} -c
server.o: server.c server.h queue.h
	${CC} server.c ${CGLAGS} ${INC} -c
client.o: client.c client.h
	${CC} client.c ${CGLAGS} ${INC} -c

clean:
	@rm -rf *.o
