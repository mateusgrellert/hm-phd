/* 
 * File:   TComAnalytics.h
 * Author: grellert
 *
 * Created on May 13, 2014, 10:42 AM
 */

#ifndef TCOMANALYTICS_H
#define	TCOMANALYTICS_H

#define EN_ANALYTICS 1

#include <fstream>
#include <string>

#include "TComDataCU.h"
#include "TypeDef.h"

class TComAnalytics {

public:
    static bool isIntra;
    static bool accountTU;
    static int predMvX, predMvY;
    static int lastPUx, lastPUy;
    static int lastPUh, lastPUw, lastPUDepth;
    static double nEvalPUs[MAX_CU_DEPTH];
    static double nCandidatesFSME;
    static double nCandidatesTZSME;
    static double nCandidatesFME;
    static long wild_counter, luma_transf[MAX_CU_DEPTH],chroma_transf[MAX_CU_DEPTH], skip_count[MAX_CU_DEPTH];
    static PartSize lastPUMode;
    static TComDataCU* currCU;
    static std::ofstream candidatesFile;

    TComAnalytics();
    // initializers for each level of information
    static void globalReset();
    static void GOPReset();
    static void frameReset();
    static void CTUReset();
    
    static void setSearchPartition(TComDataCU*, int numPart);
    static void setMv(int, int,string);
    static void setPredMv(int, int);
    static void printPUEvalReport();
    static void printCandidatesReport();
    static void resetCounters();
    static void incTransfLuma(unsigned int d);
    static void incTransfChroma(unsigned int d);
    static void incSkipMode(unsigned int d);
    static void printWildCounter();
};
    
    

#endif	/* TCOMANALYTICS_H */

