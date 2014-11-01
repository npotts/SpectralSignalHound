#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <cmath>
#include "CUSBSA.h"

using namespace std;

int main(int argc, char *argv[]) {
  CUSBSA sighound;
  cout << "Signal Hound Cal Data Extractor starting" << endl;
  int err = sighound.Initialize( 0 );
  cout << ( err ? "Unable to connect to Signal Hound" : "Connected to Signal Hound" ) << endl;
  if (!err) { //dump out cal table to file
    ofstream raw("cal-data.h");   raw << "unsigned char sh_cal_data[] = {";
    for(unsigned int i=0; i<sizeof(sighound.m_CalTable); i++) 
      raw << int(sighound.m_CalTable[i]) << ",";
    raw << "};" << endl;
    raw.close();
    cout << "raw cal data -> 'cal-data.h'" << endl;

    ofstream bin("cal-data.cal", ios::binary);
    bin.write((char *) sighound.m_CalTable, sizeof(sighound.m_CalTable));
    cout << "bin cal data -> 'cal-data.cal'" << endl;
    bin.close();
  }
}