#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_LIMIT 10000
#define NET_LIMIT 10000
#define BLOCK_LIMIT 2 
#define WAY 2
struct gain{
	char T ;
	int g ;
};
typedef struct gain G ;

struct linkCounter{
	int index ;
	int count ;
} ;
typedef struct linkCounter LC ;
struct vertex {
	char block ;
	unsigned char locked  ;
	unsigned char selected ;
	int gain[BLOCK_LIMIT] ;
	int ldc;
	LC* ld;
};
typedef struct vertex V;
struct net{
	int ldsc ;
	int ldc; 
	int blkCnt[BLOCK_LIMIT];
	LC* ld;
};
typedef struct net N;

struct move{
	int nodeIndex;
	char block ;
};
typedef struct move M ;

M movearr[NODE_LIMIT];
V vertexarr[NODE_LIMIT];
N netarr[NET_LIMIT];
int sumsize ;

int computeCellGain( N * narr, V * varr, char F, char T);
M  getCellWithMaxGain(const N * narr, const V * varr, int way);
int blockCount(const N * netN , const V * varr, char block);

int main(int argc, char ** argv){
	M tempmove ;
	memset(vertexarr, 0, sizeof(V) * NODE_LIMIT);
	memset(netarr, 0, sizeof(N) * NET_LIMIT);
	memset(movearr, 0, sizeof(M) * NODE_LIMIT);
	if(argc == 2){
		FILE *input = NULL;
		int state = 0;
		int index, Vindex ; 
		int ldindex = 0 ;
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
				int i ;
				case 0:
				index = number ;
				break ;

				case 1:
				netarr[index].ldsc = number ;
				break ;

				case 2:
				ldindex =  netarr[index].ldc - 1;
				for(i = ldindex ; i >= 0  ; i--){
					if(netarr[index].ld[i].index == number){
						ldindex = i  ;
						break;
					}
				}
				if(i == -1){
					netarr[index].ld = (LC*) realloc(netarr[index].ld, sizeof(LC) * (netarr[index].ldc + 1)) ;
					netarr[index].ldc += 1 ;
					ldindex += 1 ;
					netarr[index].ld[ldindex].count = 0 ;
				}
				Vindex = netarr[index].ld[ldindex].index = number;
				netarr[index].ld[ldindex].count += 1 ;

				ldindex = vertexarr[Vindex].ldc - 1;
				for(i = ldindex; i>= 0 ; i--){
					if(vertexarr[Vindex].ld[i].index == index){
						ldindex = i ;
						break ;
					}
				}
				if(i== - 1){
					vertexarr[Vindex].ld = (LC*) realloc(vertexarr[Vindex].ld, sizeof(LC) * (vertexarr[Vindex].ldc + 1));
					vertexarr[Vindex].ldc += 1;
					ldindex += 1 ;
					vertexarr[Vindex].ld[ldindex].count = 0;
				}
				vertexarr[Vindex].ld[ldindex].index = index ;
				vertexarr[Vindex].ld[ldindex].count += 1;
				
				if(Vindex % 2 == 0){
					vertexarr[Vindex].block = 'A' ;
				}else{
					vertexarr[Vindex].block = 'B' ;
				}
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
		sumsize = index ;
	}else{
		fprintf(stderr, "too few arguments\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	for( i = 1; netarr[i].ldc != 0 ; i++){
		int j = 0;
		printf("Net %d(%d): ", i, netarr[i].ldc);
		for(j = 0; j < netarr[i].ldc; j++){
			printf("%d(%d) ", netarr[i].ld[j].index, netarr[i].ld[j].count);
		} 
		putchar('\n');
		
	}
	for( i = 1; vertexarr[i].ldc != 0 ; i++){
		int j = 0 ;
		printf("Node %d(%d)[%c]: ", i, vertexarr[i].ldc, vertexarr[i].block);
		for(j = 0; j < vertexarr[i].ldc; j++){
			printf("%d(%d) ", vertexarr[i].ld[j].index, vertexarr[i].ld[j].count);
		} 
		putchar('\n');
	}
	for(i = 1; netarr[i].ldsc != 0; i++){
		int j ;
		for(j = 0; j < WAY; j++){
			netarr[i].blkCnt[j] = blockCount(netarr + i, vertexarr, j + 'A');
		}
	}
	computeCellGain(netarr, vertexarr, 'A', 'B');
	computeCellGain(netarr, vertexarr, 'B', 'A');
	for(i = 0; ; i++){
		int j;
		tempmove = getCellWithMaxGain(netarr, vertexarr, 2);
		printGain(netarr, vertexarr);
		if(tempmove.nodeIndex == 0 ){
			break ;
		}else{
			vertexarr[tempmove.nodeIndex].locked = tempmove.block ;
			movearr[i] = tempmove ;
			printf("maxMove [%d] to block %c\n", movearr[i].nodeIndex, movearr[i].block);
			updateGains(netarr, vertexarr, movearr + i);
		}
/*
		for(j = 1; netarr[j].ldsc != 0; j++){
			printf("Net%d A%d B%d\n", j, netarr[j].blkCnt[0], netarr[j].blkCnt[1]);
		}
*/
	}
/*
	for(i = 0; movearr[i].block != 0 ; i++){
		printf("move [%d] to %c\n", movearr[i].nodeIndex, movearr[i].block );
	}
*/
}
int printGain(const N * narr, const V * varr){
	int i ;
	for( i = 1; varr[i].block != 0 ; i++){
		if(!varr[i].locked){
			printf("   Gain of move [%d] from %c : %d\n", i, varr[i].block, varr[i].gain[!(varr[i].block - 'A')] );
		}
	}

}

int blockCount(const N * netN , const V * varr, char block){
	int count = 0 , i;
	for (i = 0; i < netN->ldc ; i++){
		int index = netN->ld[i].index ;
		if( varr[index].block == block){
			count += 1 ;
		}
	}
	return count ;
}
int computeCellGain( N * narr, V * varr, char F, char T){
	int i, j, k;
	for( i = 1; varr[i].block != 0 ; i++){
		if(varr[i].block == F && !varr[i].locked){
			varr[i].gain[T - 'A'] = 0 ;	
			for( j = 0; j < varr[i].ldc; j++){
				int Fcount = 0, Tcount = 0;
				int index = varr[i].ld[j].index;
				Fcount = narr[index].blkCnt[F - 'A'] ;//blockCount(narr + varr[i].ld[j].index, varr, F) ;
				Tcount = narr[index].blkCnt[T - 'A'];
				if(Fcount == 1){
					varr[i].gain[T - 'A'] += 1;
//					printf("haha +1 , node [%d] and net (%d)\n", i, varr[i].ld[j].index);
				}else {if(Tcount == 0 ){
					varr[i].gain[T - 'A'] -= 1;
//					printf("oo:( -1 , node [%d] and net (%d)\n", i, varr[i].ld[j].index);
				}}
			}
		//	printf("   Gain of move [%d] from %c to %c: %d\n", i, F, T, varr[i].gain[T - 'A'] );
		}
	}
	return 0;
}
float ratioOfBlock(char block){
	switch(block){
	case 'A':
	return 0.5 ;

	case 'B':
	return 0.5 ;
	
	default:
	return -1.0;
	}
}
int sizeOfBlock(const N * narr, const V * varr, char block){
	int i , count = 0;
	for(i = 1; varr[i].block != 0 ; i++){
		if(!varr[i].locked){
			if(varr[i].block == block){
				count += 1 ;
			}
		}else{
			if(varr[i].locked == block ){
				count += 1 ;
			}
		}
	}
	return count ;
}
int freeCells(const V * varr){
	int i , count = 0;
	for(i = 1; varr[i].block != 0; i++){
		if(!varr[i].locked){
			count += 1 ;
		}
	}
	return count  ;
}
int balanceCriterion(const N * narr, const V * varr, int V, char F, char T){
	//printf("freeCells %d\n", freeCells(varr));
	if( 
	ratioOfBlock(F) * V - (freeCells(varr)/5 + 1) <= sizeOfBlock(narr, varr, F) - 1 &&
	ratioOfBlock(F) * V + (freeCells(varr)/5 + 1)>= sizeOfBlock(narr, varr, F) - 1&&
	ratioOfBlock(T) * V - (freeCells(varr)/5 + 1) <= sizeOfBlock(narr, varr, T) + 1&&
	ratioOfBlock(T) * V + (freeCells(varr)/5 + 1) >= sizeOfBlock(narr, varr, T) + 1){
//		printf("BalanceCriterion yes\n");
		return 1 ;
	}else{
//		printf("BalanceCriterion no\n");
		return 0;
	}
}
M getCellWithMaxGain(const N * narr, const V * varr, int way){
	int i ,j;
	int maxGain = -NET_LIMIT, maxIndex = 0;
	char maxBlock ;
	M move;
	for(i = 1; varr[i].block != 0 ; i++){
		if(varr[i].locked){continue;}
		for(j = 0;j < way ; j++){
			if( j == varr[i].block - 'A'){continue;}
			if(varr[i].gain[j] > maxGain && balanceCriterion(narr, varr, 20, varr[i].block, j + 'A') ){
				maxGain = varr[i].gain[j] ;
				maxIndex = i ;
				maxBlock = j + 'A'; 
			}else{if(varr[i].gain[j] == maxGain && balanceCriterion(narr, varr, 20, varr[i].block, j + 'A')){
				if(sizeOfBlock(narr, varr, varr[i].block) - sizeOfBlock(narr, varr, j - 'A') > 
				sizeOfBlock(narr, varr, varr[maxIndex].block) - sizeOfBlock(narr, varr, maxBlock)){
					maxIndex = i ;
					maxBlock = j + 'A' ;
				}
			}}
		}
	}
	move.nodeIndex = maxIndex ;
	move.block = maxBlock ;
	
	return move;
}
int updateGains(N * narr, V * varr, M * move){
	int i ;
	for (i = 0 ; i < varr[move->nodeIndex].ldc; i++){
		int j;
		int netIndex = varr[move->nodeIndex].ld[i].index ;
		char F = varr[move->nodeIndex].block ;
		char T = move->block ;
		int Fcount = narr[netIndex].blkCnt[F-'A'];
		int Tcount = narr[netIndex].blkCnt[T-'A'];
		if(Tcount == 0){
			for(j = 0; j< narr[netIndex].ldc ; j ++){
				int index = narr[netIndex].ld[j].index ;
				if(varr[index].locked){continue;}
				varr[index].gain[T - 'A'] += 1;
				printf("[%d] + 1\n", index );
			}
		}else{if(Tcount == 1){
			for(j = 0; j< narr[netIndex].ldc; j++){
				int index = narr[netIndex].ld[j].index ;
				if(varr[index].locked||varr[index].block != T ){continue;}
				varr[index].gain[F - 'A'] -= 1;
				printf("[%d] - 1\n", index );
			}
		}}
		narr[netIndex].blkCnt[F-'A'] -= 1;
		narr[netIndex].blkCnt[T-'A'] += 1;
		Fcount = narr[netIndex].blkCnt[F-'A'];
		Tcount = narr[netIndex].blkCnt[T-'A'];
		if(Fcount == 0){
			for(j = 0; j< narr[netIndex].ldc ; j ++){
				int index = narr[netIndex].ld[j].index ;
				if(varr[index].locked){continue;}
				varr[index].gain[F - 'A'] -= 1;
				printf("[%d] - 1\n", index );
			}
		}else{if(Fcount == 1){
			for(j = 0; j< narr[netIndex].ldc; j++){
				int index = narr[netIndex].ld[j].index ;
				if(varr[index].locked || varr[index].block != F ){continue;}
				varr[index].gain[T - 'A'] += 1;
				printf("[%d] + 1\n", index );
			}
		}}
	}
}
