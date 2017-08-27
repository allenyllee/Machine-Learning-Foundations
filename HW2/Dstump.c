#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 20
#define TOTAL_RUN 5000



//
// genaral functions
//
int sign(double x){
    return x<0? -1:1;
}

//
// generate data set
//
typedef struct _v{
    double x;
    int y;
}Vector;

typedef struct _v2{
    double *x;
    int y;
}Vector2;


Vector Vzero = {.x = 0, .y = 0};

double randMToN(double M, double N)
{
    return M + ( (double)rand() / (double)RAND_MAX * (N-M) ) ;  
}

int rand_Flip_with_Percent(int A, double percent){
    return (double) rand()/ (double)(RAND_MAX + 1.) < percent/100 ? -A : A;
}

void sort_data(Vector *data, int datasize){
    Vector temp1=Vzero;

    //printf("sorting...\n");

    for(int i=0;i<datasize;i++){
        for(int j=i;j<datasize;j++){
            if(data[i].x > data[j].x){
                temp1 = data[i];
                data[i] = data[j];
                data[j]= temp1;
            }
        }
    }
}


//
// hypothesis set
//
typedef struct _hs HypSet;
typedef struct _hp HypParameter;

typedef void (*HypInit)(HypSet *);
typedef int (*HypFunc)(double , HypParameter *);
typedef double (*HypEoutFunc)(HypSet *);

typedef struct _hp{
    double _theta;
    int _s;
}HypParameter;

typedef struct _hs{
    HypSet *this;
    HypInit _init;
    int _hsize;
    int _datasize;
    double _Ein;
    double _Eout;
    int _hypIndex;
    Vector *_data;
    Vector2 *_data2;
    HypEoutFunc _hef;
    HypFunc _hf;
    HypParameter *_hp;
}HypSet;

int h_st(double x, HypParameter *hp){
    return hp->_s * sign(x - hp->_theta);
}

double h_Eout(HypSet *hs){
    return 0.5 + 0.3 * (hs->_hp[hs->_hypIndex]._s) * (abs(hs->_hp[hs->_hypIndex]._theta) - 1);
}

void init_hypset(HypSet *hs){

    //
    // allocate hypothesis memroy
    //
    hs->_hsize = 2*hs->_datasize;
    hs->_hp = (HypParameter*) malloc(sizeof(HypParameter)* hs->_hsize);
    
    //
    // init hypothesis function
    //
    hs->_hf = h_st;
    hs->_hef = h_Eout;

    //
    // enumarate all hypothesis set
    //
    hs->_hp[0]._s = 1;
    hs->_hp[0]._theta = ((-1)+(hs->_data[0].x))/2;
    hs->_hp[1]._s = -1;
    hs->_hp[1]._theta = ((-1)+(hs->_data[0].x))/2;

    for(int i=1; i< hs->_datasize ; i++){
        hs->_hp[2*i]._s = 1;
        hs->_hp[2*i]._theta = (hs->_data[i-1].x + hs->_data[i].x)/2;
        hs->_hp[2*i+1]._s = -1;
        hs->_hp[2*i+1]._theta = (hs->_data[i-1].x + hs->_data[i].x)/2;
    }

}

//
// load data set
//
void printDataRecord(Vector2 *data2, int index){
    int i=0;

    for(int i=0; i< 9; i++){
        printf("%lf ",data2[index].x[i]);
    }
    printf("%d\n",data2[index].y);

}

void fscanDataRecord(FILE *pFile, Vector2 *data2, int index){
    int i=0;
    
    for(int i=0; i< 9; i++){
        data2[index].x[i] = 0;
        fscanf(pFile, "%lf ", &(data2[index].x[i]) );
    }
    data2[index].y = 0;
    fscanf(pFile, "%d\n", &(data2[index].y) );
}

