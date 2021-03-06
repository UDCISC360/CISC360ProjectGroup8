//Author Eric, Sam
#include <cstdlib>
#include <ctime>
#include <utility>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include<omp.h>
#include <stdlib.h>
#include <string.h>

int ITERATE = 10; //number of iterations for each test. 
int LEN_CONST [8] = {100,1000,5000,10000, 50000, 100000, 500000,1000000};
int LEN_CONST_LEN = 8;
int MAXVAL = 10000;
int NUMTHREADS = 8; 

int testArray[3] = {1, 10, 100};
int testArrLen = 3;
std::clock_t start;
std::clock_t end;
double parStart;
double parEnd; 
double durationAvg;
double duration;

void print_array(int n, int array[]) {
  int i;
  printf("[%d", array[0]);
  for (i = 1; i < n; i++) {
    printf(", %d", array[i]);
  }
  printf("]\n");
}

void insertionsort(int a[], int n, int stride) 
{//since shellsort basically uses a bunch of insertion sorts
    for (int j=stride; j<n; j+=stride) {
        int key = a[j];
        int i = j - stride;
        while (i >= 0 && a[i] > key) {
            a[i+stride] = a[i];
            i-=stride;
        }
        a[i+stride] = key;
    }
}
int newgap(int gap)
{
    gap = (gap * 10) / 13;
    if (gap == 9 || gap == 10)
        gap = 11;
    if (gap < 1)
        gap = 1;
    return gap;
}

int * generateRandomArray(int len){

  int * tbr = new int [len];
  for(int k = 0; k< len; k++){
    tbr[k] = (rand()%MAXVAL)+1;
  }
  return tbr; 
}

int * generateSortedArray(int len){
  int * tbr = new int [len];
  for (int k = 0; k < len; k++){
    tbr[k] = k; 
  }
  return tbr;
}

int * generateBackwardsArray(int len){
  int * tbr = new int[len];
  for (int k = 0; k< len; k++){
    tbr[k] = len-k;
  }
  return tbr; 
} 
  

void sortRun(int* lenArrs, int lenArrsLen,
	void (*sortFun)(int*, int), int* (*genFun)(int) ){
	
    for(int i = 0; i < lenArrsLen; i++)
	{
	  duration = 0;
	  durationAvg = 0; 
		for(int k = 0; k < ITERATE; k++)
		{
			//loop for each Len in LEN_CONST
			//create a new array, populate array
			int* tbs = genFun(lenArrs[i]); 
			// print_array(lenArrs[i],tbs); 
			start = clock(); 
			sortFun(tbs, lenArrs[i]);
			end = clock(); 
			durationAvg += ( end - start ) / (double) CLOCKS_PER_SEC;
			free(tbs);
			//print_array(lenArrs[i],tbs);
        }
		duration = durationAvg/ITERATE;
		std::cout<<"time: "<< duration<<" length: " << lenArrs[i]<<'\n';
    }
    
}

 
void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 

//Website linked above

// A function to implement bubble sort

//http://www.geeksforgeeks.org/bubble-sort/
void bubbleSort(int arr[], int n)
{
	int i, j;
	for (i = 0; i < n-1; i++)      
 
    // Last i elements are already in place   
		for (j = 0; j < n-i-1; j++) 
			if (arr[j] > arr[j+1])
				swap(&arr[j], &arr[j+1]);
}


//https://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Comb_sort
// Function to sort a[0..n-1] using Comb Sort
 
// Function to sort a[0..n-1] using Comb Sort
void combSort(int a[], int n)
{
    // Initialize gap
    int gap = n;
 
    // Initialize swapped as true to make sure that
    // loop runs
    bool swapped = true;
 
    // Keep running while gap is more than 1 and last
    // iteration caused a swap
    while (gap != 1 || swapped == true)
    {
        // Find next gap
        gap = newgap(gap);
 
        // Initialize swapped as false so that we can
        // check if swap happened or not
        swapped = false;
 
        // Compare all elements with current gap
        for (int i=0; i<n-gap; i++)
        {
            if (a[i] > a[i+gap])
            {
				int temp = a[i];
				a[i] = a[i+gap];
				a[i+gap] = temp; 
				swapped = true;
            }
        }
    }
}
//https://geeksforgeeks.org/shellsort
/* function to sort arr using shellSort */
void shellSort(int arr[], int n)
{
    // Start with a big gap, then reduce the gap
    for (int gap = n/2; gap > 0; gap /= 2)
    {
        // Do a gapped insertion sort for this gap size.
        // The first gap elements a[0..gap-1] are already in gapped order
        // keep adding one more element until the entire array is
        // gap sorted 
        for (int i = gap; i < n; i += 1)
        {
            // add a[i] to the elements that have been gap sorted
            // save a[i] in temp and make a hole at position i
            int temp = arr[i];
 
            // shift earlier gap-sorted elements up until the correct 
            // location for a[i] is found
            int j;            
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
                arr[j] = arr[j - gap];
             
            //  put temp (the original a[i]) in its correct location
            arr[j] = temp;
        }
    }
    return;
}



