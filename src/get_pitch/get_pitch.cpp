/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

FILE *data=fopen("data.txt","w");
int llindar = 0.008087158203125; 
float mediana[3];
float aux;


static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  /// \DONE Preprocesado usando central-clipping.

for(unsigned int i=0; i<x.size(); i++){
  if(abs(x[i])<llindar){
      x[i]=0;
  } 
  else if (x[i]>=llindar){
      x[i]=x[i]-llindar;
  } 
  else {
      x[i]=x[i]+llindar;
  }

}
  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  /// \DONE Postprocesado usando un filtro de mediana

for (unsigned int i=0; i<f0.size(); i++){
  
    for (int j=-1; j<=1; j++){
      if(i>0 && i<(f0.size())){
        mediana[j+1]=f0[j+i];
      } else if(i==0){
        mediana[0]=0;
        mediana[1]=f0[0];
        mediana[2]=f0[1];
      } else if(i==f0.size()){
        mediana[0]=f0[f0.size()-1];
        mediana[1]=f0[f0.size()];
        mediana[2]=0;
      }
    }
    for (int m=0; m<3-1; m++){
      for (int n=m+1; n<3; n++){
        if(mediana[m]>mediana[n]){
          aux = mediana[n];
          mediana[n] = mediana[m];
          mediana[m] = aux;
        }
      }
    }
    f0[i]=mediana[1];
  
}


  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur


  return 0;
}
