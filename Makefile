CC = gcc
# standard is gnu99 instead of my usual c99 because it doesnt see usleep
# and i don't want to use nanosleep cause its annoying
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=gnu99

dualsense-inputs: src/main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm dualsense-inputs