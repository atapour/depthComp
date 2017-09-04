//******************************************************************************
/*
   DepthComp : efficient depth filling appraoch built on segmentation

   Requirements: depth image / segmented image

   Usage: ./depthComp <path_to_depth_image> <path_to_segmented_image>

   Implementation of DepthComp - Atapour-Abarghouei / Breckon, Proc. BMVC 2017.

   Author : Amir Atapour-Abarghouei, amir.atapour-abarghouei@durham.ac.uk

   Copyright (c) 2017 Engineering and Computing Sciences, Durham University
   License : GPL - http://www.gnu.org/licenses/gpl.html
 */
//******************************************************************************

#include "depthComp.hpp"

using namespace cv;
using namespace std;

// *****************************************************************************

void writeOutputImg(const String& imgName, const String& sufix, const Mat& mat)
{
								assert(!mat.empty());
								string delimiter = ".png";
								string token = imgName.substr(0, imgName.find(delimiter));
								string outFileName = token + sufix + delimiter;
								imwrite(outFileName, mat);
}

// *****************************************************************************

int main( int argc, char** argv ){

								Mat depthIn; 						// input depth image (with holes)
								Mat labelIn, labelRszd; // input segmented label image
								Mat depthOut ;					// output depth image (completed)

								// check arguments and read in images

								if(argc == 3) {

																if ((depthIn = imread(argv[1], IMREAD_GRAYSCALE)).empty()) {
																								cout << "\nCannot open file " << argv[1] << endl;
																								exit(-1);
																}

																if ((labelIn = imread(argv[2], IMREAD_COLOR)).empty()) {
																								cout << "\nCannot open file " << argv[2] << endl;
																								exit(-1);
																}
								} else {

									cout << "Usage: ./depthComp <depth_image> <segmented_image>" << endl;
									exit(-1);

								}

								// resize label image to depth image size (just in case different)

								resize(labelIn, labelRszd, depthIn.size());

								// normalize (stretch) labels and convert to grayscale to ensure unqiue
								// mapping (for low numbers of labels, i.e. < 255)

								normalize(labelRszd, labelRszd, 0, 255, CV_MINMAX);
								cvtColor(labelRszd, labelRszd, CV_BGR2GRAY);

								// define depth completion object

								DepthComp completer;

								// perform pre-processing and write output to file

								depthIn = completer.preProcess(depthIn, labelRszd, true);
								writeOutputImg(argv[1], "-PROCESSED", depthIn);

								// perform depthComp based depth completion
								// recording stats of the process out to data.txt

								depthOut = completer.identFillHoles(depthIn, labelRszd, true);

								// write filled depth image to file

								writeOutputImg(argv[1], "-FILLED", depthOut);

								return 0; // end of program

}// end of main

//******************************************************************************
