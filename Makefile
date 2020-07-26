CC = gcc
CFLAGS = -Wall -Wno-unused-result -Wno-unknown-pragmas -Wfatal-errors
EXEC = main
SLIB = liblunar.so

all: $(EXEC) $(SLIB)

$(EXEC): $(SLIB)
	@$(CC) -o $@ $(CFLAGS) main.c $^

$(SLIB):
	@$(CC) -o $@ $(CFLAGS) -shared -fPIC lunar.c

clean:
	@rm $(EXEC) $(SLIB)