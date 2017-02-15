/*	1 - 0.030
	2 - 0.031
	4 - 0.035
	The times have minimal difference because only a maximum of two threads
	can be doing work at the sametime as the critical regions prevent more than 
	1 thread from working on the queues
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define NUM_THREADS 4

double dequeue(double *a);
int enqueue(double *a, double val);



int main() {
	double lat[998];
	double lon[998];
	double lat_queue[10] = {};
	double lon_queue[10] = {};
	double lon_temp, lat_temp;

	FILE *fp_lat;
	FILE *fp_lon;

	fp_lat = fopen("latitude.csv","r");
	fp_lon = fopen("longitude.csv","r");

	for(int i = 0; i < 998; i++){
		fscanf(fp_lat, "%lf", &lat_temp);
		fscanf(fp_lon, "%lf", &lon_temp);
		lat[i] = lat_temp;
		lon[i] = lon_temp;
	}

	omp_set_num_threads(NUM_THREADS);

	#pragma omp parallel
	{
		double lon_val, lat_val;

		#pragma omp for schedule(dynamic)
		for(int i = 0; i < 998; i++){
			#pragma omp critical (lat)
			{
				// printf("%s\n", "lat");
				if(enqueue(lat_queue, lat[i]) == -1){
					lat_val = dequeue(lat_queue);
					enqueue(lat_queue, lat[i]);
				}
				printf("%lf %d\xc2\xb0 %d\' %d\"\n", lat_val, (int)lat_val, (int)(lat_val*60.0)%60, (int)(fabs(lat_val)*3600.0)%60);
			}
			#pragma omp critical (lon)
			{
				// printf("%s\n", "lon");
				if(enqueue(lon_queue, lon[i]) == -1){
					lon_val = dequeue(lon_queue);
					enqueue(lon_queue, lon[i]);
				}
				printf("%lf %d\xc2\xb0 %d\' %d\"\n", lon_val, (int)lon_val, (int)(lon_val*60.0)%60, (int)(fabs(lon_val)*3600.0)%60);
			}
		}
		#pragma omp for schedule(dynamic)
		for(int i = 0; i < 10; i++){
			#pragma omp critical (lat)
			{
				lat_val = dequeue(lat_queue);
			}
			if((int)lat_val != -1){
				printf("%lf %d\xc2\xb0 %d\' %d\"\n", lat_val, (int)lat_val, (int)(lat_val*60.0)%60, (int)(fabs(lat_val)*3600.0)%60);
			}
			#pragma omp critical (lon)
			{
				lon_val = dequeue(lon_queue);
			}
			if((int)lon_val != -1){
				printf("%lf %d\xc2\xb0 %d\' %d\"\n", lon_val, (int)lon_val, (int)(lon_val*60.0)%60, (int)(fabs(lon_val)*3600.0)%60);
			}
		}
	}
	fclose(fp_lat);
	fclose(fp_lon);
	return 0;

}

int enqueue(double *a, double val) {
	int i;
	for (i=0; i< 10; i++) {
		if (a[i] == 0.0) {
			a[i] = val;
			return i;
		}
	}
	return -1;
}

double dequeue(double *a) {
	int i;
	double val = a[0];
	if(a[0] == 0.0){
		return -1.0;
	}
	for (i=0; i < 9; i++) {
		a[i] = a[i+1];
	}
	a[9] = 0.0;
	return val;
}
