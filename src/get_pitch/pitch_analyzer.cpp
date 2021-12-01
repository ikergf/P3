/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
#include "pav_analysis.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \TODO Compute the autocorrelation r[l]
      /// \DONE Cálculo de la autocorrelación
      r[l]=0;
      for (unsigned int n=l; n<x.size(); n++) {
          r[l] += x[n-l]*x[n];
      }
    }
    
    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 

  }

  void PitchAnalyzer::set_window(Window win_type) {
    int i;
    float a0 = 0.53836, a1 = 0.46164, pi=3.14159265;
    int L = frameLen;
    float hamming[L];

    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \DONE Ventana Hamming hecha
      for (i=0; i<L; i++){

        hamming[i]= a0 -a1*cos(2*pi*i/(L-1));
        window.assign(i,hamming[i]);

      }

      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float Z) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.

    /// \DONE Regla para decidir si el sonido es sordo o sonoro hecha.

    // Podríamos introducir la tasa de cruces por 0.
    if(rmaxnorm> 0.35 && pot>-40 && r1norm>0.37 && Z<3000){
      return false;
    }else{
      return true;
    }
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = r.begin() + npitch_min;

    float Z=0;

    for (unsigned int i = 1; i < frameLen; i++){
        
        if((x[i]<0 && x[i-1]<0) || (x[i]>0 && x[i-1]>0)){
            Z+=0;
        }else{
            Z += 1;
        }
    }

    Z=((samplingFreq/2)*Z)/(frameLen-1);


    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.

  /// \DONE Lag entre máximo en el origen y máximo secundario encontrado.

for(iR = r.begin() + npitch_min; iR < r.begin()+npitch_max; iR++){
  if(*iR > *iRMax){
    iRMax = iR; //Vamos iterando y, si el valor de iR es mayor a iRMax, entonces iRMax pasará a valer iR. 
                //De esta forma, encontraremos el primer máximo secundario de la autocorrelación
  }
}

unsigned int lag = iRMax - r.begin(); //El lag será la distancia entre el valor máximo secundario (iRMax) y
                                      // el máximo total de la autocorrelación (en r[0])

float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 1
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << '\t' << Z << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0], Z))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
