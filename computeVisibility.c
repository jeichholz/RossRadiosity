#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



typedef struct {

	float point1[3];
	float point2[3];
	float point3[3];
	float normal[3];
	float center[3];
	float color[3];

}patch;


void printV(float* v){
	printf("(%f %f %f)",v[0],v[1],v[2]);
}

void Normalize(float* n,float* answer){
	float size=sqrt(pow(n[0],2)+pow(n[1],2)+pow(n[2],2));
	answer[0]=n[0]/size;
	answer[1]=n[1]/size;
	answer[2]=n[2]/size;
}


void VVminus(float* a, float* b,float* answer){
	answer[0]=a[0]-b[0];
	answer[1]=a[1]-b[1];
	answer[2]=a[2]-b[2];
}
	

void vsMult(float* v, float s,float* ans){
	ans[0]=v[0]*s;
	ans[1]=v[1]*s;
	ans[2]=v[2]*s;
}

void cross (float* a, float* b, float* answer){



	answer[0]=(a[1]*b[2] - a[2]*b[1]);
	answer[1]=(a[2]*b[0] - a[0]*b[2]);
	answer[2]=(a[0]*b[1] - a[1]*b[0]);

	//answer[0]=a[1]*b[2]-b[1]*a[2];
	//answer[1]=a[0]*b[2]-b[0]*a[2];
	//answer[2]=a[0]*b[1]-b[0]*a[1];

}

void vvAdd(float *a, float* b, float* answer){
	answer[0]=a[0]+b[0];
	answer[1]=a[1]+b[1];
	answer[2]=a[2]+b[2];
}

void findNormal(patch* P){
	float norm[3];
	float twomone[3];
	float threemone[3];
	VVminus(P->point2,P->point1,twomone);
	VVminus(P->point3,P->point1,threemone);
	//printf("twomone is:");
	//printV(twomone);
	//printf("\n");
	//printf("threemone is:");
	//printV(threemone);
	//printf("\n");
	
	cross(twomone,threemone,norm);
	//printf("Cross is:");
	//printV(norm);
	//printf("\n");
	Normalize(norm,norm);
	Normalize(norm,P->normal);
	//printf("Normalized is:");
	//printV(norm);
	//printf("\n");
}

float dot(float *a,float *b){
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}


int SameSide(float *p1, float* p2, float* a, float* b){
    float cp1[3];
    float cp2[3];
    float bma[3];
	float p1ma[3];
	float p2ma[3];
	VVminus(p1,a,p1ma);
	VVminus(p2,a,p2ma);
	VVminus(b,a,bma);
	cross(bma,p1ma,cp1);
	cross(bma,p2ma,cp2);

    if (dot(cp1,cp2) >= 0){ 
        return 1;
    }
    else{ 
        return 0;
    }

}



int pointInPatch(float* p, float* a, float* b, float* c){
    if (SameSide(p,a, b,c) && SameSide(p,b, a,c)&& SameSide(p,c, a,b)) {
         return 1;
    }
    else{
        return 0;
    }

}


