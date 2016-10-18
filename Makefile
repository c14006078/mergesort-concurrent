CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o thread.o main.o

#TIMING flag for only show execution time
ifeq ($(strip $(TIMING)),1)
CFLAGS += -DTIMING
endif

.PHONY: all clean test git-hook astyle

all: sort

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

git-hook:
	sh scripts/install-git-hooks

astyle:
	astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none *.[ch]

clean:
	rm -f $(OBJS) sort
	@rm -rf $(deps)

-include $(deps)
