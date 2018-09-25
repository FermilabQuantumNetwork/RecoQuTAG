#include <iostream>
#include <fstream>
#include <string>
#include <istream>

int main ()
{
  std::string data = "timestamps.bin";
  std::ifstream ifs (data.c_str(), std::ifstream::binary);

  const int header_size = 40;
  const int ts_size = 10;
  //const int channel_size = 2;
  char header[header_size];
  char ts[ts_size];
  char ts_NO[8];//Normal ordering
  //char channel[channel_size];
  if ( ifs.is_open() )
    {
      ifs.read( header, sizeof(char)*header_size);
      std::cout << "header:"  << header << std::endl;
      while ( ifs.good() )
      {
        int channel = 0;
        unsigned long long my_ts;
        ifs.read( ts, sizeof(char)*ts_size);
        for( int i = 0; i < 8; i++)  my_ts += (ts[i] << 8*i);
        for( int i = 0; i < 2; i++) channel += (ts[8+i] << 8*i);

        //channel = line_ts[8] + (line_ts[9] << 8);
        //unsigned long my_ts = atoll(ts_NO);
        std::cout << channel << " " << my_ts << std::endl;
        if( ifs.eof() ) break;
      }
      ifs.close();
    }
  else
    {
      std::cout << "Unable to open binary file" << std::endl;
    }

};