int readObjFile(char* filename,patch** patchList, int* finalPatchCount){
    printf("Opening %s for read.\n",filename);
    FILE* fp=fopen(filename,"r");
    if (fp==NULL){
        fprintf(stderr,"Error opening %s for reading. Abort!\n",filename);
        exit(1);
    }

    int maxLineSize=4096;
    char line[maxLineSize];

    int vertexCapacity=5;
    int vertexIncrement=7;
    int faceCapacity=1;
    int faceIncrement=3;
    float* vertexListFlat= malloc(3000*3*sizeof(float));
    float (*vertexList)[3] = (float (*)[3]) vertexListFlat;

    *patchList=malloc(faceCapacity*sizeof(patch));

    int nVertices=0;
    int nFaces=0;

    int tokensRead=0;
     
    int d1, d2, d3, d4;

    float AB[3];
    float AC[3];
    float tmpN[3];

    while (fgets(line,maxLineSize,fp)!=NULL){
        if (strlen(line)>=2 && line[0]=='v' && line[1]==' '){
                if (nVertices==vertexCapacity){
                    vertexCapacity+=vertexIncrement;
                    vertexListFlat=realloc(vertexListFlat,vertexCapacity*3*sizeof(float));
                    vertexList = (float (*)[3]) vertexListFlat;
                }
                tokensRead=sscanf(line,"v %f %f %f",&vertexList[nVertices][0],&vertexList[nVertices][1],&vertexList[nVertices][2]);
                if (tokensRead!=3){
                    fprintf(stderr,"Error reading %s:\nLine: %s\nExpected to read 3 floats, instead read %d.\n",filename,line,tokensRead);
                    exit(1);
                }
                nVertices++;
        }

        if (strlen(line)>=2 && line[0]=='f'){
            if (nFaces==faceCapacity){
                faceCapacity+=faceIncrement;
                *patchList=realloc(*patchList,faceCapacity*sizeof(patch));
            }
            tokensRead=sscanf(line,"f %d %d %d %d",&d1, &d2, &d3, &d4);
            if (tokensRead<3){
                tokensRead=sscanf(line,"f %d/%*d %d/%*d %d/%*d %d/%*d", &d1, &d2, &d3, &d4);
                if (tokensRead<3){
                    tokensRead=sscanf(line,"f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",&d1, &d2, &d3, &d4);
                    if (tokensRead<3){
                        tokensRead=sscanf(line,"f %d//%*d %d//%*d %d//%*d %d//%*d",&d1, &d2, &d3, &d4);
                    }
                }

            }

            if (tokensRead>3){
                fprintf(stderr,"Error reading line %s: too many vertices listed in this face; this reader can only handle trianglar faces.  Recommend opening in some other tool and resaving; this will often result in a purely triangular mesh.\n",line);
                exit(1);
            }

            if (tokensRead<3){
                fprintf(stderr,"Error reading line %s: couldn't find 3 vertices.",line);
                exit(1);
            }

            int i;
            for (i=0;i<3;i++){
                (*patchList)[nFaces].point1[i]=vertexList[d1-1][i];
                (*patchList)[nFaces].point2[i]=vertexList[d2-1][i];
                (*patchList)[nFaces].point3[i]=vertexList[d3-1][i];
            }
            (*patchList)[nFaces].center[0]=1.0/3*((*patchList)[nFaces].point1[0]+(*patchList)[nFaces].point2[0]+(*patchList)[nFaces].point3[0]);
            (*patchList)[nFaces].center[1]=1.0/3*((*patchList)[nFaces].point1[1]+(*patchList)[nFaces].point2[1]+(*patchList)[nFaces].point3[1]);
            (*patchList)[nFaces].center[2]=1.0/3*((*patchList)[nFaces].point1[2]+(*patchList)[nFaces].point2[2]+(*patchList)[nFaces].point3[2]);
            VVminus((*patchList)[nFaces].point2,(*patchList)[nFaces].point1,AB);
            VVminus((*patchList)[nFaces].point3,(*patchList)[nFaces].point1,AC);
            cross(AB,AC,tmpN);
            Normalize(tmpN,(*patchList)[nFaces].normal);

            nFaces++;
        }
    }

    *finalPatchCount=nFaces;
    fprintf(stderr,"Read %d vertices and %d faces.\n",nVertices,nFaces);


}







/*
//Functionality for reading a completely outdated file type. 
int readPatchFile(char* filename){
	printf("The file you want to open is named %s\n",filename);
//	FILE *fp ;
//	fp = fopen ( "joe.txt", “r” );

	FILE* fp;
	fp=fopen(filename,"r");
	int patchCount=0;
	int lineCount=0;

	float a;
	float b;
	float c;

	char buffer[1024];
	while (fgets(buffer,1024,fp)!=NULL){

		//printf("char 0 is %i\n",(int)buffer[0]);
		if ((int)buffer[0]!=10){
			lineCount++;
		//printf("buffer is %s\n",buffer);
		//patches[patchCount]=malloc(sizeof(patch));
		if (lineCount==1){
			sscanf(buffer,"%f %f %f",&(patches[patchCount].point1[0]),&(patches[patchCount].point1[1]),&(patches[patchCount].point1[2]));
		//	printf("Read %f %f %f\n",a,b,c);
		}

		if (lineCount==2){
			sscanf(buffer,"%f %f %f",&(patches[patchCount].point2[0]),&(patches[patchCount].point2[1]),&(patches[patchCount].point2[2]));
		}

		if (lineCount==3){
			sscanf(buffer,"%f %f %f",&(patches[patchCount].point3[0]),&(patches[patchCount].point3[1]),&(patches[patchCount].point3[2]));
		}		

		if (lineCount==4){
			sscanf(buffer,"%f %f %f",&(patches[patchCount].color[0]),&(patches[patchCount].color[1]),&(patches[patchCount].color[2]));
		}
		}
			
		if (lineCount==4){
			findNormal(&patches[patchCount]);
			//if (patchCount==3){
			//	printf("Computed normal to element 4 is: %f %f %f\n",patches[3].normal[0],patches[3].normal[1],patches[3].normal[2]);
			//}
			patches[patchCount].center[0]=(patches[patchCount].point1[0]+patches[patchCount].point2[0]+patches[patchCount].point3[0])/3;
			patches[patchCount].center[1]=(patches[patchCount].point1[1]+patches[patchCount].point2[1]+patches[patchCount].point3[1])/3;
			patches[patchCount].center[2]=(patches[patchCount].point1[2]+patches[patchCount].point2[2]+patches[patchCount].point3[2])/3;
		}
		if (lineCount==5){
			patchCount++;
			lineCount=0;
		}
	}
	numPatches=patchCount;
	printf("Read %i patches\n",patchCount);
	if (numPatches>MAXPATCHES){
		fprintf(stderr,"Read more than MAXPATCHES patches.  Recompile with higher MAXPATCHES. \n");
		exit(0);
	}
	return 1;
}
*/