//PARALLEL ALGORITHMS START HERE
void bubbleSort_Par(int arr[], int n, int threads)
{
	int k,i,j,temp;
	#pragma omp parallel shared(arr, n) private(i, j, temp, k) num_threads(threads)
	for (k = 0; k <= n-2; k++)
	{
		if (k % 2 == 0){
			#pragma omp for
			for (i = 0; i <= (n/2)-1; i++)
			{
				if (arr[2 * i] > arr[2 * i + 1])
				{
					temp = arr[2 * i];
					arr[2 * i] = arr[2 * i + 1];
					arr[2 * i + 1] = temp;
				}
			}
		}
		else{
			#pragma omp for
			for (j = 0; j <= (n/2)-2; j++)
			{
				if (arr[2 * j + 1] > arr[2 * j + 2])
				{
					temp = arr[2 * j + 1];
					arr[2 * j + 1] = arr[2 * j + 2];
					arr[2 * j + 2] = temp;
				}
			}
		}
	}
}

void combSort_Par(int array[], int n, int threads)
{
    int gap;
	int temp, i, j, m, h, g, o;
    bool swapped;
	#pragma omp parallel shared(array, n, gap) private(o, temp, j, m, i, h, g, swapped) num_threads(threads)
	for(o = 0;o=1;)//basically while(true)
	{
		gap = newgap(gap);
		swapped = 0;
		for(i = 0; i < n-gap; i++)
		{
			if(i%2 == 0)
			{
				
				#pragma omp for
				for(m = 0; m <= (n-gap); m++)
				{
					g = m + gap;
					if(array[m] > array[g])
					{
						temp = array[m];
						array[m] = array[g];
						array[g] = temp;	
						swapped = 1;
					}
				}
			}
			else
			{
				#pragma omp for
				for(h = 0; h <= (n-gap); h++)
				{
					j = h + gap;
					if(array[h] > array[j])
					{
						temp = array[h];
						array[h] = array[j];
						array[j] = temp;	
						swapped = 1;
					}
				}
			}
		}
		if(gap == 1 && !swapped)
		{
			o=1;
			break;
		}
	}
}

void shellSort_Par(int a[], int n, int threads)
{
    int i, m;
    #pragma omp parallel shared(a,n) private(i, m) num_threads(threads)
    for(m = n/2; m > 0; m /= 2)
    {
        #pragma omp for
        for(i = 0; i < m; i++)
            insertionsort(&(a[i]), n-i, m);
    }
}
/*
void parSortRun(int* lenArrs, int lenArrsLen,
	void (*sortFun)(int*, int, int),
	int* (*genFun)(int) ){
	duration = 0;
	durationAvg = 0; 
    for(int i = 0; i < lenArrsLen; i++)
	{
		for(int k = 0; k < ITERATE; k++)
		{
			
			//loop for each Len in LEN_CONST
			//create a new array, populate array
			int* tbs = genFun(lenArrs[i]); 
			// print_array(lenArrs[i],tbs); 
			start = clock(); 
			sortFun(tbs, lenArrs[i], NUMTHREADS);
			end = clock(); 
			durationAvg += ( end - start ) / (double) CLOCKS_PER_SEC;
			free(tbs);
			//print_array(lenArrs[i],tbs);
       }
       duration = durationAvg/ITERATE;
       std::cout<<"time: "<< duration<<" length: " << lenArrs[i]<<'\n';
    }
    
}
*/

void test_correctness(int n, int array[]) {
  int i;
  for (i = 1; i < n; i++) {
    if (array[i] < array[i - 1]) {
      printf("Correctness test found error at %d: %.4f is not < %.4f but appears before it\n", i, array[i - 1], array[i]);
    }
  }
}

