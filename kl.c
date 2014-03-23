#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "heap.h"

/*
 * generate a random input file
 */
void input_gen(FILE *output, int D, int V){
	Graph *G = gen(D, V) ;
	if(output == NULL){
		return ;
	}
	edges(G, output) ;
	free_graph(G) ;
	return ;
}

/*
 * show the cut set info 
 * print the cut set by edge pairs
 */
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
		fprintf(output, "\n\nCut set: \n\n") ;
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

/*
 * do the kl bi-partitioning
 */
void partition(Graph *G, Vertex *a[], Vertex *b[]){
	int minus_inf = (int)(~0) << (sizeof(int) * 8  /  2), V = G->V  ;
	int
	*d = (int *)calloc(V + 1, sizeof *d ), //d[V + 1] 
	(*cost)[V + 1] = (int (*)[V + 1])calloc(V + 1, sizeof *cost),//cost[V + 1][V + 1], 
	*gsum = (int *)calloc(V / 2 + 1, sizeof *gsum), //gsum[V / 2 + 1], 
	(*ex)[2] = (int (*)[2])calloc(V / 2 + 1, sizeof *ex),//ex[V / 2 + 1][2], 
	*locked = (int *)calloc(V + 1, sizeof *locked), //locked[V + 1] ,
	*indices = (int *)calloc(V + 1, sizeof *indices) ,
	maxk, maxgain, passes = 0, cumulative_gain = 0, size_a = V / 2, size_b = V - V / 2  ;
	register int i, j, k  ;

	char *block = (char *)calloc(V + 1, sizeof *block) ; //block[V + 1] ;
	Vertex *v1, *v2 ;
	Heap *h;
	inline int values(int i){
		int label_a = a[ (i / size_b) % size_a ]->label, label_b = b[ i % size_b ]->label ;
		if(locked[ label_a ] + locked[ label_b ]){
			return minus_inf ;
		}else{
			return 
			d[ label_a ] + 
			d[ label_b ] - 
			2 * cost[ label_a ][ label_b ] ;
		}
	}

	h = new_heap(size_a * size_b, values, MAX_h) ;

	mainloop:
	//Initialization
	memset(d, 0, (V + 1) * sizeof *d)  ;
	memset(cost, 0, (V + 1) * sizeof *cost) ;
	memset(gsum , 0, (V / 2 + 1) * sizeof *gsum ) ;
	memset(ex, 0, (V / 2 + 1) * sizeof *ex) ;
	memset(block, 0, (V + 1) * sizeof *block) ;
	memset(locked, 0, (V + 1) * sizeof *locked ) ;

	gsum[0] = 0 ;
	maxk = 0 ;
	maxgain = minus_inf ;

	for(i = 0; i < V / 2; i++){
		block[ a[i]->label ] = 'a' ;
		block[ b[i]->label ] = 'b' ;
		indices[ a[i]->label ] = i ;
		indices[ b[i]->label ] = i ;
	}	
	if( V - V / 2  != V / 2) {
		int i = V - V / 2 - 1 ;
		block[ b[ i ]->label ] = 'b' ;
		indices[ b[ i ]->label ] = i ;
	}

	//Compute the D value
	for (i = 0; i < V / 2; i++){
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

	if( V - V / 2  != V / 2) {
		v2 = b[V - V / 2 - 1] ;
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
	build_heap(h) ;
	for (k = 1; k <= V / 2; k++){
		int  to_be_exchanged[2], label_a, label_b, index_a, index_b;
		/*
		for(i = 0; i < V / 2 ; i++){
			int a_i_label = a[i]->label ;
			if (!locked[ a_i_label ]) {
				for(j = 0; j < V - V / 2  ; j++){
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
		*/
		maxgain = pop(h, 0) ;
		index_a = (h->keys[1] / size_b) % size_a ;
		index_b = h->keys[1] % size_b ;
		label_a = a[index_a]->label ;
		label_b = b[index_b]->label ;

		locked[ label_b ] = 1 ;
		locked[ label_a ] = 1 ;
		ex[k][0] = index_a ;
		ex[k][1] = index_b ;		

		for(i = 0; i < V / 2; i++){
			if(i != index_a){
				int key = i * size_b + index_b == 0 ? h->keys[size_a * size_b] : h->keys[ ( i * size_b + index_b ) ] ;
				pop(h, key) ;
			}
			if(i != index_b){
				int key = index_a * size_b + i == 0 ? h->keys[size_a * size_b] : h->keys[ (index_a * size_b + i ) ] ;
				pop(h, key);
			}
		}
		if(V / 2 != V - V / 2){
			int i = V - V / 2 - 1;
			if(i != index_b){
				int key = h->keys[ (index_a * size_b + i ) ] ;
				pop(h, key);
            }
        }
		//update D values 
		v1 = a[ index_a ] ;
		v2 = b[ index_b ] ;
		for(i = 0; i < v1->degree; i++){
			int label = v1->list[i][0], index  ;
			if(!locked[label]){
				bool same_block = (block[ v1->label ] == block[ label ]) ;
				if(same_block){
					d[ label ] = 
					d[ label ] + 2 * cost[ v1->label ][ label ] ;
					index = indices[ label ] ;
					if(index == 0){
						if(!locked[ b[0]->label ]){
							update(h, size_a * size_b, false) ;						
						}
						for(j = 1; j < V - V / 2; j++){
							if(!locked[ b[j]->label ]){
								update(h, index * size_b + j, false) ;
							}
						}
					}else{
						for(j = 0; j < V - V / 2; j++){
							if(!locked[ b[j]->label ]){
								update(h, index * size_b + j, false) ;
							}
						}
					}

				}else{
					d[ label ] = 
					d[ label ] - 2 * cost[ v1->label ][ label ] ;

					index = indices[ label ] ;
					if(index == 0){
						if(!locked [ a[0]->label ]){
							update(h, size_a * size_b,true) ;
						}
						for(j = 1; j < V - V / 2; j++){
							if(!locked[ a[j]->label ]){
								update(h, j * size_b + index, true) ;
							}
						}
					}else{
						for(j = 0; j < V - V / 2; j++){
							if(!locked[ a[j]->label ]){
								update(h, j * size_b + index, true) ;
							}
						}
						
					}	
				}
			}
		}
		for(i = 0; i < v2->degree; i++){
			int label = v2->list[i][0], index ;
			if(!locked[label]){

				bool other_block = (block[ v2->label ] != block[ label ]) ;
				if(other_block){
					d[ label ] = 
					d[ label ] - 2 * cost[ v2->label ][ label ] ;
					index = indices[ label ] ;
					if(index == 0){
						if(!locked[ b[0]->label ]){
							update(h, size_a * size_b, true) ;						
						}
						for(j = 1; j < V - V / 2; j++){
							if(!locked[ b[j]->label ]){
								update(h, index * size_b + j, true) ;
							}
						}
					}else{
						for(j = 0; j < V - V / 2; j++){
							if(!locked[ b[j]->label ]){
								update(h, index * size_b + j, true) ;
							}
						}
					}

				}else{
					d[ label ] = 
					d[ label ] + 2 * cost[ v2->label ][ label ] ;

					index = indices[ label ] ;
					if(index == 0){
						if(!locked [ a[0]->label ]){
							update(h, size_a * size_b, false) ;						
						}
						for(j = 1; j < V - V / 2; j++){
							if(!locked[ a[j]->label ]){
								update(h, j * size_b + index, false) ;
							}
						}
					}else{
						for(j = 0; j < V - V / 2; j++){
							if(!locked[ a[j]->label ]){
								update(h, j * size_b + index, false) ;
							}
						}
						
					}	
				}
			}
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
		free_heap(h) ;
		printf("\nPasses: %d, cumulative gain: %d \n", passes, cumulative_gain);
		return ;
	}else{
		passes += 1 ;
		cumulative_gain += gsum[maxk ] ;
		for(i = 1; i <= maxk; i++){
			Vertex *temp = a[ ex[i][0] ] ;
			a[ ex[i][0] ] = b[ ex[i][1] ] ;
			b[ ex[i][1] ] = temp ;
			indices[ a[ ex[i][0] ]->label ] = ex[i][0] ;
			indices[ b[ ex[i][0] ]->label ] = ex[i][1] ;
		}
		free_heap(h) ;
		goto mainloop ;
	}
}

int main(int argc, char ** argv){
	if(argc == 2){
		FILE *input, *output; //input file
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		int line_n = 0, V, E, (*elist)[4], i, j, init_cut_size ;
		Vertex **vlist, **a, **b ;
		Graph *G, *G2 ;
/*
 *Generate input by itself
 *
		output = fopen(argv[1], "w") ;
		input_gen(output, 2, 20) ;
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
		printf("\nPartitioned \n") ;
		printf("\nFirst partition :\n");
		for(i = 0; i < V / 2; i++){
			printf("%d ", a[i]->label) ;
		}
		printf("\nSecond partition :\n");
		for(i = 0; i < V - V / 2; i++){
			printf("%d ", b[i]->label) ;
		}
		printf("\nCut size after partitioning (initial cut size - cumulative gain):  %d \n", cut(G, a, b, stdout)) ;
		free(a) ;
		free(b) ;
		free_graph (G);
		free(line);
		fclose(input) ;
	}
}