int isVisible(int i,int j,patch* patches, int numPatches){
	float direction[3];
	float y0[3];
	VVminus(patches[i].center,patches[j].center,direction);
	Normalize(direction,direction);

	patch* patch1=&(patches[i]);
	patch* patch2=&(patches[j]);

	if(dot(patches[j].normal,direction)<=0.00001 ||dot(patches[i].normal,direction)>=-.000001){
		return 0;
	}

	y0[0]=patch2->center[0]; y0[1]=patch2->center[1]; y0[2]=patch2->center[2];
	float dum1[3];
	float maxTime=(dot(patch1->normal,patch1->point1)-dot(patch1->normal,y0))/(dot(patch1->normal,direction));   
	//printf("maxTime initially calculated to be: %f\n",maxTime);
	float N[3];
	patch* patch3;
	float t;
	float intPoint[3];
	int k;
        for (k = 0;k<numPatches;k++){
       		N[0]=patches[k].normal[0];N[1]=patches[k].normal[1]; N[2]=patches[k].normal[2];
        	patch3=&(patches[k]);
 
        	if (dot(N,direction)!=0){
            		t=(dot(N,patch3->point1)-dot(N,y0))/(dot(N,direction));
			//printf("i=%i j=%i k=%i t=%f\n",i,j,k,t);
           		if (t>.000001 && t<maxTime && k!= i && k!=j ){

                		//the ray meets the correct plane in positive time. and Less
                		//time than it takes to hit the other patch.  Check to see
                		//if it actually hits stuff or not.
                		vsMult(direction,t,intPoint);
				vvAdd(intPoint,y0,intPoint);
                		if (pointInPatch(intPoint,patch3->point1,patch3->point2,patch3->point3)){
						  //printf("The ray from patch A to patch B is occluded by patch %d\n",k);
                   			return 0;
				}
            		}
		}		                
         }
	return 1;
}




void makeVis(patch* patchList, int numPatches, int* visArray){
	int i;
    int (*visibility)[numPatches] = (int (*)[numPatches]) visArray;

#pragma omp parallel for schedule(dynamic, 10)
	for (i=0;i<numPatches;i++){
		visibility[i][i]=0;
		int j;
		for (j=i+1;j<numPatches;j++){
			if (isVisible(i,j,patchList,numPatches)){
				visibility[i][j]=1;
				visibility[j][i]=1;
			}
			else{
				visibility[i][j]=0;
				visibility[j][i]=0;
			}
		}
	}



}



void writeVis(char* outfile,int* visibilityArray, int numPatches){

    int (*visibility)[numPatches] = (int (*)[numPatches]) visibilityArray;


	FILE* fp;
	fp=fopen(outfile,"w");
	if (fp==NULL){
	  fprintf(stderr,"Error opening file %s\n",outfile);
	}
	int i; int j;
	for (i=0;i<numPatches;i++){
		for(j=0; j<numPatches;j++){
			fprintf(fp,"%d ",visibility[i][j]);
		}
		fprintf(fp,"\n");
	}	


}


int main(int argc, char** argv)
{

    patch* patchList;
    int nPatches;

    if (argc<3){
        fprintf(stderr,"Usage: computeVisiblity <input .obj file> <output .vis file>\n");
        exit(1);
    }

    readObjFile(argv[1],&patchList,&nPatches);
    int* vis=malloc(nPatches*nPatches*sizeof(int));
	makeVis(patchList,nPatches,vis);
	printf("Finished computing visibility, writing results.\n");
	writeVis(argv[2],vis,nPatches);

return 1;
}

