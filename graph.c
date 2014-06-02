/***************************************
 April 2014 Chaofan Li <chaof@tamu.edu>
***************************************/
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

Vertex *new_vertex(int label){
	Vertex *v = (Vertex *)malloc( sizeof (Vertex)) ;
	v->degree = 0;
	v->label = label;
	v->list = NULL;
	return  v ;
}
Graph *new_graph(int V, Vertex *vertex_list[]){
	int i = 0;
	Graph * G = (Graph *)malloc( sizeof (Graph) ) ;
	G->V = V ;
	G->E = 0 ;
	G->adj_list = (Vertex **)calloc( V + 1, sizeof (Vertex *)) ;
	G->edge_list = (int (*)[2])calloc(1, sizeof *(G->edge_list)) ;
	G->edge_pair =  (int (*)[2])calloc(1, sizeof *(G->edge_pair)) ;
	for( i = 0; i < G->V ; i++){//Make the adjacency list sorted by the label
		G->adj_list[ vertex_list[i]->label ] = vertex_list[ i  ] ;
	}
	G->adj_list[0] = new_vertex(0) ;
	memset(G->edge_list, 0, sizeof *(G->edge_list)) ;
	memset(G->edge_pair, 0, sizeof *(G->edge_pair)) ;
	return G ;
}

void free_vertex(Vertex *v){
	if(v == NULL){return ;}
	if(v->list != NULL){
		free(v->list) ;
	}
	free(v) ;
}
void free_graph(Graph *G){
	int i = 0;
	if(G == NULL){return ;}
	if(G->adj_list != NULL){
		for(i = 0; i <= G->V; i++){
			free_vertex(G->adj_list[i]); 
		}
		free(G->adj_list) ;
	}
	if(G->edge_list != NULL){
		free(G->edge_list) ;
	}
	if(G->edge_pair != NULL){
		free(G->edge_pair) ;
	}
	free(G);
}
void add_adjacency_vertex(Vertex *v, int label, int weight) {
	v->degree += 1 ;
	v->list = (int (*)[2])realloc( v->list, v->degree * sizeof *(v->list) ) ;
	v->list[v->degree - 1][0] = label ;
	v->list[v->degree - 1][1] = weight ;
}

void pv(Vertex *v, FILE *fp){
	int i ;
	fprintf(fp, "%d -> ", v->label) ;
	for(i = 0; i < v->degree; i++){
		fprintf(fp, "[%d %d] ", v->list[i][0], v->list[i][1]);
	}
	fputc('\n', fp);
}
void pg(Graph *g, FILE *fp){
	int i;
	for(i = 1; i <=g->V; i++){
		pv(g->adj_list[i], fp) ;
	}
}

