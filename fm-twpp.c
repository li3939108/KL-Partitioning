#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_LIMIT 10000

struct vertex {
	char block ;
	unsigned char locked  ;
	int ldc;
	int* ld;
};
typedef struct vertex V;

V vertexarr[NODE_LIMIT];

int main(int argc, char ** argv){
	memset(vertexarr, 0, sizeof(V) * NODE_LIMIT);
	if(argc == 2){
		FILE *input = NULL;
		int state = 0;
		int index ; 
		int ldindex = 0;
		input = fopen(argv[1], "r");
		for(;;){
			char c = fgetc(input);
			int number ;
			if(c == 'N' || c == 'e' || c == 't' ){
				state = 0;
				continue;
			}else{if( c <= '9' && c >= '0' ){
				ungetc(c, input);
				fscanf(input, "%d", &number);
				switch(state){
				case 0:
				index = number ;
				if(index <= 50 ){
					vertexarr[index].block = 'A' ;
				}else{if(index >= 50){
					vertexarr[index].block = 'B' ;
				}}
				break ;

				case 1:
				vertexarr[index].ldc = number ;
				vertexarr[index].ld = calloc(sizeof(int), number);
				memset(vertexarr[index].ld, 0, number * sizeof(int));
				break ;

				case 2:
				vertexarr[index].ld[ldindex] = number;
				ldindex += 1 ;
				break ;

				default:
				break ;
				}
			}else{if( c == ':' ){
				state = 2; 
				continue;
			}else{if( c == '('){
				state = 1;
				ldindex = 0 ;
				continue;
			}else{if( c == ' ' || c == ')'){
				continue;
			}else{if(c == EOF){
				break;
			}}}}}}
		}
	}else{
		fprintf(stderr, "too few arguments\n");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	for( i = 1; vertexarr[i].block != 0 && !vertexarr[i].locked ; i++){
		int j = 0;
		printf("Net %d(%d): ", i, vertexarr[i].ldc);
		for(j = 0; j < vertexarr[i].ldc; j++){
			printf("%d ", vertexarr[i].ld[j]);
		} 
		putchar('\n');
		
	}
	computeCellGain( vertexarr) ;
}

int computeCellGain(const V * varr){
	int i, j;
	int gain[NODE_LIMIT] ;
	for( i = 1; varr[i].block != 0 && !varr[i].locked; i++){
		gain[i] = 0 ;	
		for( j = 0; j < varr[i].ldc; j++){
			if( varr[varr[i].ld[j]].block == varr[i].block ){
				gain[i] -= 1;
			}else{if(varr[varr[i].ld[j]].block != varr[i].block){
				gain[i] += 1;
			}}
		}
		printf("Gain of Net[%d]: %d\n", i, gain[i]);
	}
	return 0;
}

int 
