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

int main( int argc, char** argv ){

								Mat depthIn, labelIn, labelRszd;

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
								}

								// resize label image to depth image size (just in case different)

								resize(labelIn, labelRszd, depthIn.size());

								// normalize (stretch) labels and convert to grayscale to ensure unqiue
								// mapping (for low numbers of labels, i.e. < 255)

								normalize(labelRszd, labelRszd, 0, 255, CV_MINMAX);
								cvtColor(labelRszd, labelRszd, CV_BGR2GRAY);

								// define depth completion object

								DepthComp holes;

								// perform pre-processing and write output to file

								depthIn = holes.preProcess(depthIn, labelRszd);
								holes.writeImg(argv[1], "-PROCESSED", depthIn);

								//to write time and result information to a file
								ofstream myfile;
								myfile.open ("data.txt", ios::ate );

								// the very main while loop to run several times with rotated image
								// row-wise and then column-wise (with latter being rotation of former)
								// with final secondary row-wise pass if needed

								while((holes.times == 0) || ((holes.times == 1) && (holes.holesExist == 1)) ||
														((holes.times == 2) && (holes.holesExist == 2))) {

																// we are running the same filling process one (more) time

																holes.times++;

																// get start time

																auto start = get_time::now();

																// perform identification and filling of holes - one pass

																depthIn = holes.identFillHoles(depthIn, labelRszd);

																// get end time

																auto end = get_time::now();
																auto diff = end - start;

																// **** perform logging

																if(myfile.is_open()) {

																								myfile <<"********************************************************** RUN (( " << holes.times << " )) *************************************************************\n\n";

																								myfile << "Important Information\n";
																								myfile << "For depth image \"" << argv[1] << "\" and segmented image \"" << argv[2] << "\" in the run number " << holes.times << endl;

																								myfile << "\nIn the image, the number of ROWS is :  " << "\t\t\t" << depthIn.rows << "\n\t\tAnd the number of COLUMNS is : " << "\t\t\t" << depthIn.cols << endl;


																								myfile <<"\nThe frequency of the cases in run number" << holes.times << endl;
																								myfile << "\nnumber of case 1  holes = \t" << holes.case1;
																								myfile << "\nnumber of case 2  holes = \t" << holes.case2;
																								myfile << "\nnumber of case 3  holes = \t" << holes.case3;
																								myfile << "\nnumber of case 4  holes = \t" << holes.case4;
																								myfile << "\nnumber of case 5  holes = \t" << holes.case5;
																								myfile << "\nnumber of case 6  holes = \t" << holes.case6;
																								myfile << "\nnumber of case 7  holes = \t" << holes.case7;
																								myfile << "\nnumber of case 8  holes = \t" << holes.case8;
																								myfile << "\nnumber of case 9  holes = \t" << holes.case9;
																								myfile << "\nnumber of case 10 holes = \t" << holes.case10;
																								myfile << "\nnumber of case 11 holes = \t" << holes.case11;
																								myfile << "\nnumber of case 12 holes = \t" << holes.case12;

																								myfile <<"\n\nThe TIME:\n";
																								myfile << "Elapsed time is :  " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()<< " milliseconds " << "\n\n";

																								myfile <<"********************************************************** RUN (( " << holes.times << " )) *************************************************************\n\n\n\n\n\n";

																}

																// **** end logging

																// check to see of there are any hole pixels left in the image

																for(int i=0; i < (depthIn.rows); i++) { // for loop checking rows for holes
																								for(int j = 0; j < (depthIn.cols); j++) { // for loop checking columns for holes

																																if(depthIn.at<uchar>(i,j) == 0) { // checking to see if holes remain

																																								if (holes.times == 1) { // the thing has been run ONCE and now we have to flip it

																																																holes.holesExist = 1; // this means holes still exist after the FIRST run

																																																goto nestBreak; //goto is used to break out of a nested loop

																																								} // end of if for when the thing has been run ONCE and we have to flip it


																																								else if (holes.times == 2) { // the thing has been run TWICE and flipped already so we now flip it back

																																																holes.holesExist = 2; // this means holes still exist after the SECOND run

																																																goto nestBreak; //goto is used to break out of a nested loop

																																								} // end of else of for when the thing has been run TWICE and flipped already so we now flip it back

																																								else if (holes.times == 3) { // the thing has been run THREE times and flipped twice already

																																																holes.holesExist = 3; // this means holes still exist after the SECOND run

																																																goto nestBreak; //goto is used to break out of a nested loop

																																								} // end of else if for the thing has been run THREE times and flipped twice already

																																} // end of if for checking if holes remain

																								} // end of for loop checking columns for holes
																} // end of for loop checking rows for holes

nestBreak:

																if((holes.times == 1) && (holes.holesExist == 0)) { // this means it has been run once and all holes are FILLED before any flipping

																								printf("process was run once only and ALL holes are filled.\n");

																} // end of else if for when it has been run once and all holes are FILLED before any flipping


																else if((holes.times == 1) && (holes.holesExist == 1)) {
																								// this means it has been run once and now we must flip to do column-wise


																								// flipping all image clockwise

																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);

																} // end of if for when it has been run once and now we must flip

																else if((holes.times == 2) && (holes.holesExist == 2)) {
																								// this means it has been run twice and holes still exist and now we must
																								// flip again for for a secondary (final) row-wise pass

																								// flipping all image clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);

																} // end of else if for when it has been run twice and holes still exist and now we must flip

																else if((holes.times == 2) && (holes.holesExist == 1)) {
																								// this means it has been run twice and all holes are FILLED and now we must flip back

																								// flipping all image counter-clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 0);
																								flip(labelRszd, labelRszd, 0);

																} // end of else if for when it has been run twice and all holes are FILLED and now we must flip

																else if((holes.times == 3) && (holes.holesExist == 3)) { // this means it has been run THREE and holes still exist

																								// flipping all image clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);
																								// flipping all image clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);

																} // end of else if for when it has been run THREE and holes still exist

																else if((holes.times == 3) && (holes.holesExist == 2)) { // this means it has been run THREE and holes still exist

																								// flipping all image clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);
																								// flipping all image clockwise
																								transpose(depthIn, depthIn);
																								transpose(labelRszd, labelRszd);
																								flip(depthIn, depthIn, 1);
																								flip(labelRszd, labelRszd, 1);

																} // end of else if for when it has been run THREE and holes still exist

								}// end of the main while loop

								// close logging file

								myfile.close();

								// write filled depth image to file

								holes.writeImg(argv[1], "-FILLED", depthIn);

								cout << "\nProcess Done.\n";
								cout << "Process Log: run-times and number of filling cases saved in - \"data.txt\".\n";
								return 0; // end of program

}// end of main

//******************************************************************************