/*
	D is the degree for every vertex
	V is the total number of vertices
	generate a random graph with V vertices, and every vertex has exact degree of D
*/
Graph *gen(int D, int V){
	Vertex 
	*(*sets)[V] = (Vertex *(*)[V])calloc(3, sizeof *sets),
	*v1 = NULL, *v2 = NULL;
	int dgr[3] = {-1, 0, 1}, len[3] = {0, 0, 0}, min_index, new_index, i ;
	if( D  > V - 1){
		perror("No such graph: total degree should be less than of equal to 2 x maximal number of edge") ;
		exit(EXIT_FAILURE) ;
	}
	for (i = 0 ; i < V; i++){
		sets[1][i] = new_vertex(i + 1) ;
	}
	len[1] = V ;
	min_index = 1;
	while(len[0] < V - 1){
		int v = rand() % (V - len[0]), l, pl  ;
		if( v / len[ min_index ] == 0){
			v1 = sets[min_index][v] ;
			memmove(sets[min_index] + v, sets[min_index] + v + 1, (len[min_index] - v - 1) * sizeof(Vertex *)) ;
			len[min_index] -= 1 ;//pop (v)
			if(len[min_index] == 0){
				min_index = ! (min_index - 1) + 1;
			}
		}else{
			new_index = ! (min_index - 1) + 1 ;
			v -= len[min_index] ;	
			v1 = sets[new_index][v] ;
			memmove(sets[new_index] + v, sets[new_index] + v + 1, (len[new_index] - v - 1) * sizeof(Vertex *)) ;
			len[new_index] -= 1 ;//pop (v)
		}
		l = D - v1->degree ;
		pl = 0 ;//Pop length 
		while(l > 0 && dgr[min_index] < D){
			int untouched_len = len[min_index] - pl, weight ;
			new_index = ! (min_index - 1) + 1 ;
			if(l < untouched_len){
				for(i = 0; i < l; i++){
					v = rand() % (len[min_index] - pl) ;
					v2 = sets[min_index][v] ;
					memmove(sets[min_index] + v, sets[min_index] + v + 1, (len[min_index] - v - 1) * sizeof(Vertex *) ) ;
					len[min_index] -= 1 ;
					weight = 1 + rand() % MAX_EDGE_WEIGHT ;
					add_adjacency_vertex(v2, v1->label, weight);
					add_adjacency_vertex(v1, v2->label, weight);
					if(v2->degree == D){
						sets[0][ len[0] ] = v2 ;
						len[0] += 1 ;
					}else{
						sets[new_index][ len[new_index] ] = v2 ;
						len[new_index] += 1 ;
					}
				}
				dgr[new_index] = dgr[min_index] + 1 ;				
				l = 0 ;
			}else{
				for (i = 0; i < untouched_len; i++){
					v = rand() % (len[min_index] - pl) ;
					v2 = sets[min_index][v] ;
					memmove(sets[min_index] + v, sets[min_index] + v + 1, (len[min_index] - v - 1) * sizeof(Vertex *) ) ;
					len[min_index] -= 1;//pop(v)
					weight = 1 + rand() % MAX_EDGE_WEIGHT ;
					add_adjacency_vertex(v2, v1->label, weight);
					add_adjacency_vertex(v1, v2->label, weight);
					if(v2->degree == D){
						sets[0][ len[0] ] = v2 ;
						len[0] += 1 ;
					}else{
						sets[new_index][ len[new_index] ] = v2 ;
						len[new_index] += 1 ;
					}
				}
				l -= untouched_len ;
				pl = untouched_len ;
				dgr[new_index] = dgr[min_index] + 1 ;
				min_index = new_index ;
			}
		}
		sets[0][ len[0] ] = v1 ;
		len[0] += 1 ;
	}
	if(len[0] == V){
		Graph *G = new_graph(V, sets[0]) ;
		free(sets) ;
		return G ;
	}else{
		perror("No such graph: Returned graph has a vertex with degree less than min_index") ;
		sets[0][len[0] ] = sets[min_index][0] ;
		Graph *G = new_graph(V, sets[0]) ;
		free(sets) ;
		return G ;
	}
}
void edges(Graph * G, FILE *output){
	int i, j;
	if(G->E > 0 ){
		if( output == NULL){
			return ;
		}else{
			fprintf (output, "%d  %d\n", G->V, G->E) ;		
			for(i = 1; i <= G->E; i++){
				fprintf(output, "%d %d\n", G->edge_pair[i][0], G->edge_pair[i][1]) ;
			}
		}
	}else{
		for( i = 1; i <= G->V; i++){
			Vertex *v = G->adj_list[i] ;
			for( j = 0; j < v->degree; j++){
				if(v->label < v->list[j][0]){
					G->E += 1 ;
					G->edge_list = (int (*)[2])realloc(G->edge_list, (G->E + 1)* sizeof *(G->edge_list)); 		
					G->edge_list[ G->E ][0] = G->E ;	
					G->edge_list[ G->E ][1] = v->list[j][1] ;	
					G->edge_pair = (int (*)[2])realloc(G->edge_pair, (G->E + 1)* sizeof *(G->edge_pair)); 		
					G->edge_pair[ G->E ][0] = v->label ;	
					G->edge_pair[ G->E ][1] = v->list[j][0] ;	
				}
			}
		}
		edges(G, output) ;
	}
}
/*
 *uncomment this to see sample output
int main(){
	Graph *G ;
	G = gen(3, 1000);
	pg(G);
	free_graph(G);
	return 0 ;
}
*/
