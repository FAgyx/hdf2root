#include "inc/WaveAttr.h"

double WaveAttr::time_range_left = 0;
double WaveAttr::time_range_right = 0;
int    WaveAttr::n_points = 0;

WaveAttr::WaveAttr(TTree* tree_attr, std::vector<int> chnls){
  TBranch *channel_horiz_offset = tree_attr->
    GetBranch(("channel"+std::to_string(chnls.at(0))+"_horiz_offset").c_str());
  double horiz_offset;
  channel_horiz_offset->SetAddress(&horiz_offset);
  channel_horiz_offset->GetEntry(0);
  std::cout<<"horiz_offset = "<<horiz_offset<<std::endl;
  TBranch *channel_horiz_interval = tree_attr->
    GetBranch(("channel"+std::to_string(chnls.at(0))+"_horiz_interval").c_str());
  double horiz_interval;
  channel_horiz_interval->SetAddress(&horiz_interval);
  channel_horiz_interval->GetEntry(0);
  std::cout<<"horiz_interval = "<<horiz_interval<<std::endl;
  TBranch *channel_nom_subarray_count = tree_attr->
    GetBranch(("channel"+std::to_string(chnls.at(0))+"_nom_subarray_count").c_str());
  int nom_subarray_count;
  channel_nom_subarray_count->SetAddress(&nom_subarray_count);
  channel_nom_subarray_count->GetEntry(0);
  std::cout<<"nom_subarray_count = "<<nom_subarray_count<<std::endl;
  TBranch *channel_wave_array_count = tree_attr->
    GetBranch(("channel"+std::to_string(chnls.at(0))+"_wave_array_count").c_str());
  int wave_array_count;
  channel_wave_array_count->SetAddress(&wave_array_count);
  channel_wave_array_count->GetEntry(0);
  std::cout<<"wave_array_count = "<<wave_array_count<<std::endl;
  
  for (auto chnl : chnls){
    TBranch *pb_channel_vertical_gain = tree_attr->
      GetBranch(("channel"+std::to_string(chnl)+"_vertical_gain").c_str());
    double vertical_gain;
    pb_channel_vertical_gain->SetAddress(&vertical_gain);
    pb_channel_vertical_gain->GetEntry(0);
    std::cout<<"Channel "<<chnl<<" vertical_gain = "<<vertical_gain<<std::endl;
    vertical_gains.push_back(vertical_gain);
    TBranch *pb_channel_vertical_offset = tree_attr->
      GetBranch(("channel"+std::to_string(chnl)+"_vertical_offset").c_str());
    double vertical_offset;
    pb_channel_vertical_offset->SetAddress(&vertical_offset);
    pb_channel_vertical_offset->GetEntry(0);
    std::cout<<"vertical_offset = "<<vertical_offset<<std::endl;   
    vertical_offsets.push_back(vertical_offset);
  }
  n_points = wave_array_count/nom_subarray_count;
  time_range_left = horiz_offset;
  time_range_right = horiz_offset+horiz_interval*n_points;

}