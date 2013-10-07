#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_LIMIT 10000
#define NET_LIMIT 10000
#define BLOCK_LIMIT 5
int WAY = 2;
int WEIGHT[BLOCK_LIMIT] = {1, 1, 0, 0 ,0} ;
int WEIGHT_SUM = 2;


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
	int ldsc ; //count of edges on this net
	int ldc; //count of nodes on this net
	int blkCnt[BLOCK_LIMIT];// count of nodes on a block
	LC* ld;// nodes sequence
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
int TOTAL_NETS ;


int gainSum[NODE_LIMIT] ;
int balanceValue[NODE_LIMIT ];


int gainSumMax = -NODE_LIMIT;
int gainSumMaxIndex = -1 ;

int computeCellGain( N * narr, V * varr, char F, char T);
M  getCellWithMaxGain(const N * narr, const V * varr, int way);
int blockCount(const N * netN , const V * varr, char block);
int balanceCriterion(const N * narr, const V * varr, int V, char F, char T);
int printNetList(const N * narr);

int computeNetCut(N * narr, V * varr, int way){
	int i;
	int sumCnt = 0;
	for (i = 0; i <= TOTAL_NETS; i++){
		if(narr[i].ldc != 0){//not a empty net or nonexist net
			int j , cnt = -1; //net on 2 blocks has 1 net-cut, net on 3 blocks has 2 net-cut ... 
			for (j = 0; j < way; j++){
				if(narr[i].blkCnt[ j] > 0){
					cnt += 1;
				}
			}
			sumCnt += cnt ;	
		}
	}
	return sumCnt ;
}

