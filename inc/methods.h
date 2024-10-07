#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime> 


#include "TTree.h"
#include "TFile.h"
#include "TKey.h"


int parseLine(char* line);
int getValueVM(); //Note: this value is in KB!
int getValuePM();  //Note: this value is in KB!
void printMemoryUsage(long entry, long event_print);
TTree* getTreeFromRoot(TFile* rootfile);
std::vector<TBranch*> getAllBranchFromTree(TTree* tree);