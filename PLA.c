
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//typedef struct v Vector;

typedef struct v{
    double X0;
    double X1;
    double X2;
    double X3;
    double X4;
}Vector;

//typedef struct d Dataset;

typedef struct d{
    Vector *inputX;
    int *outputY;
    int size;
}Dataset;

Vector Vzero = {.X0 = 0, .X1 = 0, .X2 = 0, .X3 = 0, .X4 = 0};
Vector Vtest1 = {.X0 = 0, .X1 = 1, .X2 = 2, .X3 = 3, .X4 = 4};
Vector Vtest2 = {.X0 = 0, .X1 = -1.5, .X2 = -2.5, .X3 = -3.5, .X4 = -4.5};

double innerProduct(Vector *v1, Vector *v2){
    return v1->X0*v2->X0 + v1->X1*v2->X1 + v1->X2*v2->X2 + v1->X3*v2->X3 + v1->X4*v2->X4;
}

Vector vectorMultiplyConstant(Vector *v1, double Constant){
    Vector v3;
    v3.X0 = v1->X0 * Constant;
    v3.X1 = v1->X1 * Constant;
    v3.X2 = v1->X2 * Constant;
    v3.X3 = v1->X3 * Constant;
    v3.X4 = v1->X4 * Constant;
    return v3;
}

Vector vectorAdd(Vector *v1, Vector *v2){
    Vector v3;
    v3.X0 = v1->X0 + v2->X0;
    v3.X1 = v1->X1 + v2->X1;
    v3.X2 = v1->X2 + v2->X2;
    v3.X3 = v1->X3 + v2->X3;
    v3.X4 = v1->X4 + v2->X4;
    return v3;
}

void printVector(Vector *v1){
    printf("%lf %lf %lf %lf %lf\n",v1->X0,
                                   v1->X1, 
                                   v1->X2, 
                                   v1->X3, 
                                   v1->X4
                                   );
}

void printDataRecord(Dataset *dset, int index){
    printf("%lf %lf %lf %lf %lf %d\n",dset->inputX[index].X0,
                                  dset->inputX[index].X1, 
                                  dset->inputX[index].X2, 
                                  dset->inputX[index].X3, 
                                  dset->inputX[index].X4, 
                                  dset->outputY[index]
                                  );
}

void fscanDataRecord(FILE *pFile, Dataset *dset, int index){
    dset->inputX[index].X0 = 1;
    fscanf(pFile, "%lf %lf %lf %lf %d\n", &(dset->inputX[index].X1), 
                                          &(dset->inputX[index].X2), 
                                          &(dset->inputX[index].X3), 
                                          &(dset->inputX[index].X4), 
                                          &(dset->outputY[index])
                                          );
}

int loadDataset(Dataset *dset){

    FILE *pFile=NULL;

    char c=0;
    int line_number=0;
    int i=0;


    // open data file
    pFile = fopen("hw1_15_train.dat","r");

    if (pFile == NULL) {
        printf("Open file error");
        return 1;
    }

    // count number of data records
    do{
        if(c == '\n') line_number++;
        c = fgetc(pFile);
    }while(c != EOF);
    rewind (pFile);
    dset->size = line_number;

    // alloc memory
    dset->inputX = (Vector*) malloc(sizeof(Vector)* dset->size);
    
    if(dset->inputX == NULL) {
        printf("out of memory\n");
        return 1;
    }

    dset->outputY = (int*) malloc(sizeof(int)* dset->size);

    if(dset->outputY == NULL) {
        printf("out of memory\n");
        return 1;
    }


    // get data records
    for(i=0 ; i<line_number ; i++){
        fscanDataRecord(pFile,dset,i);
    }

    // print data records
    printf("size of data records: %d\n", dset->size);
    printf("\n");
    printf("=====first data record=====\n");
    printDataRecord(dset,0);
    printf("=====last data record=====\n");
    printDataRecord(dset,dset->size-1);
    printf("\n");

    return 0;
}


