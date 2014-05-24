
#include "TComAnalytics.h"
#include "TComDataCU.h"

using namespace std;

PartSize TComAnalytics::lastPUMode;
TComDataCU* TComAnalytics::currCU;

int TComAnalytics::predMvX;
int TComAnalytics::predMvY;

int TComAnalytics::lastPUx;
int TComAnalytics::lastPUy;
int TComAnalytics::lastPUh;
int TComAnalytics::lastPUw;
int TComAnalytics::lastPUDepth;

long TComAnalytics::wild_counter;

bool TComAnalytics::isIntra;

double TComAnalytics::nEvalPUs[MAX_CU_DEPTH];

long TComAnalytics::luma_transf[MAX_CU_DEPTH];
long TComAnalytics::chroma_transf[MAX_CU_DEPTH];
long TComAnalytics::skip_count[MAX_CU_DEPTH];

double TComAnalytics::nCandidatesFSME;
double TComAnalytics::nCandidatesTZSME;
double TComAnalytics::nCandidatesFME;

ofstream TComAnalytics::candidatesFile;

TComAnalytics::TComAnalytics() {
}

void TComAnalytics::globalReset(){
    wild_counter = 0;
    for(int i = 0; i < MAX_CU_DEPTH; i++){
        nEvalPUs[i] = 0.0;    
        luma_transf[i] = 0;
        chroma_transf[i] = skip_count[i] = 0;
    }
    isIntra = true;
    lastPUMode= SIZE_NONE;
    nCandidatesFSME = nCandidatesTZSME = nCandidatesFME = 0.0; // counted as M cands
    candidatesFile.open("candidates.csv",ofstream::out);
    candidatesFile << "Mode\tx\ty\tw\th\tpred mv X\tpred mv Y" << endl;
}

void TComAnalytics::setSearchPartition(TComDataCU* pcCU, int numPart){
    currCU = pcCU;
    isIntra = false;
    int x,y,w,h;
    unsigned int uiAbsPartIdx;
    PartSize PUMode;
    
    //string PUModeMap[] = {"2Nx2N","2NxN","Nx2N","NxN","2NxnU","2NxnD","nLx2N","nRx2N"};

    currCU->getPartPosition(currCU->getZorderIdxInCU(),x,y,w,h);
    currCU->getPartIndexAndSize(currCU->getZorderIdxInCU(), uiAbsPartIdx, w,h);
    PUMode = currCU->getPartitionSize(uiAbsPartIdx);
    
    if (PUMode == lastPUMode){
        switch(PUMode){
            case SIZE_2NxN:
            case SIZE_2NxnU:
            case SIZE_2NxnD: y += lastPUh; break;
            
            case SIZE_Nx2N:
            case SIZE_nLx2N:
            case SIZE_nRx2N: x += lastPUw; break;
            
            default: break;
        }
    }
    
    lastPUMode = PUMode;
    lastPUDepth = currCU->getDepth(uiAbsPartIdx);
    lastPUx = x;
    lastPUy = y;
    lastPUh = h;
    lastPUw = w;
    nEvalPUs[pcCU->getDepth(uiAbsPartIdx)] += (1.0*numPart);
   // candidatesFile << endl << PUModeMap[PUMode] << "\t"  << x << "\t"  << y << "\t"  << w << "\t"  << h << "\t" << predMvX << "\t" << predMvY << endl;
}

void TComAnalytics::setPredMv(int x, int y){
    predMvX = x;
    predMvY = y;
}

void TComAnalytics::setMv(int mvX, int mvY, string func){
    double sumX, sumY;
    


    if(func == "FME"){
        sumX = lastPUy + (mvX >> 2) + (predMvX >> 2);
        sumY = lastPUy + (mvY >> 2) + (predMvY >> 2);
        
        sumX += (mvX & 0x03) * 0.25; // mv offset
        sumX += (predMvX & 0x03) * 0.25; // pred mv offset

        sumY += (mvY & 0x03) * 0.25; // mv offset
        sumY += (predMvY & 0x03) * 0.25; // pred mv offset

        nCandidatesFME += (lastPUw*lastPUh*1.0)/(4096*1000);

    }
    else{
        sumX = lastPUy + mvX + (predMvX >> 2);
        sumY = lastPUy + mvY + (predMvY >> 2);
        if (func == "TZSME")
            nCandidatesTZSME += (lastPUw*lastPUh*1.0)/(4096*1000);
            //nCandidatesTZSME += (1.0)/(1000);
        else{
            nCandidatesFSME += (lastPUw*lastPUh*1.0)/(4096*1000);
            //nCandidatesFSME += (1.0)/(1000);
        }
    }

    
    
    //if (sumX < 0 or sumY < 0)
        //return;
    //else
        candidatesFile << "(" << sumX << "," << sumY << ")" << "\t";
        return;
}

void TComAnalytics::printPUEvalReport(){
    printf(" \n\n #Inter PUs evaluated:  ");
    for(int i = 0; i < 4; i++){
      printf("d%d: %.0f  ", i,TComAnalytics::nEvalPUs[i]);
      nEvalPUs[i] = 0.0;
    }
}

void TComAnalytics::printCandidatesReport(){

            luma_transf[0] += (4*(1+4+4*4+4*4*4))*skip_count[0];
            luma_transf[1] += ((1+4+4*4+4*4*4))*skip_count[1];
            luma_transf[2] += ((1+4+4*4))*skip_count[2];
            luma_transf[3] += ((1+4))*skip_count[3];
        
            chroma_transf[0] += (4*(1+4+4*4+4))*skip_count[0];
            chroma_transf[1] += ((1+4+4*4))*skip_count[1];
            chroma_transf[2] += ((1+4))*skip_count[2];
            chroma_transf[3] += ((1))*skip_count[3];
    printf("\n #Inter TUs evaluated:  ");
    for(int i = 0; i < 5; i++){
            printf("d%d: %ld  ", i,luma_transf[i]);
    }
    printf("\n                        ");
    for(int i = 0; i < 5; i++){
            printf("d%d: %ld  ", i,chroma_transf[i]);
    
    }
    //printWildCounter();
    wild_counter = 0;
            
  
  printf("\n\n #FSME candidates*10^3: %.2f\n",nCandidatesFSME);
  printf(" #TZSME candidates*10^3: %.2f\n",nCandidatesTZSME);
  printf(" #FME candidates*10^3: %.2f\n\n",nCandidatesFME);
  
  resetCounters();
  
}

void TComAnalytics::resetCounters(){
    nCandidatesFSME = nCandidatesTZSME = nCandidatesFME = 0.0;
    for(int i = 0; i < 5; i++){
        luma_transf[i] = chroma_transf[i] = skip_count[i]= 0;
    }
}

void TComAnalytics::incTransfLuma(unsigned int d){
        luma_transf[d]++;
}

void TComAnalytics::incTransfChroma(unsigned int d){
        chroma_transf[d]++;
}

void TComAnalytics::incSkipMode(unsigned int d){
    skip_count[d]++;
}

void TComAnalytics::printWildCounter(){
    printf("\n%ld\n", wild_counter);
}