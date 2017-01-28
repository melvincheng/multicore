#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000;
double step;
const int THREAD = 2;

int main() {
	double pi;
	double array[THREAD];
	double sum;

	//change in x (i.e. width of rectangle)
	step = 1.0/(double)num_steps;

	omp_set_num_threads(THREAD);
	#pragma omp parallel
	{
		int thread_count = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		double x;
		//calculate the summation of F(x)
		// (i.e. sum of rectangles)
		//in the approximation of pi
		
		for (int i=(double)thread_id/(double)thread_count * num_steps; i < ((double)thread_id + 1)/(double)thread_count * num_steps; i++) {
			//calculate height
			x = (i+0.5)*step;
			array[thread_id] = array[thread_id] + 4/(1.0+x*x); //sum F(x)
		}
	}
	for(int i = 0; i < THREAD; i++){
		sum += array[i];	
	}
	pi = step * sum;
	printf("pi = %f\n", pi);
}