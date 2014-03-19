#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _Vertex{
	int degree ;
	int label ;
	int rank ;
	int (*list)[2] ;
	struct _Vertex *parent ;
} Vertex;


int main(int argc, char ** argv){
	if(argc == 2){
		FILE* input; //input file
		char *line = NULL;
		size_t len = 0;
		ssize_t read;

		input = fopen(argv[1], "r");
		if (input == NULL){
			exit(EXIT_FAILURE);}

		while ((read = getline(&line, &len, input)) != -1) {
			int num1, num2 ;
			char *saveptr ;
			num1 = atoi( strtok_r(line, " ", &saveptr) );
			num2 = atoi( strtok_r(NULL, " ", &saveptr) );
			printf("two numbers read: %d %d\n", num1, num2);
	        }
		free(line);
	}
}
