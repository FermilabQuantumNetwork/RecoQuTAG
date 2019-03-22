#include <iostream>
#include <fstream>
#include <map>
//ROOT
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

int main( int argc, char** argv )
{
  gROOT->Reset();
  TFile* fout = new TFile("test_histo.root", "recreate");
  TH1F* h = new TH1F("histo","histo", 1000, -10000,10000);
  TTree* qutag = new TTree("qutag", "qutag time stamps");
  //setting TTree branches
  int evt;
  unsigned long long q_time[4];
  qutag->Branch("evt", &evt, "evt/I");
  qutag->Branch("time", q_time, "time[4]/l");

  std::ifstream ifs;
  ifs.open("/home/sxie/QUTAG-LX64-V1.1.6/userlib/src/data_test.txt", std::ifstream::in);
  
  
  unsigned long long time[50];
  int ch[50];
  char tmp[250];
  ifs.getline(tmp, 256);
  std::cout << tmp << std::endl;
  int prev_ch = 666;
  unsigned int i_coin=0;
  const int active_chs = 10;
  unsigned long long last_time = 0;
  int last_ch = -666666;
  int max_delta_time_arr[1000];
  int event_coinc_window = 0;
  while (ifs.good()){

    while( i_coin < 100 ){

      for( unsigned int i = 0; i < active_chs; i++){
	ifs >> ch[i] >> time[i];
	if( ifs.eof() ) break;
      }
      
      int max_delta_time = -66666666666666;
      for( unsigned i = 0; i < active_chs; i++){
	int current_delta_time = time[i]-last_time;
	int current_delta_ch = abs(ch[i]-last_ch);
	last_time = time[i];
	last_ch = ch[i];
	
	if( current_delta_time > max_delta_time  && current_delta_ch < 5){
	  max_delta_time = current_delta_time;
	}
      
      }
      //std::cout << "max delta time = " <<  max_delta_time << std::endl;
      max_delta_time_arr[i_coin] = max_delta_time;
      i_coin++;
    }
    
    float mean_delta_time = 0;
    float rms_delta_time = 0;
    for ( unsigned i = 0; i < i_coin-1; i++ ){
      mean_delta_time += max_delta_time_arr[i]/float(i_coin-1);
    }
    
    for ( unsigned i = 0; i < i_coin; i++ ){
      rms_delta_time += pow(max_delta_time_arr[i]-mean_delta_time,2.);
    }
    rms_delta_time = sqrt(rms_delta_time/(i_coin-2));
    //printf( "mean: %.1f , rms: %.1f", mean_delta_time, mean_delta_time-10.0*rms_delta_time);
    //event_coinc_window = int( mean_delta_time-100.0*rms_delta_time);
    event_coinc_window = int( mean_delta_time/2.0);
    //event_coinc_window = 80000;
    break;
    //if ( prev_ch != ch && ch < 5 && prev_ch < 5 ) h->Fill(time-prev_time);
    //std::cout << time-prev_time << " " << prev_ch << " " <<  ch << std::endl;
    //prev_ch = ch;
    //prev_time = time;
    //std::cout << time << " " << channel << std::endl;
  }
  
  std::cout << "event_coinc_window: " << event_coinc_window << std::endl;
  std::map<int, unsigned long long> time_ch_map;
  bool new_evt = false;
  bool init_event = false;
  bool good_event = true;
  while( ifs.good()){
    for( unsigned i = 0; i < active_chs; i++){
      ifs >> ch[i] >> time[i];
      if( ifs.eof() ) break;
    }
    
    for( unsigned i = 0; i < active_chs; i++){
      int current_delta_time = time[i]-last_time;
      int current_delta_ch = abs(ch[i]-last_ch);
      if(current_delta_time > event_coinc_window  && current_delta_ch < 5 ){
	//std::cout << "NEW EVENT" << std::endl;
	new_evt = true;
      }
      else{
	new_evt = false;
      }
      
      if(new_evt){
	//do not write only one channel
	if(time_ch_map.size()<=1) good_event = false; 
	if( good_event ) 
	  {
	    //std::cout << "==map==" << std::endl;
	    unsigned long long trigger_time = time_ch_map[1];
	    for( auto& tmp : time_ch_map ){
	      //std::cout << tmp.first << " " << tmp.second << std::endl;
	      q_time[tmp.first-1] = tmp.second-trigger_time;
	    }
	    
	    qutag->Fill();
	  }
	//std::cout << "============" << std::endl;
	for(int iich = 0; iich < 4; iich++) q_time[iich] = -666;
	evt++;
	good_event = true;
	time_ch_map.clear();
	if( time_ch_map.find(ch[i]) == time_ch_map.end() && ch[i] < 5 ) time_ch_map[ch[i]] = time[i];
      }
      else{
	if( time_ch_map.find(ch[i]) == time_ch_map.end() && ch[i] < 5 ) time_ch_map[ch[i]] = time[i];
	else good_event = false;
      }
      
      //std::cout << current_delta_time << " " << ch[i] << " " << last_ch << " " <<  time[i]  << std::endl;
      last_time = time[i];
      last_ch = ch[i];
    }
    
    if( evt % 100000 == 0) std::cout << "evt: " << evt << std::endl;
  }
  
  
  ifs.close();
  qutag->Write();
  h->Write();
  fout->Close();
  return 0;
}