int main(int argc, char ** argv){
	M tempmove ;
	memset(vertexarr, 0, sizeof(V) * NODE_LIMIT);
	memset(netarr, 0, sizeof(N) * NET_LIMIT);
	memset(movearr, 0, sizeof(M) * NODE_LIMIT);
	if(argc >= 2){
		FILE *input = NULL;
		int state = 0, parameterState = 0;
		int index, Vindex, i = 1, parameter = 0; 
		int ldindex = 0 ;
		/*
		 * Read parameters
		 */
		while( i < argc){
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
				int iter  ;
				case 'k'://must be defined before -w
				if( i + 1 < argc){WAY = atoi(argv[ i + 1 ]);}
				else{
					perror("Unspecified parameter for k");
					exit(EXIT_FAILURE) ;
				}
				for(iter = 0; iter < WAY; iter ++){
					WEIGHT[iter] = 1 ;
				}
				WEIGHT_SUM = WAY;
				i = i + 2 ;
				continue ;
				break;
				
				case 'w':
				WEIGHT_SUM = 0;
				for(iter = 0; iter < WAY; iter ++){
					if( i + 1 + iter < argc){
						WEIGHT[iter] = atoi(argv[i + 1 + iter ]) ;
					}else{
						perror("Unspecified parameter for w");
						exit(EXIT_FAILURE) ;
					}
					WEIGHT_SUM += WEIGHT[iter] ;
				}
				i = i + WAY + 1;
				continue;
				break ;

				default:
				perror("parameter error");
				exit(EXIT_FAILURE);
				break;
				}
			}else{
				input = fopen(argv[i], "r");
			}
			i++;
		}
		/*
		 * Read the input
		 */
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
				int i , wsum = 0;
				case 0:
				index = number ;
				break ;

				case 1:
				netarr[index].ldsc = number ;
				break ;

				case 2:
				ldindex =  netarr[index].ldc - 1;
				for(i = ldindex ; i >= 0 ; i--){
					if(netarr[index].ld[i].index == number){
						ldindex = i  ;
						break ;
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
				/*
				 * Initial Random Partitioning
				 */
				vertexarr[Vindex].block = 'A' ;
				for(i = 0; i < WAY - 1; i++){
					wsum += WEIGHT[i] ;
					vertexarr[Vindex].block += (((Vindex - 1) % WEIGHT_SUM) >= ( wsum)) ? 1 : 0 ;
				}
				wsum = 0;
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
		/*
		 * Calculate the total nodes
		 */
		int j = 0;
		for( i = 0; i < NODE_LIMIT  ; i++){
			if(vertexarr[i].ldc != 0){
				j += 1;
			}
		}
		TOTAL_NODES = j ;
	}else{
		perror("too few arguments");
		exit(EXIT_FAILURE);
	}

	//printNetList(netarr);
	//printNodeList(vertexarr);
	int i = 0;
///////////////////////
	/*
	 * Main loop of the F-M heuristic
	 */
	while(1){
	int fromBlkCnt = 0, toBlkCnt = 0;
	//Calculate the nodes of a certain net on a certain block
	for(i = 1; netarr[i].ldsc != 0; i++){
		int j ;
		for(j = 0; j < WAY; j++){
			netarr[i].blkCnt[j] = blockCount(netarr + i, vertexarr, j + 'A');
		}
	}
	printf("sumNetCut %d\n\n\n", computeNetCut( netarr, vertexarr, WAY) );
	for(fromBlkCnt = 0 ; fromBlkCnt < WAY; fromBlkCnt ++){
		for(toBlkCnt = 0 ; toBlkCnt < WAY; toBlkCnt ++){
			if(toBlkCnt != fromBlkCnt){
				computeCellGain(netarr, vertexarr, 'A' + fromBlkCnt, 'A' + toBlkCnt);
			}
		}
	}
	/*
	 * A pass
	 */
	for(i = 0; ; i++){
		int j;
		//printGain(netarr, vertexarr);
		tempmove = getCellWithMaxGain(netarr, vertexarr, WAY);
		gainSum[i] = (i - 1 >= 0 ? gainSum[i - 1] : 0) + vertexarr[tempmove.nodeIndex].gain[tempmove.block - 'A'] ;
		balanceValue[i] = balanceCriterion(netarr, vertexarr, TOTAL_NODES, vertexarr[tempmove.nodeIndex].block, tempmove.block);
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
		//	printf("maxMove [%d] to block %c\n", movearr[i].nodeIndex, movearr[i].block);
			updateGains(netarr, vertexarr, movearr + i);
		}
	}
	if(gainSumMax <= 0){
		printNodeList(vertexarr);
		break;
	}
	gainSum[i + 1] = -NODE_LIMIT ;
/*
	for(i = 0; gainSum[i] != -NODE_LIMIT; i++){
		printf("gainsum [%d] is %d\n", i, gainSum[i]);
	}
	printf("max gain sum [%d] %d \n",gainSumMaxIndex, gainSum[gainSumMaxIndex] );
*/

	/*
	 * Make the move permanent
	 */
	for(i = 0; i <= gainSumMaxIndex; i++){
		vertexarr[movearr[i].nodeIndex].block = movearr[i].block ;
	}
	for(i = 1; vertexarr[i].block != 0 ;i++){
		vertexarr[i].locked = 0 ;
	}
	gainSumMax  = -NODE_LIMIT ; 
	gainSumMaxIndex = -1 ;
//	printf("---------------\n");
/*
	for( i = 1; vertexarr[i].ldc != 0 ; i++){
		int j = 0 ;
		printf("Node %d(%d)[%c]: ", i, vertexarr[i].ldc, vertexarr[i].block);
		for(j = 0; j < vertexarr[i].ldc; j++){
			printf("%d(%d) ", vertexarr[i].ld[j].index, vertexarr[i].ld[j].count);
		} 
		putchar('\n');
	}
*/
//	printf("end of pass\n");
	}
	return 0 ;
}
int printNetList(const N * narr){
	int i ;
	for( i = 0; i <= TOTAL_NETS ; i++){
		if(narr[i].ldc != 0){ 
			int j = 0;
			printf("Net %d(%d): ", i, narr[i].ldc);
			for(j = 0; j < narr[i].ldc; j++){
				printf("%d(%d) ", narr[i].ld[j].index, narr[i].ld[j].count);
			} 
			putchar('\n');
		}
	}
	return 0 ;
}
/*
 * Print every net where a node is on
 * for debug
 */
