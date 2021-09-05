CC := gcc
CFLAGS := -Isrc -static
AR := ar

.POSIX: clean

all: libcsflib.a

clean:
	rm -f libcsflib.a $(shell find src/ -name "*.o")

libcsflib.a: $(patsubst %.c, %.o, $(shell find src/ -name "*.c"))
	$(AR) r $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
