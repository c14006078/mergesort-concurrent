CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o thread.o main.o

.PHONY: all clean test git-hook

all: sort

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

git-hook:
	sh scripts/install-git-hooks

clean:
	rm -f $(OBJS) sort
	@rm -rf $(deps)

-include $(deps)
