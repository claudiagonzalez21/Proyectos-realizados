#
#	MACROS
#

CC=gcc
CFLAGS = -g -Wall -c -pthread

all: shm_producer shm_consumer 

#
#	EJECUTABLES
#

shm_producer: shm_producer.o shared_queue.o
	$(CC) -pthread -o $@ $^ -lrt

shm_consumer: shm_consumer.o shared_queue.o
	$(CC) -pthread -o $@ $^ -lrt

#
#	OBJETOS
#

shm_producer.o: shm_producer.c shared_queue.h
	$(CC) $(CFLAGS) $<

shm_consumer.o: shm_consumer.c shared_queue.h
	$(CC) $(CFLAGS) $<


shared_queue.o: shared_queue.c shared_queue.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o

runv:
	valgrind --leak-check=full ./shm_consumer
