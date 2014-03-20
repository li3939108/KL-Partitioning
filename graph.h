#ifndef __GRAPH_H__
#define __GRAPH_H__

#define MAX_EDGE_WEIGHT 5000
#include <stdio.h>


typedef struct _Vertex{
	int degree ;
	int label ;
	int rank ;
	int **list;//[label, weight] pair
	struct _Vertex *parent ;
} Vertex;

typedef struct _Graph{
	int V ;
	int E ;
	int **edge_list ; //[label, v1, v2, weight] pair
	Vertex **adj_list ;
} Graph;

#ifndef __GRAPH_FUN__
#define __GRAPH_FUN__

extern Graph *new_graph(int V, Vertex *vertex_list[]) ;
extern Vertex *new_vertex(int label) ;
extern void free_vertex(Vertex *v);
extern void free_graph(Graph *G);
extern void add_adjacency_vertex(Vertex *v, int label, int weight) ;
extern Graph *gen(int D, int V) ;
extern void pg(Graph *) ;
extern void pv(Vertex *) ;
extern void edges(Graph *, FILE* output) ;

#endif
#endif
