#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"



int main(int argc, char ** argv){
	if(argc == 2){
		FILE* input; //input file
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		int line_n = 0, V, E ;
		Vertex **vlist ;
		int **elist ;
		Graph *G ;

		input = fopen(argv[1], "r");
		if (input == NULL){
			exit(EXIT_FAILURE);}

		while ((read = getline(&line, &len, input)) != -1) {
			int num1, num2 ;
			char *saveptr ;
			num1 = atoi( strtok_r(line, " ", &saveptr) );
			num2 = atoi( strtok_r(NULL, " ", &saveptr) );
			printf("two numbers read: %d %d\n", num1, num2);
			if(line_n == 0){
				V = num1 ;
				E = num2 ;
				vlist = (Vertex **)calloc(V + 1, sizeof(Vertex *)) ;
				elist = (int **)calloc(E + 1, sizeof(int *)) ;
				elist[0] = NULL ;
				vlist[0] = NULL ;
			}else{
				int *pair = (int *)calloc(4, sizeof(int)) ;
				Vertex *v1, *v2 ;
				pair[0] = line_n ;
				pair[1] = num1 ;
				pair[2] = num2 ;
				pair[3] = 1 ;
				elist[line_n] = pair ;
				v1 = vlist[num1] ;
				v2 = vlist[num2] ;
				if(v1 == NULL){
					v1 = new_vertex(num1) ;
				}
				if(v2 == NULL){
					v2 = new_vertex(num2) ;
				}
				add_adjacency_vertex(v1, v2->label, 1) ;
				add_adjacency_vertex(v2, v1->label, 1) ;
				vlist[v1->label] = v1 ;
				vlist[v2->label] = v2 ;
			}
			line_n += 1 ;
		}
		G = new_graph(0, NULL);
		G->V = V ;
		G->E = E ;
		free(G->edge_list);
		free(G->adj_list);
		G->edge_list = elist ;
		G->adj_list = vlist ;
		pg(G);
		free_graph (G);
		free(line);
		fclose(input) ;
	}
}