void parSortRun(int* lenArrs, int lenArrsLen, void (*sortFun)(int*, int, int), int* (*genFun)(int) )
{
	int* tbs;
	int a, b;
	 
	for(a = 0; a < lenArrsLen; )
	 
	{
	  duration = 0;
	  durationAvg = 0; 
		for(b = 1; b <= ITERATE; )
		{
			tbs = genFun(lenArrs[a]);  
			parStart = omp_get_wtime(); 
			sortFun(tbs, lenArrs[a], NUMTHREADS);
			parEnd = omp_get_wtime(); 
			durationAvg += ( parEnd - parStart );
			
			b++;
		}
		duration = durationAvg/ITERATE;
		printf("time:  %F  length:  + %d + \n", duration, lenArrs[a]);
		test_correctness(lenArrs[a], tbs); 
		a++;
    }
	free(tbs);
    
}


void init_random_vector(int n, int array[]) {
  int i;
  for (i = 0; i < n; i++) {
    array[i] = (rand() % 100);
  }
}
 
int main(){

 
  	printf("Running BubbleSort Tests\n");
	printf("BubbleSort Random.\n"); 
	sortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort, generateRandomArray);
	printf("BubbleSort Sorted\n");
	sortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort, generateSortedArray);
	printf(" BubbleSort Backwards\n"); 
	sortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort, generateBackwardsArray);
	printf("Running ShellSort Tests\n");
	printf("ShellSort Random.\n"); 
	sortRun(LEN_CONST, LEN_CONST_LEN, shellSort, generateRandomArray);
	printf("ShellSort Sorted:\n");
	sortRun(LEN_CONST, LEN_CONST_LEN, shellSort, generateSortedArray);
	printf("ShellSort Backwards:\n");
	sortRun(LEN_CONST, LEN_CONST_LEN, shellSort, generateBackwardsArray);
	
	
	printf("Running Combsort Tests\n");
	printf(" CombSort Random:\n"); 
	sortRun(LEN_CONST, LEN_CONST_LEN, combSort, generateRandomArray);
	printf("CombSort Sorted \n");
	sortRun(LEN_CONST, LEN_CONST_LEN, combSort, generateSortedArray);
	printf("CombSort Backwards");
	sortRun(LEN_CONST, LEN_CONST_LEN, combSort, generateBackwardsArray);
 


  
  	printf("Running Parallel BubbleSort Tests\n");
	printf("Parallel BubbleSort Random.\n"); 
	parSortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort_Par, generateRandomArray);
	printf("ParallelBubbleSort Sorted\n");
	parSortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort_Par, generateSortedArray);
	printf("Parallel BubbleSort Backwards\n"); 
	parSortRun(LEN_CONST, LEN_CONST_LEN, bubbleSort_Par, generateBackwardsArray);
	printf("Running Parallel ShellSort Tests\n");
	printf("Parallel ShellSort Random.\n"); 
       	parSortRun(LEN_CONST, LEN_CONST_LEN, shellSort_Par, generateRandomArray);
	printf("Parallel ShellSort Sorted:\n");
	parSortRun(LEN_CONST, LEN_CONST_LEN, shellSort_Par, generateSortedArray);
	printf("Parallel ShellSort Backwards:\n");
	parSortRun(LEN_CONST, LEN_CONST_LEN, shellSort_Par, generateBackwardsArray);
	
	//note - CombSort was taken out due to bugs. Run CombSortOnly if you want to 
	//see performance. 
	/*
	THERE IS A BUG WHERE IT DOES NOT RUN ONE COMBSORT AFTER ANOTHER AND WORKS WHEN
	USING CIN.GET() AND HAVING USER HIT ENTER
	Side Note: CombSort will run worse because of extra loops for parallelization?
	
	printf("ParallelRunning Combsort Tests\n");
	printf("Parallel CombSort Random:\n"); 
	parSortRun(LEN_CONST, LEN_CONST_LEN, combSort_Par, generateRandomArray);
	printf("Parallel CombSort Sorted (HIT ENTER):\n");
	std::cin.get();
	parSortRun(LEN_CONST, LEN_CONST_LEN, combSort_Par, generateSortedArray);
	printf("Parallel CombSort Backwards (HIT ENTER): \n");
	std::cin.get();
	parSortRun(LEN_CONST, LEN_CONST_LEN, combSort_Par, generateBackwardsArray);
	*/
	return 0; 
}
