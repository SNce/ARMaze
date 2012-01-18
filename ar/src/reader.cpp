#include"reader.h"

//void loadMaze(const char *file,float **vArray,float **normal1,float **normal2)
void loadMaze(const char *file,float **vArray,int nWalls)
{
	int temp;
	int nValues;
	int i;
	int count1 = 0;
	int count2 = 0;

	FILE *fp;
	fp = fopen(file,"r");
	
	fscanf(fp,"%d",&temp);
	nValues = 4 * nWalls;

	for(i = 0; i < nValues;i++)
	{
		fscanf(fp,"%f",&vArray[i][0]);
		fscanf(fp,"%f",&vArray[i][1]);
		fscanf(fp,"%f",&vArray[i][2]);
	}

#ifdef __DEBUG
	for(i = 0;i < (nWalls * 4);i++)
	{
		for(int j = 0;j < 3;j++)
		{
			printf("%f ",vArray[i][j]);
		}
		printf("\n");
	}
#endif

	fclose(fp);
}

int getNumWalls(const char* file)
{
	FILE *fp;
	fp = fopen(file,"r");

	//char* buf;
	int size;

	//fscanf(fp,"%s",buf);
	fscanf(fp,"%d",&size);

	return(size);
}
	