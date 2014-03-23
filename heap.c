#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "heap.h"

 int left(int index){
	return index * 2 ;
}
 int right(int index){
	return 1 + index * 2 ;
}
 int parent(int index){
	return index / 2 ;
}
void exchange(Heap *h, int index1, int index2){
	int temp = h->keys[ index1 ] ;
	h->keys[ index1 ] = h->keys[ index2 ] ;
	h->keys[ index2 ] = temp ;
	h->indices[ h->keys[ index1 ] ] = index1 ;
	h->indices[ h->keys[ index2 ] ] = index2 ;
}

void min_heapify(Heap *h, int i){
	int l = left(i), r = right(i), minimum ;
	if(l <= h->size && (*h->values)( h->keys[ l ] ) < (*h->values)( h->keys[ i ] )){
		minimum = l ;
	}else{
		minimum = i ;
	}
	if(r <= h->size && (*h->values)( h->keys[ r ] ) < (*h->values)( h->keys[ minimum ] )){
		minimum = r ;
	}
	if( minimum != i){
		exchange( h, i, minimum) ;
		min_heapify(h, minimum) ;
	}

}

void max_heapify(Heap *h, int i){
	int l = left(i), r = right(i), maximum ;
	if(l <= h->size && (*h->values)( h->keys[ l ] ) > (*h->values)( h->keys[ i ] )){
		maximum = l ;
	}else{
		maximum = i ;
	}
	if(r <= h->size && (*h->values)( h->keys[ r ] ) > (*h->values)( h->keys[ maximum ] )){
		maximum = r ;
	}
	if( maximum != i){
		exchange( h, i, maximum) ;
		max_heapify(h, maximum) ;
	}
}
void max_update(Heap *h, int key, bool smaller){
	int index; 
	if( key >= 1 && key <= h->max_size){
		index = h->indices[key] ;
	}else{
		perror("No such key") ;
		exit(EXIT_FAILURE) ;
	}
	if( index > h->size || index < 1){
		perror("The key has been deleted or no such key");
		exit(EXIT_FAILURE) ;
	}
	if( smaller){
		max_heapify(h, index) ;
	}else{
		while( (*h->values)(h->keys[parent(index)]) < (*h->values)(h->keys[index]) && index > 1){
			exchange(h, parent(index), index);
			index = parent(index) ;
		}
	}
}
void min_update(Heap *h, int key, bool larger ){
	int index; 
	if( key >= 1 && key <= h->max_size){
		index = h->indices[key] ;
	}else{
		perror("No such key") ;
		exit(EXIT_FAILURE) ;
	}
	if( index > h->size || index < 1){
		perror("The key has been deleted or no such key");
		exit(EXIT_FAILURE) ;
	}
	if( larger){
		min_heapify(h, index) ;
	}else{
		while((*h->values)(h->keys[parent(index)]) > (*h->values)(h->keys[index]) && index > 1){
			exchange(h, parent(index), index);
			index = parent(index) ;
		}
	}
}
void set_type(Heap *h, heap_t t){
	if(t != h->t){
		h->t = t ;
		switch(t){
			case MAX_h:
			h->heapify = max_heapify ;
			h->update = max_update ;
			break;

			case MIN_h:
			h->heapify = min_heapify ;
			h->update = min_update ;
			break;

			default:
			perror("Unknown heap type") ;
			exit(EXIT_FAILURE) ;
			break;
		}
	}
}

void build_heap(Heap *h){
	int i ;
	for(i = h->size / 2; i > 0; i--){
		(*h->heapify)(h, i) ;
	}
}

/* 
 * remove the key and set corresponding index to 0
 * if the key is 0 simple pop the first element in the heap (min element for min-heap and max element for max-heap)
 * the returned value is the corresponding value for the key
 */
int pop(Heap *h, int key){
	int index ;
	if( key == 0 ){
		index = 1 ;
		key = h->keys[1] ;
	}else if( key >= 1 && key <= h->max_size){
		index = h->indices[key] ;
	}else{
		perror("No such key") ;
		exit(EXIT_FAILURE) ;
	}
	if(index <= h->size && index >= 1){
		h->keys[index] = h->keys[h->size] ;
		h->indices[ h->keys[ index ] ] = index ;
		h->indices[ key ] = 0 ;
		h->size -= 1 ;
		if( index <= parent(h->size) ){
			(*h->heapify)(h, index) ;
		}
		return (*h->values)( key ) ;
	}else{
		perror("The key has been deleted or no such key");
		exit(EXIT_FAILURE) ;
	}

}
/*
void insert(Heap *h, int key, int value){
	int index ;
	if( key >= 1 && key <= h->max_size){
		index = h->indices[key] ;
	}else{
		perror("No such key") ;
		exit(EXIT_FAILURE) ;
	}	
	if(index <= h->size && index >= 1){
		(*h->update)(h, key, value) ;
	}else if(index == 0){
		if(h->size >= h->max_size){
			perror("size overflow") ;
			exit(EXIT_FAILURE) ;
		}
		h->size += 1 ;
		(*h->values)(key) = value ;
		h->keys[h->size] = key ;
		h->indices[key] = h->size ;
		(*h->update)(h, key, value) ;
	}else{
		perror("Index out of range") ;
		exit(EXIT_FAILURE);
	}
}
*/
Heap *new_heap(int max_size, int (*values)(int ), heap_t min_or_max){
	Heap *h = (Heap *)malloc(sizeof *h) ;
	int i, j;
	h->sorted = false ;
	h->max_size = max_size ;
	h->t = UNSET_h ;
	h->values = values ;
	h->indices = (int *)calloc( (max_size + 1) , sizeof *(h->indices)) ;
	h->keys = (int *)calloc( (max_size + 1) , sizeof *(h->keys)) ;
	h->size = max_size ;
	for(i = 0; i <= max_size ; i++){
		h->keys[i] = i ;
		h->indices[ i] = i ;
	}
	set_type(h, min_or_max) ;
//	build_heap(h) ;
	return h ;
}

void sort(Heap *h){
	int original_size = h->size, i ;
	for (i = original_size; i > 1; i--){
		exchange(h, 1, i) ;
		h->size -= 1;
		h->heapify(h, 1) ;
	}
	h->sorted = true ;
	h->t = !h->t ;
	h->size = original_size ;
}
void free_heap(Heap *h){
	free(h->indices) ;
	free(h->keys) ;
	free(h) ;
}
void update(Heap *h, int key, bool down){
	return (*h->update)(h, key, down) ;
}
void heapify(Heap *h, int index) {
	return (*h->heapify)(h, index) ;
}
bool has(Heap *h, int key){
	return (bool)(!!(h->indices[key])) ;
}
void ph(Heap *h, int index){

	if(index == 0){
		int i;
		for(i = 1; i <= h->size; i++){
			ph(h, i) ;
		}
	}else{
		printf("%d %d %d %d\n", index, h->keys[index], h->indices[ h->keys[ index ] ], (*h->values)( h->keys[ index ] ) ) ;
	}

}
/*
 * uncomment this to test
int main(){
	Heap *h = new_heap(10, 10, NULL, MAX_h) ;
	p(h, 0);
	sort(h) ;
	p(h, 0) ;
	free_heap(h);
	return 0;
}
*/
