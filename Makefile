
all: fm kl

fm: fm.c
	$(CC) -g $+ -o $@
kl: graph.c kl.c
	$(CC) -g $+ -o $@
clean:
	rm -f *o kl fm