int PLA(Dataset *dset, int *randIndex, double eta){
    Vector W = Vzero;
    Vector Vtemp = Vzero;
    double dtemp = 0;
    int i = 0; 
    int noMistakeCount = 0;
    int updateCount = 0;

    do{
        /*
        printf("==== loop %d=====\n",i);
        printDataRecord(dset,i);
        printf("==== loop %d=====\n",i);
        */

        dtemp = innerProduct(&W, &(dset->inputX[randIndex[i]]));

        //printf("inner product dtemp %lf\n", dtemp);

        if (dtemp == 0) dtemp = -1;
        dtemp = dtemp * dset->outputY[randIndex[i]];

        //printf("sign dtemp %lf\n",dtemp);

        if(dtemp < 0){
            /*
            printf("==== loop %d=====\n",i);
            printDataRecord(dset,randIndex[i]);
            printf("==== loop %d=====\n",i);
            printf("sign dtemp %lf\n",dtemp);

            printf("==== before update =====\n");
            printVector(&Vtemp);
            printVector(&W);
            */
            
            Vtemp = vectorMultiplyConstant( &(dset->inputX[randIndex[i]]), (double) eta * dset->outputY[randIndex[i]] );
            W = vectorAdd( &W, &Vtemp );
            /*
            printf("==== after update =====\n");
            printVector(&Vtemp);
            printVector(&W);
            
            printf("\n");
            */
            
            updateCount++;
            noMistakeCount = 0;
        }else{
            noMistakeCount++;
        }

        i++;

        if(i >= dset->size) {
            i %= dset->size;
        }
        //printf("update count: %d\n", updateCount);
        //printf("no mistake count: %d\n", noMistakeCount);
        //printf("\n");
        
        //if(updateCount == 100) break;
    }while(noMistakeCount < dset->size);

    return updateCount;
}


int PocketPLA(Dataset *dset){
    Vector W = Vzero;
    Vector Vtemp = Vzero;
    double dtemp = 0;
    int i = 0; 
    int noMistakeCount = 0;
    int updateCount = 0;
    int temp=0;


    srand(time(NULL));

    temp = rand() % dset.size;

    do{
        /*
        printf("==== loop %d=====\n",i);
        printDataRecord(dset,i);
        printf("==== loop %d=====\n",i);
        */

        dtemp = innerProduct(&W, &(dset->inputX[temp]));

        //printf("inner product dtemp %lf\n", dtemp);

        if (dtemp == 0) dtemp = -1;
        dtemp = dtemp * dset->outputY[temp];

        //printf("sign dtemp %lf\n",dtemp);

        if(dtemp < 0){
            /*
            printf("==== loop %d=====\n",i);
            printDataRecord(dset,temp);
            printf("==== loop %d=====\n",i);
            printf("sign dtemp %lf\n",dtemp);

            printf("==== before update =====\n");
            printVector(&Vtemp);
            printVector(&W);
            */
            
            Vtemp = vectorMultiplyConstant( &(dset->inputX[temp]), (double) dset->outputY[temp] );
            W = vectorAdd( &W, &Vtemp );
            /*
            printf("==== after update =====\n");
            printVector(&Vtemp);
            printVector(&W);
            
            printf("\n");
            */
            
            updateCount++;
            noMistakeCount = 0;
        }else{
            noMistakeCount++;
        }

        i++;

        if(i >= dset->size) {
            i %= dset->size;
        }
        //printf("update count: %d\n", updateCount);
        //printf("no mistake count: %d\n", noMistakeCount);
        //printf("\n");
        
        if(updateCount == 50) break;
    }while(noMistakeCount < dset->size);

    return updateCount;
}



void normalCycle(int *indexArray, int size){
    int i=0;
    for(i=0;i<size;i++){
        indexArray[i] = i;
        //printf("i = %d, indexArray[i] = %d\n", i, indexArray[i]);
    }
}

void randomCycle(int *indexArray, int size){
    int temp=0;
    int i=0, j=0;

    srand(time(NULL));
    
    //printf("get random\n");

    for(i=0;i<size;i++){
        temp = rand() % 400;
        
        for(j=0;j<i;j++){
            if(indexArray[j] == temp) {
                //printf("temp = %d\n",temp);
                break;
            }
        }
        if(j==i) {
            indexArray[i] = temp;
            //printf("i = %d, indexArray[i] = %d\n", i, indexArray[i]);
        }else{
            i--;
        }
    }

    /*
    for(i=0;i<size;i++){
        printf("%d\n",indexArray[i]);
    }
    */


}


int main(){

    Dataset dset1;
    int updates = 0;
    int randIndex[400]={0};
    int i=0;

    loadDataset(&dset1);
    //updates = PLA(&dset1);

    normalCycle(randIndex, dset1.size);
    updates = PLA(&dset1, randIndex, 1);
    printf("update count is : %d\n", updates);

    for(i=0;i<2000;i++){
        randomCycle(randIndex, dset1.size);
        updates += PLA(&dset1, randIndex, 0.5);
        //printf("update count is : %d\n", updates);
    }
    //printf("test inner product %lf\n", innerProduct(&Vtest1, &Vtest2));
    printf("total update count is : %d\n", updates);
    updates /= 2000;
    printf("average update count is : %d\n", updates);

    return 0;
}
