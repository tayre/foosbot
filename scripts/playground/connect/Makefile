connect: connect.c
	gcc $^ -o connect -Wall -lwebsockets

test-client: test-client.c
	gcc $^ -o test-client -Wall -lwebsockets

.PHONY: clean

clean:
	rm -f connect test-client
