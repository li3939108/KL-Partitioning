cc=gcc
fm: fm.c
	$(cc) -g $< -o $@
