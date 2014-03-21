#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

void input_gen(FILE *output, int D, int V){
	Graph *G = gen(D, V) ;
	if(output == NULL){
		return ;
	}
	edges(G, output) ;
	free_graph(G) ;
	return ;
}

int cut(Graph *G, Vertex *a[], Vertex *b[], FILE *output){
	int 
	(*cost)[G->V + 1] = (int (*)[G->V + 1])calloc(G->V + 1, sizeof *cost),//cost[G->V + 1][G->V + 1], 
	i, j, total_cost ;
	char *block = (char *)calloc(G->V + 1, sizeof *block) ; //block[G->V + 1] ;
	Vertex *v1, *v2 ;

	memset(cost, 0, (G->V + 1) * sizeof *cost) ;
	memset(block, 0, (G->V + 1) * sizeof *block) ;
	total_cost = 0 ;

	for(i = 0; i < G->V / 2; i++){
		block[ a[i]->label ] = 'a' ;
		block[ b[i]->label ] = 'b' ;
	}	
	block[ b[G->V - G->V / 2 - 1]->label ] = 'b' ;
	if(output != NULL){
		fprintf(output, "\nCutset: \n") ;
	}

	for(i = 0; i < G->V / 2; i++){
		v1 = a[i] ;
		v2 = b[i] ;
		for(j = 0; j < v1->degree; j++){
			cost[ v1->label ][ v1->list[j][0] ] = v1->list[j][1] ;
			if(block[ v1->label ] != block[ v1->list[j][0] ]){
				total_cost +=  v1->list[j][1] ;
				if(output != NULL){
					fprintf(output, "%d -- %d\n", v1->label, v1->list[j][0]);
				}
			}
		}
	}

	free(cost);
	free(block);
	return total_cost  ;
}

void partition(Graph *G, Vertex *a[], Vertex *b[]){
	int minus_inf = (int)(~0) << (sizeof(int) * 8 - 1) ;
	int
	*d = (int *)calloc(G->V + 1, sizeof *d ), //d[G->V + 1] 
	(*cost)[G->V + 1] = (int (*)[G->V + 1])calloc(G->V + 1, sizeof *cost),//cost[G->V + 1][G->V + 1], 
	*gsum = (int *)calloc(G->V / 2 + 1, sizeof *gsum), //gsum[G->V / 2 + 1], 
	(*ex)[2] = (int (*)[2])calloc(G->V / 2 + 1, sizeof *ex),//ex[G->V / 2 + 1][2], 
	*locked = (int *)calloc(G->V + 1, sizeof *locked), //locked[G->V + 1] ,
	i, j, k, maxk, maxgain, passes = 0  ;

	char *block = (char *)calloc(G->V + 1, sizeof *block) ; //block[G->V + 1] ;
	Vertex *v1, *v2 ;

	mainloop:
	//Initialization
	memset(d, 0, (G->V + 1) * sizeof *d)  ;
	memset(cost, 0, (G->V + 1) * sizeof *cost) ;
	memset(gsum , 0, (G->V / 2 + 1) * sizeof *gsum ) ;
	memset(ex, 0, (G->V / 2 + 1) * sizeof *ex) ;
	memset(block, 0, (G->V + 1) * sizeof *block) ;
	memset(locked, 0, (G->V + 1) * sizeof *locked ) ;

	gsum[0] = 0 ;
	maxk = 0 ;
	maxgain = minus_inf ;

	for(i = 0; i < G->V / 2; i++){
		block[ a[i]->label ] = 'a' ;
		block[ b[i]->label ] = 'b' ;
	}	
	if( G->V - G->V / 2  != G->V / 2) {
		block[ b[G->V - G->V / 2 - 1]->label ] = 'b' ;
	}

	//Compute the D value
	for (i = 0; i < G->V / 2; i++){
		v1 = a[i];
		v2 = b[i] ;
		for(j = 0; j < v1->degree; j++){
			if(block[ v1->list[j][0] ] == block[ v1->label ]){
				d[v1->label] -= v1->list[j][1] ;
			}else{
				d[v1->label] += v1->list[j][1] ;
			}
			cost[ v1->label ][ v1->list[j][0] ] = v1->list[j][1] ;
		}	
		for(j = 0; j < v2->degree; j++){
			if(block[ v2->list[j][0] ] == block[v2->label ]){
				d[v2->label] -= v2->list[j][1] ;
			}else{
				d[v2->label] += v2->list[j][1] ;
			}
			cost[ v2->label ][ v2->list[j][0] ] = v2->list[j][1] ;
		}
	}	

	if( G->V - G->V / 2  != G->V / 2) {
		v2 = b[G->V - G->V / 2 - 1] ;
		for(j = 0; j < v2->degree; j++){
			if(block[ v2->list[j][0] ] == block[v2->label ]){
				d[v2->label] -= v2->list[j][1] ;
			}else{
				d[v2->label] += v2->list[j][1] ;
			}
			cost[ v2->label ][ v2->list[j][0] ] = v2->list[j][1] ;
		}
	}

	//Inner loop, get the max-gain exchange pair 
	for (k = 1; k <= G->V / 2; k++){
		int to_be_locked[2], to_be_exchanged[2] ;
		for(i = 0; i < G->V / 2 ; i++){
			int a_i_label = a[i]->label ;
			if (!locked[ a_i_label ]) {
				for(j = 0; j < G->V - G->V / 2  ; j++){
					int b_j_label = b[j]->label ;
					if(!locked[ b_j_label ]){
						int gain = d[ a_i_label ] + d[ b_j_label ] - 2 * cost[ a_i_label ][ b_j_label ] ;
						if( gain > maxgain ){
							maxgain = gain ;
							to_be_locked[0] = a_i_label ;
							to_be_exchanged[0] = i ;
							to_be_locked[1] = b_j_label ;
							to_be_exchanged[1] = j ;
						}
					}
				}
			}
		}
		locked[ to_be_locked[0] ] = 1 ;
		locked[ to_be_locked[1] ] = 1 ;
		ex[k][0] = to_be_exchanged[0] ;
		ex[k][1] = to_be_exchanged[1] ;		
		//update D values 
		v1 = a[ to_be_exchanged[0] ] ;
		v2 = b[ to_be_exchanged[1] ] ;
		for(i = 0; i < v1->degree; i++){
			d[ v1->list[i][0] ] = 
			d[ v1->list[i][0] ] + (block[ v1->label ] == block[ v1->list[i][0] ] ? 2 : -2) * cost[ v1->label ][ v1->list[i][0] ] ;
		}
		for(i = 0; i < v2->degree; i++){
			d[ v2->list[i][0] ] = 
			d[ v2->list[i][0] ] + (block[ v2->label ] == block[ v2->list[i][0] ] ? 2 : -2) * cost[ v2->label ][ v2->list[i][0] ] ;
		}

		gsum[k] = gsum[k - 1] + maxgain;
		if(gsum[k] > gsum[ maxk ]){
			maxk = k ;
		}
		maxgain = minus_inf ;
	}
	if(maxk == 0){
		free(d) ;
		free(cost);
		free(gsum);
		free(ex);
		free(block) ;
		free(locked);
		printf("\npasses: %d \n", passes);
		return ;
	}else{
		passes += 1 ;
		for(i = 1; i <= maxk; i++){
			Vertex *temp = a[ ex[i][0] ] ;
			a[ ex[i][0] ] = b[ ex[i][1] ] ;
			b[ ex[i][1] ] = temp ;
		}
		goto mainloop ;
	}
				
}

