#include "inc/methods.h"

int parseLine(char* line){
  // This assumes that a digit will be found and the line ends in " Kb".
  int i = strlen(line);
  const char* p = line;
  while (*p <'0' || *p > '9') p++;
  line[i-3] = '\0';
  i = atoi(p);
  return i;
}

int getValueVM(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];

  while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmSize:", 7) == 0){
          result = parseLine(line);
          break;
      }
  }
  fclose(file);
  return result/1024;
}

int getValuePM(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];

  while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmRSS:", 6) == 0){
          result = parseLine(line);
          break;
      }
  }
  fclose(file);
  return result/1024;
}

void printMemoryUsage(long entry, long event_print){
  std::time_t current_time_date = std::chrono::system_clock::to_time_t(
  std::chrono::system_clock::now());
  auto gmt_time = gmtime(&current_time_date);
  auto timestamp = std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S");
  std::cout << "Processing event " << entry <<" at "<<
  timestamp<<", VM="<<getValueVM()<<" MB, PM="<<getValuePM()<<" MB"<<std::endl;
}


TTree * getTreeFromRoot(TFile* rootfile){
  TTree *tree;
  TIter next(rootfile->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {  //loop to find a TTree
    if (strstr(key->GetClassName(),"TTree")) {
      rootfile->GetObject(key->GetName(), tree);
      break;
    }
  }
  tree->ls();
  tree->Print();
  return tree;
}

std::vector<TBranch*> getAllBranchFromTree(TTree* tree){
  TBranch *pb;
  std::vector<TBranch*> pbs;
  TIter next_pb(tree->GetListOfBranches());
  while ((pb = (TBranch*)next_pb())) {  //push all branches to pbs
      pbs.push_back(pb);
  }
  return pbs;
}

