#ifndef __HEAP_H__
#define __HEAP_H__

#define VALUE_RANGE 1000 


typedef enum _heap_type {
	MAX_h = 0, 
	MIN_h,
	UNSET_h 
} heap_t;

#ifndef __cplusplus
typedef enum _bool{
	false = 0, 
	true  
} bool ;
#endif

typedef struct _Heap{
	bool sorted ;
	int max_size ;
	int size ;
	int *keys ;
	int (*values)(int index) ;
	int *indices ;
	heap_t t ;
	void (*heapify)(struct _Heap *h, int index) ;
//	void (*update)(struct _Heap *h, int key, int value) ;
} Heap ;

#ifndef __HEAP_FUN__
#define __HEAP_FUN__

extern void sort(Heap *h) ;
extern int pop(Heap *h, int key) ;
extern void insert(Heap *h, int key, int value) ;
extern Heap *new_heap(int max_size, int (*values)(int ), heap_t min_or_max) ;
extern bool has(Heap *h, int key) ;
extern void ph(Heap *h, int index) ;
extern void free_heap(Heap *h) ;

#endif
#endif
