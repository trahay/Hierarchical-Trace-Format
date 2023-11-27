BIN=libpthread_trace.so
CFLAGS=-Wall -g `pkg-config --cflags htf`
LDFLAGS=`pkg-config --libs htf`

all: $(BIN)

libpthread_trace.so: cond.c mutex.c tracer.c
	$(CC) -shared -o libpthread_trace.so cond.c mutex.c tracer.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f libpthread_trace.so
