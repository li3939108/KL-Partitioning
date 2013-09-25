#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_LIMIT 10000
#define NET_LIMIT 10000
#define BLOCK_LIMIT 5
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

int TOTAL_NODES ;

int gainSum[NODE_LIMIT] ;
int balanceValue[NODE_LIMIT ];

int TOTAL_NETS ;

int gainSumMax = -NODE_LIMIT;
int gainSumMaxIndex = -1 ;

int computeCellGain( N * narr, V * varr, char F, char T);
M  getCellWithMaxGain(const N * narr, const V * varr, int way);
int blockCount(const N * netN , const V * varr, char block);
int balanceCriterion(const N * narr, const V * varr, int V, char F, char T);
int printNetList(const N * narr);

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
				//Initial Random Partitioning
				if(Vindex / 11 == 0){
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
		TOTAL_NETS = index ;

		int i,j = 0;
		for( i = 0; i < NODE_LIMIT  ; i++){
			if(vertexarr[i].ldc != 0){
				j += 1;
			}
		}
		TOTAL_NODES = j ;
		printf("TOTAL NODE: %d\n", TOTAL_NODES);
	}else{
		fprintf(stderr, "too few arguments\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	
	printNetList(netarr);
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
///////////////////////
	start:
	while(1){
	int fromBlkCnt = 0, toBlkCnt = 0;
	for(fromBlkCnt = 0 ; fromBlkCnt < WAY; fromBlkCnt ++){
		for(toBlkCnt = 0 ; toBlkCnt < WAY; toBlkCnt ++){
			if(toBlkCnt != fromBlkCnt){
				computeCellGain(netarr, vertexarr, 'A' + fromBlkCnt, 'A' + toBlkCnt);
			}
		}
	}
/*
	computeCellGain(netarr, vertexarr, 'A', 'B');
	computeCellGain(netarr, vertexarr, 'B', 'A');
*/
	for(i = 0; ; i++){
		int j;
		tempmove = getCellWithMaxGain(netarr, vertexarr, WAY);
		printGain(netarr, vertexarr);
		gainSum[i] = (i - 1 >= 0 ? gainSum[i - 1] : 0) + vertexarr[tempmove.nodeIndex].gain[tempmove.block - 'A'] ;
		balanceValue[i] = balanceCriterion(netarr, vertexarr, 20, vertexarr[tempmove.nodeIndex].block, tempmove.block);
		if(gainSum[i] > gainSumMax){
			gainSumMax = gainSum[i] ;
			gainSumMaxIndex = i ;
		}else{if(gainSum[i] == gainSumMax){
			if(balanceValue[i] > balanceValue[gainSumMaxIndex]){//> is better 
				gainSumMax = gainSum[i] ;
				gainSumMaxIndex = i ;
			}
		}}
		if(tempmove.nodeIndex == 0 ){
			break ;
		}else{
			vertexarr[tempmove.nodeIndex].locked = tempmove.block ;
			movearr[i] = tempmove ;
			printf("maxMove [%d] to block %c\n", movearr[i].nodeIndex, movearr[i].block);
			updateGains(netarr, vertexarr, movearr + i);
		}
	}
	if(gainSumMax <= 0){// < 0 causes loop forever
	//	goto end ;
		break;
	}
	gainSum[i + 1] = -NODE_LIMIT ;
	for(i = 0; gainSum[i] != -NODE_LIMIT; i++){
		printf("gainsum [%d] is %d\n", i, gainSum[i]);
	}
	printf("max gain sum [%d] %d \n",gainSumMaxIndex, gainSum[gainSumMaxIndex] );
	for(i = 0; i <= gainSumMaxIndex; i++){
		vertexarr[movearr[i].nodeIndex].block = movearr[i].block ;
	}
	for(i = 1; vertexarr[i].block != 0 ;i++){
		vertexarr[i].locked = 0 ;
	}
	gainSumMax  = -NODE_LIMIT ; 
	gainSumMaxIndex = -1 ;
	printf("---------------\n");
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
	printf("end of pass\n");
	}
	return 0 ;
}
int printNetList(const N * narr){
	int i ;
	for( i = 1; netarr[i].ldc != 0 ; i++){
		int j = 0;
		printf("Net %d(%d): ", i, netarr[i].ldc);
		for(j = 0; j < netarr[i].ldc; j++){
			printf("%d(%d) ", netarr[i].ld[j].index, netarr[i].ld[j].count);
		} 
		putchar('\n');
	}
	return 0 ;
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
				Fcount = narr[index].blkCnt[F - 'A'];
				Tcount = narr[index].blkCnt[T - 'A'];
				if(Fcount == 1){
					varr[i].gain[T - 'A'] += 1;
				}else {if(Tcount == 0 ){
					varr[i].gain[T - 'A'] -= 1;
				}}
			}
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

	case 'C':
	return 0 ;
	
	case 'D':
	return 0 ;
	
	case 'E':
	return 0;
	
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
	/*
	freeCells(varr)/5
        freeCells(varr)/5
        freeCells(varr)/5
        freeCells(varr)/5
	*/
	if( 
	ratioOfBlock(F) * V - ( + 1) <= sizeOfBlock(narr, varr, F) - 1 &&
	ratioOfBlock(F) * V + ( + 1)>= sizeOfBlock(narr, varr, F) - 1&&
	ratioOfBlock(T) * V - ( + 1) <= sizeOfBlock(narr, varr, T) + 1&&
	ratioOfBlock(T) * V + ( + 1) >= sizeOfBlock(narr, varr, T) + 1){
		return NODE_LIMIT - abs(ratioOfBlock(F) * V - (sizeOfBlock(narr, varr, F) - 1)) - abs(ratioOfBlock(F) * V - (sizeOfBlock(narr, varr, T) + 1)) ;
	}else{
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
