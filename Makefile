CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o thread.o main.o

#TIMING flag for only show execution time
ifeq ($(strip $(TIMING)),1)
CFLAGS += -DTIMING
endif

##benchmark argument
TEST_SET=( 1 2 4 8 16 )
#TEST_NUM=${#TEST_SET[@]}
TEST_NUM=5
TEST_DATA_NUM=10000


.PHONY: all clean test git-hook astyle

all: sort

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

benchmark-time:
	bash scripts/$@.bash

benchmark-lock:
	bash scripts/$@.bash

git-hook:
	sh scripts/install-git-hooks

astyle:
	astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none *.[ch]

clean:
	rm -f $(OBJS) sort
	@rm -rf $(deps)

-include $(deps)
