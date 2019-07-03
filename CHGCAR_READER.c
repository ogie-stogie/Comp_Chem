#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	//DECLARE POINTERS TO FRONT OF FILES
	FILE *CHGCAR = fopen("CHGCAR", "r");
	FILE *CHGCAR_DATA = fopen("CHGCAR_DATA","w");
	FILE *POSITIONS = fopen("POSITIONS","r");
	
	char line[256];
	int NGXF, NGYF, NGZF ;
	double double_delta_x, double_delta_y, double_delta_z;
	int delta_x,delta_y,delta_z;

	//Read NGXF NGYF NGZF values from POSITIONS file
	fgets(line, sizeof(line),POSITIONS);
        sscanf(line,"%d %d %d",&NGXF,&NGYF,&NGZF);	

	//Get positions of ions and translate accordingly
	fgets(line, sizeof(line),POSITIONS);
        sscanf(line,"%lf %lf %lf",&double_delta_x,&double_delta_y,&double_delta_z);
	
	//FREE POINTER FOR POSITIONS FILE
	fclose(POSITIONS);

	//Round parameters to integer equivalents
	double_delta_x = double_delta_x - 0.5;
	double_delta_x = double_delta_x * NGXF;
	double_delta_y = double_delta_y - 0.5;
        double_delta_y = double_delta_y * NGYF;
	double_delta_z = double_delta_z - 0.5;
        double_delta_z = double_delta_z * NGZF;

	delta_x = (int)round(double_delta_x);
	delta_y = (int)round(double_delta_y);
	delta_z = (int)round(double_delta_z);
	
	
	//NEED BETTER VARIABLE NAMES
	double a, b, c, d, e;

        int i = 0, j = 0, m, n = 0;

	printf("C-CODE\n");

	//FINENESS OF FFT GRID IS STORED
	printf("%d %d %d\n",NGXF,NGYF,NGZF);

	//densities AND densityMatrix ARRAYS ARE INITIALIZED
	double densities[NGXF*NGYF*NGZF];
	double densityMatrix[NGXF][NGYF][NGZF];
	
	//NORMALIZE THE delta_x VARIABLE
        if(delta_x > 0){
                while(delta_x > NGXF){
			delta_x = delta_x - NGXF;
		}
        }
	else if(delta_x < 0){
		while(delta_x < -1*NGXF){
			delta_x = delta_x + NGXF;
		}
	}
	
	//NORMALIZE THE delta_y VARIABLE
        if(delta_y > 0){
                while(delta_y > NGYF){
                        delta_y = delta_y - NGYF;
                }
        }
        else if(delta_y < 0){
                while(delta_y < -1*NGYF){
                        delta_y = delta_y + NGYF;
                }
        }

	//Z variable
        if(delta_z > 0){
                while(delta_z > NGZF){
                        delta_z = delta_z - NGZF;
                }
        }
        else if(delta_z < 0){
                while(delta_z < -1*NGZF){
                        delta_z = delta_z + NGZF;
                }
        }

	printf("PARAMETERS: %d %d %d\n",delta_x,delta_y,delta_z);

	//DECLARES densities ARRAY: 1D ARRAY OF CHG FILE DENSITIES
	while (fgets(line, sizeof(line), CHGCAR)) {
		
		/*SKIPS FIRST 11 LINES OF CHGCAR
		 *LIMITS ELEMENT SIZE TO TOTAL DIVISIONS
		 *ONLY READS IN 5 ELEMENTS AT A TIME
		 */
		if(i > 11
		&& j < NGXF*NGYF*NGZF
		&& sscanf(line,"%lf %lf %lf %lf %lf",&a,&b,&c,&d,&e) == 5){
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
		i++;
	}

	//DECLARES densityMatrix FROM densities ARRAY
	//THIS IS UNNECESSARY WASTE OF COMPUTER SPACE
	n = 0;
	for(m=0;m<NGZF;m++){
		for(j=0;j<NGYF;j++){
			for(i=0;i<NGXF;i++){
				densityMatrix[i][j][m] = densities[n];
				n++;
			}
		}

	}

	printf("FIRST:%lf LAST:%lf\n",densityMatrix[0][0][0],densityMatrix[NGXF-1][NGYF-1][NGZF-1]);

	//INITIALIZE tempMatrix 
	double tempMatrix[NGXF][NGYF][NGZF];

	//delta_x translates the elements in the +x direction
        for(m=0;m<NGZF;m++){
                for(j=0;j<NGYF;j++){
                        for(i=0;i<NGXF;i++){
				if(i+delta_x >= NGXF){
					tempMatrix[i+delta_x-NGXF][j][m] = densityMatrix[i][j][m];
				}
				else if(i+delta_x < 0){
					tempMatrix[i+delta_x+NGXF][j][m] = densityMatrix[i][j][m];
				}
				else{
					tempMatrix[i+delta_x][j][m] = densityMatrix[i][j][m];
				}
                        }
                }

        }
	
	//Change old matrix to new matrix with x-shift
	for(m=0;m<NGZF;m++){
		for(j=0;j<NGYF;j++){
        		for(i=0;i<NGXF;i++){
				densityMatrix[i][j][m] = tempMatrix[i][j][m];
       			}
		}
	}

	//delta_y translates the elements in the +y direction
       	for(m=0;m<NGZF;m++){
               	for(j=0;j<NGYF;j++){
                       	for(i=0;i<NGXF;i++){
                               	if(j+delta_y >= NGYF){
                                       	tempMatrix[i][j+delta_y-NGYF][m] = densityMatrix[i][j][m];
                               	}
				else if(j+delta_y < 0){
                                       	tempMatrix[i][j+delta_y+NGYF][m] = densityMatrix[i][j][m];
                               	}
                               	else{
                                       	tempMatrix[i][j+delta_y][m] = densityMatrix[i][j][m];
                               	}
                       	}
               	}
       }

	//Change old matrix to new matrix with y-shift
	for(m=0;m<NGZF;m++){
        	for(j=0;j<NGYF;j++){
                	for(i=0;i<NGXF;i++){
                        	densityMatrix[i][j][m] = tempMatrix[i][j][m];
                	}
        	}
	}

	//delta_z translates the elements in the +z direction
        for(m=0;m<NGZF;m++){
                for(j=0;j<NGYF;j++){
                        for(i=0;i<NGXF;i++){
                                if(m+delta_z >= NGZF){
                                        tempMatrix[i][j][m+delta_z-NGZF] = densityMatrix[i][j][m];
                                }
				else if(m+delta_z < 0){
                                        tempMatrix[i][j][m+delta_z+NGZF] = densityMatrix[i][j][m];
                                }
                                else{
                                        tempMatrix[i][j][m+delta_z] = densityMatrix[i][j][m];
                                }
                        }
                }

        }
	
	//Change old matrix to new matrix with z-shift
	for(m=0;m<NGZF;m++){
        	for(j=0;j<NGYF;j++){
                	for(i=0;i<NGXF;i++){
                        	densityMatrix[i][j][m] = tempMatrix[i][j][m];
                	}
        	}
	}

	//Prints data to CHGCAR_DATA in a format acceptable for use in looking at individual densities
	for(m=0;m<NGZF;m++){
                for(j=0;j<NGYF;j++){
                        for(i=0;i<NGXF;){
				if( ( i + 1 ) % 5 == 0
				&& i != 0){
					fprintf(CHGCAR_DATA,"%lf\n",densityMatrix[i][j][m]);
				}
				else{
					fprintf(CHGCAR_DATA,"%lf ",densityMatrix[i][j][m]);
				}
				i++;
                        }
                }

        }

	//FREE REMAINING FILE POINTERS
	fclose(CHGCAR_DATA);
	fclose(CHGCAR);
	
	//RETURN VARIABLE TO DEFINE SUCCESSFUL RUN
	return 1;
}