int main(int argc, char ** argv){
	if(argc == 2){
		FILE *input, *output; //input file
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		int line_n = 0, V, E, (*elist)[4], i, j ;
		Vertex **vlist, **a, **b ;
		Graph *G, *G2 ;
/*
 *Generate input by itself
 *
		output = fopen(argv[1], "w") ;
		input_gen(output, 4, 1000) ;
		fclose(output) ;
*/

		input = fopen(argv[1], "r");
		if (input == NULL){
			exit(EXIT_FAILURE);}

		while ((read = getline(&line, &len, input)) != -1) {
			int num1, num2 ;
			char *saveptr ;
			num1 = atoi( strtok_r(line, " ", &saveptr) );
			num2 = atoi( strtok_r(NULL, " ", &saveptr) );
			if(line_n == 0){
				V = num1 ;
				E = num2 ;
				vlist = (Vertex **)calloc(V + 1, sizeof(Vertex *)) ;
				elist = (int (*)[4])calloc(E + 1, sizeof *elist) ;
				memset(elist, 0, sizeof *elist) ;
				vlist[0] = NULL ;
			}else{
				Vertex *v1, *v2 ;
				elist[line_n][0] = line_n ;
				elist[line_n][1] = num1 ;
				elist[line_n][2] = num2 ;
				elist[line_n][3] = 1 ;
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
		//Graph G generated
		
		a = (Vertex **)calloc(V / 2, sizeof(Vertex *));		
//		printf("a:\n");
		for(i = 0; i < V / 2; i++){
			a[i] = G->adj_list[i + 1] ;
//			pv(a[i]);
		}
		b = (Vertex **)calloc(V - V / 2, sizeof(Vertex *));		
//		printf("b:\n");
		for(i = 0; i < V - V / 2; i++){
			b[i] = G->adj_list[V / 2 + i + 1] ;
//			pv(b[i]);
		}
		printf("Initial cut size: %d \n", cut(G, a, b, NULL)) ;
		partition(G, a, b);
		printf("partitioned \n") ;
		printf("a:\n");
		for(i = 0; i < V / 2; i++){
			printf("%d ", a[i]->label) ;
		}
		printf("\nb:\n");
		for(i = 0; i < V - V / 2; i++){
			printf("%d ", b[i]->label) ;
		}
		printf("\ncut size: %d \n", cut(G, a, b, stdout)) ;
		free(a) ;
		free(b) ;
		free_graph (G);
		free(line);
		fclose(input) ;
	}
}
