/*	1 - 0.002
 	2 - 0.002
 	4 - 0.006
 	8 - 0.008
 	*/
#include <stdio.h>
#include <omp.h>
#include <string.h>

#define NUM_THREADS 4

int main(){
	omp_set_num_threads(NUM_THREADS);

	char top_first[8][255];
	char top_last[8][255];
	float top_gpa[8];
	#pragma omp parallel
	{
		#pragma omp for
		for(int i = 1; i < 9; i++){
			FILE *fp;
			char file[5];
			char first[255];
			char last[255];
			char buff[255];
			float gpa;
			sprintf(file, "%d.csv", i);

			printf("%s\n", file);

			fp = fopen(file,"r");
			while(fscanf(fp, "%[^,],%[^,],%f[^\n]", first, last, &gpa) > 0){
				// printf("%s %s %f\n", first, last, gpa);
				#pragma omp critical
				{
					if(gpa > top_gpa[i - 1]){
						top_gpa[i - 1] = gpa;
						strcpy(top_last[i - 1],last);
						strcpy(top_first[i - 1],first);
					}
				}
			};
			fclose(fp);
		}
		#pragma omp barrier
		{
			
			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 8; j++){
					#pragma omp critical
					{
						if(top_gpa[i] > top_gpa[j]){
							
							float temp_gpa = top_gpa[i];
							top_gpa[i] = top_gpa[j];
							top_gpa[j] = temp_gpa;
							char temp_first[255];
							strcpy(temp_first, top_first[i]);
							strcpy(top_first[i], top_first[j]);
							strcpy(top_first[j], temp_first);
							char temp_last[255];
							strcpy(temp_last, top_last[i]);
							strcpy(top_last[i], top_last[j]);
							strcpy(top_last[j], temp_last);
						}
					}

				}
			}
		}
	}
	for(int i = 0; i < 8; i++){
		printf("%s %s %f\n", top_first[i], top_last[i], top_gpa[i]);
	}
}