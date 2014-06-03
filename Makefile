
kl: graph.c kl.c
	$(CC) -g $+ -o $@
clean:
	rm -f *o kl fm
