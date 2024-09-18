#ifndef WAVEATTR
#define WAVEATTR

#include "TTree.h"
#include <iostream>




class WaveAttr {
public:
  WaveAttr(TTree* tree_attr, std::vector<int> chnls);
  
  static double time_range_left;
  static double time_range_right;
  static int n_points;
  std::vector<double> vertical_gains;
  std::vector<double> vertical_offsets;
  
private:
  

}; //class WaveAttr


#endif
