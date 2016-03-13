src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)

CC = gcc
CFLAGS = -Wall
# Update this to include wiringPi on the Pi
LDFLAGS = -lwebsockets

foosbot: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) foosbot

.PHONY: cleandep
cleandep:
	rm -f $(dep)