int loadDataset(HypSet *hs, char *filename){
    
        FILE *pFile=NULL;
    
        char c=0;
        int line_number=0;
        int i=0;
    
    
        // open data file
        pFile = fopen(filename,"r");
    
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

        hs->_datasize = line_number;
        //dset->size = line_number;
    
        // alloc memory
        hs->_data2 = (Vector2*) malloc(sizeof(Vector2)* hs->_datasize);
        //dset->inputX = (Vector*) malloc(sizeof(Vector)* dset->size);
        
        for(int i=0; i < hs->_datasize; i++){
            hs->_data2[i].x = (double*) malloc(sizeof(double) * 9);
        }

        /*
        if(hs->_data2 == NULL) {
            printf("out of memory\n");
            return 1;
        }
        */
        /*
        dset->outputY = (int*) malloc(sizeof(int)* dset->size);
    
        if(dset->outputY == NULL) {
            printf("out of memory\n");
            return 1;
        }
        */
    
        // get data records
        for(i=0 ; i<line_number ; i++){
            fscanDataRecord(pFile, hs->_data2, i);
        }
    
        // print data records
        printf("size of data records: %d\n", hs->_datasize);
        printf("\n");
        printf("=====first data record=====\n");
        printDataRecord(hs->_data2,0);
        printf("=====last data record=====\n");
        printDataRecord(hs->_data2,hs->_datasize-1);
        printf("\n");
    
        return 0;
}


//
// evaluate function (independent of hyppthesis set)
//
int err(int f, int g){
    return f == g ? 0:1;
}

int ErrCnt(HypSet *hs, int hypIndex){
    int i=0;
    int err_count=0;

    for(int i=0; i< DATA_SIZE; i++){
        err_count += err(hs->_data[i].y, hs->_hf(hs->_data[i].x, &(hs->_hp[hypIndex])));
    }
    return err_count;
}

void E_in(HypSet *hs){
    int i=0;
    int min_err=0, temp=0;
    int hypIndex=0;

    //
    // find min E_in
    //
    min_err = ErrCnt(hs, 0);
    hypIndex = 0;

    for(int i=1; i < hs->_hsize; i++){
        temp = ErrCnt(hs, i);
        if( temp < min_err) {
            min_err = temp;
            hypIndex = i;
        }
    }

    hs->_hypIndex = hypIndex;
    hs->_Ein = (double) min_err/ (double) hs->_datasize;

    //printf("HypSet Index %d with min err count %d\n", hypIndex, min_err);
    
    //printf("Ein = %.6f\n", hs->_Ein);
    
}

void E_out(HypSet *hs){
    hs->_Eout = hs->_hef(hs);
    //printf("Eout = %.6f\n", hs->_Eout);
}







//
// main function
//
int main(){
    int i=0;
    //int counterPos=0, counterNeg=0;
    Vector data[DATA_SIZE];
    HypSet hs;
    double avg_Ein=0, avg_Eout=0;


    srand(time(NULL));
    //randMToN(-1,1);

    for(int i=0;i<DATA_SIZE;i++){
        data[i].x = randMToN(-1,1);
        data[i].y = rand_Flip_with_Percent(sign(data[i].x), 20);
        //data[i].y < 0 ? counterNeg++ : counterPos++ ;
        printf("data[%d].x = %.4f, data[%d].y = %d\n",i ,data[i].x ,i ,data[i].y);
        //printf("rand = %.6f\n",randMToN(-1,1));
    }

    sort_data(data,DATA_SIZE);

    for(int i=0;i<DATA_SIZE;i++){
        printf("data[%d].x = %.4f, data[%d].y = %d\n",i ,data[i].x ,i ,data[i].y);
    }

    //printf("Positive = %d, Negative = %d\n", counterPos, counterNeg);


    hs.this = &hs;
    hs._data = data;
    hs._datasize = DATA_SIZE;
    hs._init = init_hypset;
    hs._init(hs.this);

    for(int i=0; i<TOTAL_RUN; i++){
        //srand(time(NULL));
    
        for(int i=0;i<DATA_SIZE;i++){
            data[i].x = randMToN(-1,1);
            data[i].y = rand_Flip_with_Percent(sign(data[i].x), 20);
            //data[i].y < 0 ? counterNeg++ : counterPos++ ;
            //printf("data[%d].x = %.4f, data[%d].y = %d\n",i ,data[i].x ,i ,data[i].y);
            //printf("rand = %.6f\n",randMToN(-1,1));
        }
    
        sort_data(data,DATA_SIZE);

        E_in(&hs);
        avg_Ein += hs._Ein;

        E_out(&hs);
        avg_Eout += hs._Eout;
    }

    avg_Ein /= TOTAL_RUN;
    avg_Eout /= TOTAL_RUN;

    printf("avg Ein = %.6f\n", avg_Ein);
    printf("avg Eout = %.6f\n", avg_Eout);


    loadDataset(&hs, "hw2_train.dat");





    return 0;
}