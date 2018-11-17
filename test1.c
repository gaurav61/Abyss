#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


// map gnome seq to graph
// read k-mer from file


/*
    mapping
    A -> 0;
    T -> 1;
    G -> 2;
    C -> 3;

*/

// we are representing kmer as as a pair of 4 unsigned long
// at max there can be 128 base pairs in kmer 
typedef struct Pair_{      
    unsigned long a;
    unsigned long b;
    unsigned long c;
    unsigned long d;
} Pair;



Pair genPair(char* kmer){
	Pair ans;
	ans.a =0;ans.b=0;
	ans.c=0;ans.d=0;
	
	unsigned long e;
	int count = 0,i=0;
	int index = strlen(kmer) -1;
	printf("%d\n",index);
	unsigned long shiftop = 1; 

	//converting rightmost part of kmer into long and storing in d
	for(i=0;i<4 && index>=0 ;i++){
		shiftop =1;
		count=0;
		e=0;
		for(;count<32 && index>=0;index--){
			switch(kmer[index]){
				case 'A': e+=shiftop*0;    
						break;
				case 'T': e+=shiftop*1;
						break;
				case 'G': e+=shiftop*2;
						break;
				case 'C': e+=shiftop*3;
						break;
			}
			shiftop=shiftop<<2;     //multiply by 4
			//  printf("%lu \n",shiftop);
			count++;
		}

		switch(i){
			case 0: ans.d = e;break;
			case 1: ans.c = e;break;
			case 2: ans.b = e;break;
			case 3: ans.a = e;break;
		}
	}

	return ans;
}



void buildGraph(unsigned long* Node, unsigned int* Edge, unsigned int N,int SA,int SB,int SC,int SD){
    //unsigned int idx = blockDim.x * blockIdx.x + threadIdx.x;
   
    for(unsigned int idx=0;idx<N;idx++){
        unsigned int k = 4*idx;
        unsigned long a = Node[k]; 
        unsigned long b = Node[k+1];
        unsigned long c = Node[k+2];
        unsigned long d = Node[k+3];

        unsigned int e = 8*idx;

        unsigned long end = 3;
        unsigned long start = end<<62;

        //at begining
        for(unsigned long i=0;i<4;i++){
            //add last base of C to first of D
            //atleast size of 1 base 2 bits
            unsigned long a1,b1,c1,d1;
            a1=b1=c1=d1=0;

            if(SD>1 && SC>1)
                d1 = (d>>2) | ( (c & end)<<(SD-2));
            if(SC>1 && SB>1)
                c1= (c>>2) | ( (b & end)<<(SC-2));
            if(SB>1 && SA>1)
                b1 = (b>>2) | ( (a & end)<<(SB-2));
            
            //Add here diff bases
            if(SA>1)
                a1 = (a>>2) | (i<<(SA-2));
            else if(SB>1)
                b1 = (b>>2) | (i<<(SB-2));
            else if(SC>1)
                c1 = (c>>2) | (i<<(SC-2));
            else if(SD>1)
                d1 = (d>>2) | (i<<(SD-2));

	            //search for a1,b1,c1,d1
            unsigned int m,l,r;
            l=0;
            r=N;
		    while ( l <= r) 
		    { 
		        m = l + (r-l)/2; 
		  
		        // Check if x is present at mid 
		        if (Node[4*m] == a1  && Node[4*m+1] == b1  && Node[4*m+2] == c1  && Node[4*m+3] == d1 ){
		        	Edge[8*idx+i]=m; 
		        }
		  
		        // all are less go to left
		        if (Node[4*m] > a1 && Node[4*m+1] > b1 && Node[4*m+2] > c1 && Node[4*m+3] > d1){
		        				r = m-1; 
		        }
		  
		        // any one is greater
		        else{
		            l = m + 1; 
		        }
		    } 

        }

        //at end
        for(unsigned long i=0;i<4;i++){
            unsigned long a1,b1,c1,d1;
            a1=b1=c1=d1=0;
            unsigned long lim = 1;
            
            if(SA>1)
                a1 = (a<<2) | ( (b & start)>>62 );
            if(SB>1)
                b1 = (b<<2) | ( (c & start)>>62 );
            if(SC>1)
                c1 = (c<<2) | ( (d & start)>>62 );
            if(SD>1)
                d1 = (d<<2)| i ;
            
            //removing additional bits if not full limit
            if(SA>1 && SA<64)
                a1 = a1 & ((lim<<SA)-1);
            if(SA>1 && SA<64)
                b1 = b1 & ((lim<<SB)-1);
            if(SA>1 && SA<64)
                c1 = c1 & ((lim<<SC)-1);
            if(SA>1 && SA<64)
                d1 = d1 & ((lim<<SD)-1);

            //search now
            unsigned int m,l,r;
            l=0;
            r=N;
		    while ( l <= r) 
		    { 
		        m = l + (r-l)/2; 
		  
		        // Check if x is present at mid 
		        if (Node[4*m] == a1  && Node[4*m+1] == b1  && Node[4*m+2] == c1  && Node[4*m+3] == d1 ){
		        	Edge[8*idx+4+i]=m; 
		        }
		  
		        // all are less go to left
		        if (Node[4*m] > a1 && Node[4*m+1] > b1 && Node[4*m+2] > c1 && Node[4*m+3] > d1){
		        				r = m-1; 
		        }
		  
		        // any one is greater
		        else{
		            l = m + 1; 
		        }
		    }
            
        }
    } 
}


int main( ){
	
	char kmer[128];



	FILE* fp = fopen("inp","r");

	if(fp==NULL){
		printf("couldn't open file inp ");
		return 1;
	}

	while( fscanf(fp,"%s\n",kmer) != EOF){
		genPair(kmer);
		printf(" %s: a = %lu\t b = %lu\t c = %lu\t d = %lu\n" ,kmer,ans.a,ans.b,ans.c,ans.d);
	}

	fclose(fp);

	return 0;
}