#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	//DECLARES FILE POINTERS
	FILE *CHGCAR_DATA = fopen("CHGCAR_DATA","r");
	FILE *NAIVE_CHGCAR_READ = fopen("NAIVE_CHGCAR","r");
	FILE *POSITIONS = fopen("POSITIONS","r");
	
	char line[256];
        int NGXF, NGYF, NGZF;
	int i,j,m,n = 0;

	//Read NGXF NGYF NGZF values from POSITIONS file
        fgets(line, sizeof(line),POSITIONS);
        sscanf(line,"%d %d %d",&NGXF,&NGYF,&NGZF);
	printf("%d %d %d\n",NGXF,NGYF,NGZF);
	//FREE POSITIONS POINTER
	fclose(POSITIONS);

	//CReate matrices to store the two charge densities to be superimposed
	double densities[NGXF*NGYF*NGZF];
	double naive_densities[NGXF*NGYF*NGZF];
	double a,b,c,d,e,aa,bb,cc,dd,ee;

printf("Atoms read in:\n");


	//WRITE BETTER VARIABLE NAMES
	i=0;
	j=0;
	while ( fgets(line, sizeof(line), CHGCAR_DATA)) {
                if( sscanf(line,"%lf %lf %lf %lf %lf",&a,&b,&c,&d,&e) == 5
		&& j < NGXF*NGYF*NGZF){
                        densities[j] = a;
                        j++;
                        densities[j] = b;
                        j++;
                        densities[j] = c;
                        j++;
                        densities[j] = d;
                        j++;
                        densities[j] = e;
                        j++;
                }
	}

        j=0;
        while ( fgets(line, sizeof(line), NAIVE_CHGCAR_READ)) {
                if(sscanf(line,"%lf %lf %lf %lf %lf",&a,&b,&c,&d,&e) == 5
		&& j < NGXF*NGYF*NGZF){
                        naive_densities[j] = a;
                        j++;
                        naive_densities[j] = b;
                        j++;
                        naive_densities[j] = c;
                        j++;
                        naive_densities[j] = d;
                        j++;
                        naive_densities[j] = e;
                        j++;
                }
        }
	fclose(CHGCAR_DATA);
	fclose(NAIVE_CHGCAR_READ);

	for(i=0;i<NGXF*NGYF*NGZF;i++){
		naive_densities[i] = naive_densities[i] + densities[i];
        }



	//Clears NAIVE_CHGCAR file
	FILE *NAIVE_CHGCAR_WRITE = fopen("NAIVE_CHGCAR","w");


	//Write data into NAIVE_CHGCAR file
	//Prints data to CHGCAR_DATA in a format acceptable for later CHGCAR use
	for(i=0;i<NGXF*NGYF*NGZF;i++){
		if( (i + 1) % 5 == 0
		&& i != 0 ){
			fprintf(NAIVE_CHGCAR_WRITE,"%lf\n",naive_densities[i]);
                }
		else{
			fprintf(NAIVE_CHGCAR_WRITE,"%lf ",naive_densities[i]);
		}
	}
	fclose(NAIVE_CHGCAR_WRITE);
}