int printNodeList(const V * varr){
	int i ;
	for( i = 0; i <= TOTAL_NODES ; i++){
		if(varr[i].ldc != 0){
			int j = 0 ;
			printf("Node %d(%d)[%c]: ", i, varr[i].ldc, varr[i].block);
/*
			for(j = 0; j < varr[i].ldc; j++){
				printf("%d(%d) ", varr[i].ld[j].index, varr[i].ld[j].count);
			} 
*/
			putchar('\n');
		}
	}
	return 0;
}
/*
 * Print the gain array of every node
 * just for debug
 */
int printGain(const N * narr, const V * varr){
	int i ;
	for( i = 1; varr[i].block != 0 ; i++){
		int j = 0;
		if( varr[i].locked ){continue;}
		for( j = 0 ; j < WAY ; j++){
			if( j != varr[i].block - 'A'){
				printf("   Gain of move [%d] from %c to %c: %d\n", i, varr[i].block, j + 'A', varr[i].gain[j] );
			}
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
				if(Fcount == 1 ){
					int j ;
					varr[i].gain[T - 'A'] += 1;
				}
				if(Tcount == 0 ){
					varr[i].gain[T - 'A'] -= 1;
				}
			}
		}
	}
	return 0;
}
float ratioOfBlock(char block){
	return WEIGHT[block - 'A'] / (WEIGHT_SUM + 0.0);
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
/*
 * calculating the number of free cells
 */
int freeCells(const V * varr){
	int i , count = 0;
	for(i = 1; varr[i].block != 0; i++){
		if(!varr[i].locked){
			count += 1 ;
		}
	}
	return count  ;
}
/*
 * check if a move meets the balance criterion
 */
int balanceCriterion(const N * narr, const V * varr, int V, char F, char T){
	if( 
	ratioOfBlock(F) * V -(freeCells(varr) / (TOTAL_NODES / 2 + 1) ) <= sizeOfBlock(narr, varr, F) - 1 &&
	ratioOfBlock(F) * V +(freeCells(varr) / (TOTAL_NODES / 2 + 1) ) >= sizeOfBlock(narr, varr, F) - 1&&
	ratioOfBlock(T) * V -(freeCells(varr) / (TOTAL_NODES / 2 + 1) ) <= sizeOfBlock(narr, varr, T) + 1&&
	ratioOfBlock(T) * V +(freeCells(varr) / (TOTAL_NODES / 2 + 1) ) >= sizeOfBlock(narr, varr, T) + 1){
		return NODE_LIMIT - abs(ratioOfBlock(F) * V - (sizeOfBlock(narr, varr, F) - 1)) - abs(ratioOfBlock(F) * V - (sizeOfBlock(narr, varr, T) + 1)) ;
	}else{
		return 0;
	}
}
/*
 * return a max-gain move 
 */
M getCellWithMaxGain(const N * narr, const V * varr, int way){
	int i ,j;
	int maxGain = -NET_LIMIT, maxIndex = 0;
	char maxBlock ;
	M move;
	for(i = 1; varr[i].block != 0 ; i++){
		if(varr[i].locked){continue;}
		for(j = 0;j < way ; j++){
			if( j == varr[i].block - 'A'){continue;}
			if(varr[i].gain[j] > maxGain && balanceCriterion(narr, varr, TOTAL_NODES, varr[i].block, j + 'A') ){
				maxGain = varr[i].gain[j] ;
				maxIndex = i ;
				maxBlock = j + 'A'; 
			}else{if(varr[i].gain[j] == maxGain && balanceCriterion(narr, varr, TOTAL_NODES, varr[i].block, j + 'A')){
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
/*
 * update the gains after a move 
 * and update the blkCnt on related net
 */
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
			}
		}else{if(Tcount == 1){
			for(j = 0; j< narr[netIndex].ldc; j++){
				int index = narr[netIndex].ld[j].index , k;
				if(varr[index].locked||varr[index].block != T ){continue;}
				for(k = 0; k < WAY; k++){
					if(k != T - 'A'){varr[index].gain[k] -= 1;}
				}
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
			}
		}else{if(Fcount == 1){
			for(j = 0; j< narr[netIndex].ldc; j++){
				int index = narr[netIndex].ld[j].index , k;
				if(varr[index].locked || varr[index].block != F ){continue;}
				for(k = 0; k < WAY; k++){
					if(k != F - 'A'){varr[index].gain[k] += 1;}
				}
			}
		}}
	}
}
