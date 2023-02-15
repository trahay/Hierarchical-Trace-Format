BIN= liblock.so read_trace #analyze_trace

CFLAGS=-Wall -g -O3
LDFLAGS=-ldl -pthread

OBJ=write_events.o timestamp.o
all: $(BIN)

read_trace: read_trace.c liblock.h
	$(CC) -o read_trace read_trace.c $(CFLAGS) $(LDFLAGS)

analyze_trace: analyze_trace.c liblock.h
	$(CC) -o analyze_trace analyze_trace.c $(CFLAGS) $(LDFLAGS)

liblock.so: mutex.o cond.o $(OBJ)
	$(CC) -shared $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $^ -fPIC $(CFLAGS)

clean:
	rm -f $(BIN) *.o
