/*
 DepthComp : efficient depth filling appraoch built on segmentation

 Requirements: depth image / segmented image

Implementation of DepthComp - Atapour-Abarghouei / Breckon, Proc. BMVC 2017.

 Author : Amir Atapour-Abarghouei, amir.atapour-abarghouei@durham.ac.uk

 Copyright (c) 2017 Engineering and Computing Sciences, Durham University
 License : GPL - http://www.gnu.org/licenses/gpl.html

***********************************************************************************
*/

#include "depthComp.hpp"


DepthComp::DepthComp(){

case1 = 0;
case2 = 0;
case3 = 0;
case4 = 0;
case5 = 0;
case6 = 0;
case7 = 0;
case8 = 0;
case9 = 0;
case10 = 0;
case11 = 0;
case12 = 0;

times = 0; // this shows how many times we have run this over the image
holesExist = 0; // this is used to check if holes still exist after each run
holeFound = 0;
}


DepthComp::~DepthComp(){
	// all deallocating is done automatically
}


Mat DepthComp::preProcess(Mat depthIn, const Mat& labelRszd){

	normalize(depthIn, depthIn, 0, 255, CV_MINMAX);
	depthIn.convertTo(depthIn, CV_16SC1);
	filterSpeckles(depthIn, 0, 200, 1);
	depthIn.convertTo(depthIn, 0);
	normalize(depthIn, depthIn, 0, 255, CV_MINMAX);

	// the goal is to remove wrong disparity values

	for(int i=0 ; i < (depthIn.rows) ; i++) {  // loop over the rows
		for (int j = 1; j < (depthIn.cols); j++) {  // loop over the column


			if ((depthIn.at<uchar>(i, j + 1) == depthIn.at<uchar>(i, j - 1)) && (labelRszd.at<uchar>(i, j + 1) == labelRszd.at<uchar>(i, j - 1)) &&
				(depthIn.at<uchar>(i, j + 1) != 0) && (depthIn.at<uchar>(i, j + 1) != depthIn.at<uchar>(i, j))  ) {
				depthIn.at<uchar>(i, j) = 0;
			}
			if ((depthIn.at<uchar>(i, j + 2) == depthIn.at<uchar>(i, j - 1)) && (labelRszd.at<uchar>(i, j + 2) == labelRszd.at<uchar>(i, j - 1)) &&
				(depthIn.at<uchar>(i, j + 2) != 0) && (depthIn.at<uchar>(i, j + 2) != depthIn.at<uchar>(i, j)) && (depthIn.at<uchar>(i, j) != 0) && (depthIn.at<uchar>(i, j + 1) != 0)	) {
				depthIn.at<uchar>(i, j) = 0;
				depthIn.at<uchar>(i, j + 1) = 0;
			}

		} // end of loop over the columns
	} // end of loop over the rows


	return depthIn;
}

void DepthComp::writeImg(const String& imgName, const String& sufix, const Mat& mat)
{
	assert(!mat.empty());
	string delimiter = ".png";
	string token = imgName.substr(0, imgName.find(delimiter));
	string outFileName = token + sufix + delimiter;
	imwrite(outFileName, mat);
}



Mat DepthComp::identFillHoles(Mat depthIn, const Mat& labelRszd){


	int begin1; // beginning of hole in a row
	int end1; // end of hole in a row
	int hLen; // length of hole in a row
	int cBegin1; // beginning of the sampling area
	int cEnd1; // end of the sampling area


	int rightInfo = 1; // this is used to show that information on the right does not include holes.
// When it is ZERO, there ARE HOLES, so the information is NOT AVAILABLE.
// When it is ONE, the are NO HOLES, so the information on the right is AVAILABLE.



	int caseNo = 0; // specifies the case number as described in the notebook. When it is zero, no case number has been assigned.




	double tempArray[depthIn.cols]; // this is used for temporarily holding the fillings to remove accumilating errors



	// check the depth row by row until you reach a black pixel and set the variable 'begin1' as the position of the first black pixel.
	// i is ROW
	// j is COLUMN
	for(int i=0 ; i < (depthIn.rows) ; i++) {  // loop over the rows
		for(int j = 0; j < (depthIn.cols); j++) {  // loop over the columns

			if(depthIn.at<uchar>(i,j) == 0){ // we have reached a hole in a line




//region caseIdentification

				holeFound++;

				begin1 = j;  //beginning of the hole

				if(begin1 != 0) {  // when the hole does NOT begin1 in the begin1ning of the line
					while ((labelRszd.at<uchar>(i, j) == labelRszd.at<uchar>(i, (j + 1))) && (depthIn.at<uchar>(i, j) == 0)  &&  (j != (depthIn.cols - 1))){
						j++;
					} // end1 of while

					if ((j == (depthIn.cols - 1)) || (labelRszd.at<uchar>(i, j) != labelRszd.at<uchar>(i, j + 1))) {
						// either the holes extend1s to the end1 of the line OR the object has end1ed but the hole has not.

						if (depthIn.at<uchar>(i, j) == 0) {  //when we have stopped on the hole
							end1 = j;
							hLen = end1 - begin1 + 1;


							if ((begin1 - hLen) > 0) {  // when there is enough information on the left of the hole
								if (labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i, begin1)) {  // and more than enough information on the left is in the same object
									caseNo = 1;
									case1++;

								}  // end1 of if for case 1
								else if (labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) != labelRszd.at<uchar>(i, begin1) &&
										 labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) {
									// and Just enough information is in the same object
									caseNo = 2;
									case2++;

								}  // end1 of if for case 3
								else if (labelRszd.at<uchar>(i, (begin1 - hLen)) != labelRszd.at<uchar>(i, begin1)) {  // but ALL the information is NOT in the same object

									if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
										caseNo = 3;
										case3++;
									} // end1 of if for case 3
									else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
										caseNo = 12;
										case12++;
									}  // end1 of if for case 12

								} // end1 of else if for when ALL the information is NOT in the same object


							} // end1 of if for when there is enough information on the left of the hole

							else if ((begin1 - hLen) < 0) {  //when there is NOT enough information on the left but some

								if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
									caseNo = 3;
									case3++;
								} //end1 of if for case 3
								else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
									caseNo = 12;
									case12++;
								} //end1 of if for case 12

							} // end1 of if for when there is NOT enough information on the left but some

							else if ((begin1 - hLen) == 0) {  // when the amount of information on the left is EXACTLY the same as the length of the hole

								if (labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) { // there is EXACTLY the same amount of info in the same label on the left
									caseNo = 2;
									case2++;
								} //end1 of if for case 2
								else if (labelRszd.at<uchar>(i, (begin1 - hLen)) != labelRszd.at<uchar>(i, begin1)) { // there is NOT enough information on the left in the same label

									if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
										caseNo = 3;
										case3++;

									} //end1 of if for case 3
									else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
										caseNo = 12;
										case12++;
									} //end1 of if for case 12

								} //end1 of if for when there is NOT enough information on the left in the same label

							}// end1 of else if for when the amount of information on the left is EXACTLY the same as the length of the hole




						}// end1 of if for when when we have stopped on the hole

						else if (depthIn.at<uchar>(i, j) != 0) {  //when the hole does NOT continue to the end1 of the line, but to one pixel before the end1
							end1 = j - 1;
							hLen = end1 - begin1 + 1;


							if ((begin1 - hLen) > 0) {  // when there is enough information on the left of the hole

								if ((labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i, begin1))  && (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1))) {  // and more than enough information on the left is in the same object

									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = -1 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 4;
										case4++;

									} // end1 of if for case 4

									else if(checkLeftLabel == 0){

										int checkLeftLabel = 1; // we assume the left info is all in the same label

										for(int chindex = 0 ; chindex < hLen ; chindex++){
											if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
												checkLeftLabel = 0;
											}
										}

										if(checkLeftLabel == 1){
											caseNo = 5;
											case5++;

										} // end1 of if for case 5

										else{
											caseNo = 11;
											case11++;


										} // end1 of case 11

									}


								}// end1 of if for case 4
								else if ((labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) != labelRszd.at<uchar>(i, begin1)) &&
										 (labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) &&
										 (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1))) {		// and just enough information on the left is in the same object
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								}// end1 of if

								else if ( ( (labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i, begin1)) &&
											(labelRszd.at<uchar>(i,((begin1 - 1))) != labelRszd.at<uchar>(i,begin1)) ) ||
										  ((labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) &&
										   (labelRszd.at<uchar>(i,((begin1 - 1))) != labelRszd.at<uchar>(i,begin1)) ) )	{
									// this means there are in fact no pixels available on the left


									if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
										// there is more enough information on the right in the same label to fill the hole

										rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
										for(int rightInfoIndex = 0 ; rightInfoIndex < hLen + 1 ; rightInfoIndex++) {
											if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
												rightInfo = 0;
											}
										}

										if(rightInfo == 1){ // case 8  .. nothing on the left , more ethan enough on the right
											caseNo = 8;
											case8++;

										} // end1 of case 8

										else if (rightInfo == 0) {

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if
										}


									}// end1 of if for when there is more enough information on the right in the same label to fill the hole


									else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
										// there is NOT MORE than enough info on the right to fill the hole

										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

									else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

										if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object

									} // end1 of else if for when the information on the right is not enough


									else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right, but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									} // end1 of else if for when the information on the right is JUST enough



								} // end1 of else if for when there are in fact no pixels available on the left






								else if (labelRszd.at<uchar>(i, (begin1 - hLen)) != labelRszd.at<uchar>(i, begin1)) {  // but ALL the information is NOT in the same object

									if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
										caseNo = 11;
										case11++;

									} // end1 of if for case 11
									else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
										caseNo = 10;
										case10++;

									}  // end1 of if for case 10

								} // end1 of else if for when ALL the information is NOT in the same object

							}// end1 of if for when there is enough information on the left of the hole


							else if ((begin1 - hLen) < 0) {  //when there is NOT enough information on the left but some

								if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
									caseNo = 11;
									case11++;

								} //end1 of if for case 11
								else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
									caseNo = 10;
									case10++;

								} //end1 of if for case 12

							} // end1 of if for when there is NOT enough information on the left but some

							else if ((begin1 - hLen) == 0) {  // when the amount of information on the left is EXACTLY the same as the length of the hole

								if (labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) { // there is EXACTLY the same amount of info in the same label on the left
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								} //end1 of if
								else if (labelRszd.at<uchar>(i, (begin1 - hLen)) != labelRszd.at<uchar>(i, begin1)) { // there is NOT enough information on the left in the same label

									if (labelRszd.at<uchar>(i, (begin1 - 1)) == labelRszd.at<uchar>(i, begin1)) { // there is at least one pixel on the left in the same label
										caseNo = 11;
										case11++;

									} //end1 of if for case 11
									else if (labelRszd.at<uchar>(i, (begin1 - 1)) != labelRszd.at<uchar>(i, begin1)) { // there are NO pixels on the left in the same label
										caseNo = 10;
										case10++;

									} //end1 of if for case 10

								} //end1 of if for when there is NOT enough information on the left in the same label

							}// end1 of else if for when the amount of information on the left is EXACTLY the same as the length of the hole




						}// end1 of else if for when the hole does NOT continue to the end1 of the line, but to one pixel before the end1










					}// end1 of if that started after the while (when either the holes extend1s to the end1 of the line OR the object has end1ed but the hole has not..)



					if(caseNo == 0){ // a condition in the "while" was not met and NO case number is assigned yet

						if((depthIn.at<uchar>(i,j) != 0) && (labelRszd.at<uchar>(i,j) == labelRszd.at<uchar>(i,j+1))){  // the object has not end1ed but the hole has end1ed
							// which means some stuff is available on the right


							end1 = j - 1;
							hLen = end1 - begin1 + 1;


							if((begin1 - hLen) > 0){  // when there is enough information on the left of the hole

								if((labelRszd.at<uchar>(i,((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i,begin1))  &&  (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1))){
									// and more than enough information is in the same object

									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = -1 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 4;
										case4++;

									} // end1 of if for case 4

									else if(checkLeftLabel == 0){

										int checkLeftLabel = 1; // we assume the left info is all in the same label

										for(int chindex = 0 ; chindex < hLen ; chindex++){
											if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
												checkLeftLabel = 0;
											}
										}

										if(checkLeftLabel == 1){
											caseNo = 5;
											case5++;

										} // end1 of if for case 5
										else{
											caseNo = 11;
											case11++;


										} // end1 of case 11

									}





								}  // possibly and more than enough information is in the same object


								else if((labelRszd.at<uchar>(i,((begin1 - hLen) - 1)) != labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,((begin1 - hLen))) == labelRszd.at<uchar>(i,begin1))
										&&  (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1))){
									// and JUST enough information on the left is in the same object
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								}  // end1 of if



								else if ( ( (labelRszd.at<uchar>(i, ((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i, begin1)) &&
											(labelRszd.at<uchar>(i,((begin1 - 1))) != labelRszd.at<uchar>(i,begin1)) ) ||
										  ((labelRszd.at<uchar>(i, (begin1 - hLen)) == labelRszd.at<uchar>(i, begin1)) &&
										   (labelRszd.at<uchar>(i,((begin1 - 1))) != labelRszd.at<uchar>(i,begin1)) ) )	{
									// this means there are in fact no pixels available on the left


									if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
										// there is more enough information on the right in the same label to fill the hole

										rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
										for(int rightInfoIndex = 0 ; rightInfoIndex < hLen + 1 ; rightInfoIndex++) {
											if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
												rightInfo = 0;
											}
										}

										if(rightInfo == 1){ // case 8  .. nothing on the left , more ethan enough on the right
											caseNo = 8;
											case8++;

										} // end1 of case 8

										else if (rightInfo == 0) {

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if
										}


									}// end1 of if for when there is more enough information on the right in the same label to fill the hole


									else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
										// there is NOT MORE than enough info on the right to fill the hole

										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

									else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

										if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object

									} // end1 of else if for when the information on the right is not enough


									else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right, but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									} // end1 of else if for when the information on the right is JUST enough



								} // end1 of else if for when there are in fact no pixels available on the left









								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){  // but ALL the information is NOT in the same object


									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label


										if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))
										   && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))){
											// there is more enough information on the right in the same label to fill the hole

											rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
											for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}

											if(rightInfo == 1){ // case 6  .. not enough on the left , more ethan enough on the right
												caseNo = 6;
												case6++;

											} // end1 of case 6

											else if (rightInfo == 0) {

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7... not enough on the left, JUST enough on the right
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if
											}


										}// end1 of if for when there is more enough information on the right in the same label to fill the hole


										else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
											// there is NOT MORE than enough info on the right to fill the hole
											if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1))
											   && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))) { // there is JUST enough info on the right

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object


										}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

										else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

											if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object

										} // end1 of else if for when the information on the right is not enough


										else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



											if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) ) {
												// there is JUST enough info on the right in the same label

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) ||  (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object


										} // end1 of else if for when the information on the right is JUST enough



									} // end1 of if for when there is at least one pixel on the left in the same label




									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label



										if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
											// there is more enough information on the right in the same label to fill the hole

											rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
											for(int rightInfoIndex = 0 ; rightInfoIndex < hLen + 1 ; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}

											if(rightInfo == 1){ // case 8  .. nothing on the left , more ethan enough on the right
												caseNo = 8;
												case8++;

											} // end1 of case 8

											else if (rightInfo == 0) {

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if
											}


										}// end1 of if for when there is more enough information on the right in the same label to fill the hole


										else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
											// there is NOT MORE than enough info on the right to fill the hole

											if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object


										}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

										else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

											if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object

										} // end1 of else if for when the information on the right is not enough


										else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



											if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right, but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object


										} // end1 of else if for when the information on the right is JUST enough





									}  // end1 of if for when there are NO pixels on the left in the same label

								} // end1 of else if for when ALL the information is NOT in the same object







							} // end1 of if for when there is enough information on the left of the hole


							else if((begin1 - hLen) < 0){  //when there is NOT enough information on the left but some

								if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label





									if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))
									   && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))){
										// there is more enough information on the right in the same label to fill the hole

										rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
										for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
											if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
												rightInfo = 0;
											}
										}

										if(rightInfo == 1){ // case 6  .. not enough on the left , mor ethan enough on the right
											caseNo = 6;
											case6++;

										} // end1 of case 6

										else if (rightInfo == 0) {

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 7... not enough on the left, JUST enough on the right
												caseNo = 7;
												case7++;

											} // end1 of case 7
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if
										}


									}// end1 of if for when there is more enough information on the right in the same label to fill the hole


									else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
										// there is NOT MORE than enough info on the right to fill the hole
										if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) ) { // there is JUST enough info on the right

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 7
												caseNo = 7;
												case7++;

											} // end1 of case 7
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
												caseNo = 11;
												case11++;

											} // end1 of case 11

											else{ // case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

											} // end1 of case 3

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 3 ... some on the left, nothing on the right
											caseNo = 3;
											case3++;

											// end1 of case 3

										} // end1 of else if for when there are NO pixels on the right in the same object


									}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

									else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

										if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
												caseNo = 11;
												case11++;

											} // end1 of case 11

											else{ // case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

											} // end1 of case 3

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 3 ... some on the left, nothing on the right
											caseNo = 3;
											case3++;

											// end1 of case 3

										} // end1 of else if for when there are NO pixels on the right in the same object

									} // end1 of else if for when the information on the right is not enough


									else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



										if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1))  && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))) {
											// there is JUST enough info on the right in the same label

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 7
												caseNo = 7;
												case7++;

											} // end1 of case 7
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
												caseNo = 11;
												case11++;

											} // end1 of case 11

											else{ // case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

											} // end1 of case 3

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 3 ... some on the left, nothing on the right
											caseNo = 3;
											case3++;

											// end1 of case 3

										} // end1 of else if for when there are NO pixels on the right in the same object


									} // end1 of else if for when the information on the right is JUST enough



								} //end1 of if for when there is at least one pixel on the left in the same label





								else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label





									if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
										// there is more enough information on the right in the same label to fill the hole

										rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
										for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
											if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
												rightInfo = 0;
											}
										}

										if(rightInfo == 1){ // case 8  .. nothing on the left , mor ethan enough on the right
											caseNo = 8;
											case8++;

										} // end1 of case 8

										else if (rightInfo == 0) {

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if
										}


									}// end1 of if for when there is more enough information on the right in the same label to fill the hole


									else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
										// there is NOT MORE than enough info on the right to fill the hole

										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

									else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

										if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object

									} // end1 of else if for when the information on the right is not enough


									else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



										if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

											rightInfo = 1; // we assume info on the right is available, and then we check.
											for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}
											if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
												caseNo = 9;
												case9++;

											} // end1 of case 9
											else if (rightInfo == 0) {

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

										} // end1 of if for when there is JUST enough info on the right

										else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

											if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right, but not enough
												caseNo = 10;
												case10++;

											} // end1 of case 10

											else{ // case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

											} // end1 of case 12

										} // end1 of last else if

										else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

											// case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

											// end1 of case 12

										} // end1 of else if for when there are NO pixels on the right in the same object


									} // end1 of else if for when the information on the right is JUST enough




								} //end1 of if for when there are NO pixels on the left in the same label

							} // end1 of if for when there is NOT enough information on the left but some




							else if((begin1 - hLen) == 0){  // when the amount of information on the left is EXACTLY the same as the length of the hole




								if((labelRszd.at<uchar>(i,((begin1 - hLen))) == labelRszd.at<uchar>(i,begin1))){
									// and JUST enough information on the left is in the same object
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								}  // end1 of if
								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){  // but ALL the information is NOT in the same object


									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label


										if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))
										   && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))){
											// there is more enough information on the right in the same label to fill the hole

											rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
											for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}

											if(rightInfo == 1){ // case 6  .. not enough on the left , mor ethan enough on the right
												caseNo = 6;
												case6++;

											} // end1 of case 6

											else if (rightInfo == 0) {

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7... not enough on the left, JUST enough on the right
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if
											}


										}// end1 of if for when there is more enough information on the right in the same label to fill the hole


										else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
											// there is NOT MORE than enough info on the right to fill the hole
											if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1))) {
												// there is JUST enough info on the right

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object


										}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

										else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

											if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object

										} // end1 of else if for when the information on the right is not enough


										else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



											if((labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) ) {
												// there is JUST enough info on the right in the same label

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 7
													caseNo = 7;
													case7++;

												} // end1 of case 7
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
														caseNo = 11;
														case11++;

													} // end1 of case 11

													else{ // case 3 ... some on the left, nothing on the right
														caseNo = 3;
														case3++;

													} // end1 of case 3

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 11 .. some on the left, some on the right, neither enough
													caseNo = 11;
													case11++;

												} // end1 of case 11

												else{ // case 3 ... some on the left, nothing on the right
													caseNo = 3;
													case3++;

												} // end1 of case 3

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 3 ... some on the left, nothing on the right
												caseNo = 3;
												case3++;

												// end1 of case 3

											} // end1 of else if for when there are NO pixels on the right in the same object


										} // end1 of else if for when the information on the right is JUST enough



									} // end1 of if for when there is at least one pixel on the left in the same label




									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label



										if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
											// there is more enough information on the right in the same label to fill the hole

											rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
											for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
												if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
													rightInfo = 0;
												}
											}

											if(rightInfo == 1){ // case 8  .. nothing on the left , mor ethan enough on the right
												caseNo = 8;
												case8++;

											} // end1 of case 8

											else if (rightInfo == 0) {

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if
											}


										}// end1 of if for when there is more enough information on the right in the same label to fill the hole


										else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
											// there is NOT MORE than enough info on the right to fill the hole

											if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object


										}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

										else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

											if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object

										} // end1 of else if for when the information on the right is not enough


										else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



											if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

												rightInfo = 1; // we assume info on the right is available, and then we check.
												for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
													if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
														rightInfo = 0;
													}
												}
												if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
													caseNo = 9;
													case9++;

												} // end1 of case 9
												else if (rightInfo == 0) {

													if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
														caseNo = 10;
														case10++;

													} // end1 of case 10

													else{ // case 12 ... nothing on the left, nothing on the right
														caseNo = 12;
														case12++;

													} // end1 of case 12

												} // end1 of last else if

											} // end1 of if for when there is JUST enough info on the right

											else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

												if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right, but not enough
													caseNo = 10;
													case10++;

												} // end1 of case 10

												else{ // case 12 ... nothing on the left, nothing on the right
													caseNo = 12;
													case12++;

												} // end1 of case 12

											} // end1 of last else if

											else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

												// case 12 ... nothing on the left, nothing on the right
												caseNo = 12;
												case12++;

												// end1 of case 12

											} // end1 of else if for when there are NO pixels on the right in the same object


										} // end1 of else if for when the information on the right is JUST enough





									}  // end1 of if for when there are NO pixels on the left in the same label

								} // end1 of else if for when ALL the information is NOT in the same object








							}// end1 of else if for when the amount of information on the left is EXACTLY the same as the length of the hole





						} // end1 of if for when the object has not end1ed but the hole has end1ed






						else if((depthIn.at<uchar>(i,j) == 0) && (labelRszd.at<uchar>(i,j) != labelRszd.at<uchar>(i,j+1))){  // the object has end1ed but the hole has NOT end1ed
							// this means the hole covers the object boundary or continues right up to the boundary for all we care
							// which means NO information on the right is available


							end1 = j;
							hLen = end1 - begin1 + 1;



							if((begin1 - hLen) > 0){  // when there is enough information on the left of the hole

								if(labelRszd.at<uchar>(i,((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i,begin1)){  // and more than enough information is in the same object
									caseNo = 1;
									case1++;

								}  // end1 of if for case 1
								else if((labelRszd.at<uchar>(i,((begin1 - hLen) - 1))) != labelRszd.at<uchar>(i,begin1) && labelRszd.at<uchar>(i,(begin1 - hLen)) == labelRszd.at<uchar>(i,begin1)){
									// and Just enough information on the left is in the same object
									caseNo = 2;
									case2++;

								}  // end1 of if for case 2
								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){  // but ALL the information on the left is NOT in the same object

									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
										caseNo = 3;
										case3++;

									} // end1 of if for case 12
									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
										caseNo = 12;
										case12++;

									}  // end1 of if for case 12

								} // end1 of else if for when ALL the information on the left is NOT in the same object


							} // end1 of if for when there is enough information on the left of the hole

							else if((begin1 - hLen) < 0){  //when there is NOT enough information on the left but some

								if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
									caseNo = 3;
									case3++;

								} //end1 of if for case 3
								else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
									caseNo = 12;
									case12++;
								} //end1 of if for case 12

							} // end1 of if for when there is NOT enough information on the left but some

							else if((begin1 - hLen) == 0){  // when the amount of information on the left is EXACTLY the same as the length of the hole

								if(labelRszd.at<uchar>(i,(begin1 - hLen)) == labelRszd.at<uchar>(i,begin1)){ // there is EXACTLY the same amount of info in the same label on the left
									caseNo = 2;
									case2++;

								} //end1 of if for case 2
								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){ // there is NOT enough information on the left in the same label

									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
										caseNo = 3;
										case3++;

									} //end1 of if for case 3
									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
										caseNo = 12;
										case12++;

									} //end1 of if for case 12

								} //end1 of else if for when there is NOT enough information on the left in the same label

							}// end1 of else if for when the amount of information on the left is EXACTLY the same as the length of the hole





						}  // end1 of else if for when the object has end1ed but the hole has NOT end1ed





						else if((depthIn.at<uchar>(i,j) != 0) && (labelRszd.at<uchar>(i,j) != labelRszd.at<uchar>(i,j+1))){  // the object has end1ed and the hole has end1ed
							// this means BOTH the hole and the object have end1ed




							end1 = j -1;
							hLen = end1 - begin1 + 1;


							if((begin1 - hLen) > 0){  // when there is enough information on the left of the hole

								if((labelRszd.at<uchar>(i,((begin1 - hLen) - 1)) == labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1)) ){  // and more than enough information is in the same object

									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = -1 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 4;
										case4++;

									} // end1 of if for case 4

									else if(checkLeftLabel == 0){

										int checkLeftLabel = 1; // we assume the left info is all in the same label

										for(int chindex = 0 ; chindex < hLen ; chindex++){
											if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
												checkLeftLabel = 0;
											}
										}

										if(checkLeftLabel == 1){
											caseNo = 5;
											case5++;

										} // end1 of if for case 5
										else{
											caseNo = 11;
											case11++;


										} // end1 of case 11

									}


								}  // end1 of if for case 4
								else if((labelRszd.at<uchar>(i,((begin1 - hLen) - 1)) != labelRszd.at<uchar>(i,begin1)) && (labelRszd.at<uchar>(i,((begin1 - 1))) == labelRszd.at<uchar>(i,begin1))
										&& (labelRszd.at<uchar>(i,((begin1 - hLen))) == labelRszd.at<uchar>(i,begin1))){
									// and JUST enough information on the left is in the same object
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								}  // end1 of if


								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){  // but ALL the information is NOT in the same object

									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
										caseNo = 11;
										case11++;

									} // end1 of if for case 11
									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
										caseNo = 10;
										case10++;

									}  // end1 of if for case 10

								} // end1 of else if for when ALL the information is NOT in the same object

							}// end1 of if for when there is enough information on the left of the hole


							else if((begin1 - hLen) < 0){  //when there is NOT enough information on the left but some

								if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
									caseNo = 11;
									case11++;

								} //end1 of if for case 11
								else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
									caseNo = 10;
									case10++;

								} //end1 of if for case 12

							} // end1 of if for when there is NOT enough information on the left but some

							else if((begin1 - hLen) == 0){  // when the amount of information on the left is EXACTLY the same as the length of the hole

								if(labelRszd.at<uchar>(i,(begin1 - hLen)) == labelRszd.at<uchar>(i,begin1)){ // there is EXACTLY the same amount of info in the same label on the label
									int checkLeftLabel = 1; // we assume the left info is all in the same label

									for(int chindex = 0 ; chindex < hLen ; chindex++){
										if((labelRszd.at<uchar>(i, begin1 - hLen + chindex) != labelRszd.at<uchar>(i, begin1)) || (depthIn.at<uchar>(i, begin1 - hLen + chindex) == 0)  ) {
											checkLeftLabel = 0;
										}
									}

									if(checkLeftLabel == 1){
										caseNo = 5;
										case5++;

									} // end1 of if for case 5

									else{
										caseNo = 11;
										case11++;


									} // end1 of case 11

								} //end1 of if
								else if(labelRszd.at<uchar>(i,(begin1 - hLen)) != labelRszd.at<uchar>(i,begin1)){ // there is NOT enough information on the left in the same label

									if(labelRszd.at<uchar>(i,(begin1 - 1)) == labelRszd.at<uchar>(i,begin1)){ // there is at least one pixel on the left in the same label
										caseNo = 11;
										case11++;

									} //end1 of if for case 11
									else if(labelRszd.at<uchar>(i,(begin1 - 1)) != labelRszd.at<uchar>(i,begin1)){ // there are NO pixels on the left in the same label
										caseNo = 10;
										case10++;

									} //end1 of if for case 10

								} //end1 of if for when there is NOT enough information on the left in the same label

							}// end1 of else if for when the amount of information on the left is EXACTLY the same as the length of the hole




						}  // end1 of else if for when both the object has end1ed and the hole has end1ed



					} // end1 of if for when a condition in the "while" was not met and NO case number is assigned yet

				}// end1 of if for when the hole does NOT begin1 in the begin1ning of the line



				else if(begin1 == 0){  //when the hole begin1s at the very begin1ning of the line
					// which means nothing is available on the left

					while((labelRszd.at<uchar>(i,j) == labelRszd.at<uchar>(i,j+1)) && (depthIn.at<uchar>(i,j)) == 0){
						j++;
					} // end1 of while



					if ((j == (depthIn.cols - 1)) || (labelRszd.at<uchar>(i, j) != labelRszd.at<uchar>(i, j + 1)) ){ // the entire line from the very begin1ning of the LINE is made up of one object.
						// this means we stop when the object or the line is over


						if(depthIn.at<uchar>(i,j) == 0){  //when the hole continues until the end1 of the line or label
							end1 = j;
							hLen = begin1 - end1 + 1;

							// this means the entire line or label is covered by a hole and one object
							// nothing on the left and nothing on the right .. case 12
							caseNo = 12;
							case12++;


						}// end1 of if for when the hole continues until the end1 of the line for case 12


						else if(depthIn.at<uchar>(i,j) != 0){  //when the hole does NOT continue to the end1 of the line, but to one pixel before the end1
							end1 = j -1;
							hLen = begin1 - end1 + 1;


							// this means the entire line except for the last pixel is covered by a hole and one object

							// nothing on the left, some but not enough on the right.. case 10
							caseNo = 10;
							case10++;


						}// end1 of else if for when the hole does NOT continue to the end1 of the line, but to one pixel before the end1




					}// end1 of if that started after the while (the entire line from the begin1ning of the line is made up of one object.)




					if(caseNo == 0){ // a condition in the "while" was not met and NO case number is assigned yet

						if((depthIn.at<uchar>(i,j) != 0) && (labelRszd.at<uchar>(i,j) == labelRszd.at<uchar>(i,j+1))){  // the object has not end1ed but the hole has end1ed
							// which means nothing is available on the left, but some stuff may be available on the right



							end1 = j -1;
							hLen = end1 - begin1 + 1;



							if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) == labelRszd.at<uchar>(i,begin1))){
								// there is more enough information on the right in the same label to fill the hole

								rightInfo = 1; // we assume info on the right is available (without any holes), and then we check.
								for(int rightInfoIndex = 0 ; rightInfoIndex < (hLen + 1) ; rightInfoIndex++) {
									if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
										rightInfo = 0;
									}
								}

								if(rightInfo == 1){ // case 8  .. nothing on the left , mor ethan enough on the right
									caseNo = 8;
									case8++;

								} // end1 of case 8

								else if (rightInfo == 0) {

									rightInfo = 1; // we assume info on the right is available, and then we check.
									for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
										if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
											rightInfo = 0;
										}
									}
									if (rightInfo == 1) { // case 9... nothing on the left, JUST enough on the right
										caseNo = 9;
										case9++;

									} // end1 of case 9
									else if (rightInfo == 0) {

										if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but less than enough on the right
											caseNo = 10;
											case10++;

										} // end1 of case 10

										else{ // case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

										} // end1 of case 12

									} // end1 of last else if
								}


							}// end1 of if for when there is more enough information on the right in the same label to fill the hole


							else if(((end1 + hLen + 1) < depthIn.cols) && (labelRszd.at<uchar>(i,(end1 + hLen + 1)) != labelRszd.at<uchar>(i,begin1))){
								// there is NOT MORE than enough info on the right to fill the hole

								if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info in the same label on the right

									rightInfo = 1; // we assume info on the right is available, and then we check.
									for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
										if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
											rightInfo = 0;
										}
									}
									if (rightInfo == 1) { // case 9 .. nothing on the left, JUST enough on the right
										caseNo = 9;
										case9++;

									} // end1 of case 9
									else if (rightInfo == 0) {

										if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
											caseNo = 10;
											case10++;

										} // end1 of case 10

										else{ // case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

										} // end1 of case 12

									} // end1 of last else if

								} // end1 of if for when there is JUST enough info on the right

								else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

									if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some but not enough on the right
										caseNo = 10;
										case10++;

									} // end1 of case 10

									else{ // case 12 ... nothing on the left, nothing on the right
										caseNo = 12;
										case12++;

									} // end1 of case 12

								} // end1 of last else if

								else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

									// case 12 ... nothing on the left, nothing on the right
									caseNo = 12;
									case12++;

									// end1 of case 12

								} // end1 of else if for when there are NO pixels on the right in the same object


							}// end1 of else if for when there is NOT MORE than enough info on the right to fill the hole

							else if(((end1 + hLen + 1) > depthIn.cols) ){ // the information on the right is not enough

								if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

									if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
										caseNo = 10;
										case10++;

									} // end1 of case 10

									else{ // case 12 ... nothing on the left, nothing on the right
										caseNo = 12;
										case12++;

									} // end1 of case 12

								} // end1 of last else if

								else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

									// case 12 ... nothing on the left, nothing on the right
									caseNo = 12;
									case12++;

									// end1 of case 12

								} // end1 of else if for when there are NO pixels on the right in the same object

							} // end1 of else if for when the information on the right is not enough


							else if(((end1 + hLen + 1) == depthIn.cols) ){ // the information on the right is JUST enough



								if(labelRszd.at<uchar>(i,(end1 + hLen)) == labelRszd.at<uchar>(i,begin1)) { // there is JUST enough info on the right in the same label

									rightInfo = 1; // we assume info on the right is available, and then we check.
									for (int rightInfoIndex = 0; rightInfoIndex < hLen; rightInfoIndex++) {
										if ((depthIn.at<uchar>(i, (end1 + 1 + rightInfoIndex)) == 0) || (labelRszd.at<uchar>(i, (end1 + 1 + rightInfoIndex)) != labelRszd.at<uchar>(i, (begin1)) ) ){
											rightInfo = 0;
										}
									}
									if (rightInfo == 1) { // case 9 .. nothing on the left, just enough on the right
										caseNo = 9;
										case9++;

									} // end1 of case 9
									else if (rightInfo == 0) {

										if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 10 .. nothing on the left, some on the right but not enough
											caseNo = 10;
											case10++;

										} // end1 of case 10

										else{ // case 12 ... nothing on the left, nothing on the right
											caseNo = 12;
											case12++;

										} // end1 of case 12

									} // end1 of last else if

								} // end1 of if for when there is JUST enough info on the right

								else if(labelRszd.at<uchar>(i,(end1 + 1)) == labelRszd.at<uchar>(i,begin1)) { // there is at least one pixel on the right but NOT ENOUGH in the same object

									if(depthIn.at<uchar>(i, (end1 + 1)) != 0){ // case 9 .. nothing on the left, some on the right, but not enough
										if(hLen == 1){ // one pixel hole. nothing is available on the left and one pixel available on the right ... case 9
											caseNo = 9;
											case9++;

										}
										else { // nothing on the left and some but not enough on the right .. case 10
											caseNo = 10;
											case10++;

										}
									} // end1 of case 10

									else{ // case 12 ... nothing on the left, nothing on the right
										caseNo = 12;
										case12++;

									} // end1 of case 12

								} // end1 of last else if

								else if(labelRszd.at<uchar>(i,(end1 + 1)) != labelRszd.at<uchar>(i,begin1)) { // there are NO pixels on the right in the same object

									// case 12 ... nothing on the left, nothing on the right
									caseNo = 12;
									case12++;

									// end1 of case 12

								} // end1 of else if for when there are NO pixels on the right in the same object


							} // end1 of else if for when the information on the right is JUST enough




						} // end1 of if for when the object has not end1ed but the hole has end1ed


						else if((depthIn.at<uchar>(i,j) == 0) && (labelRszd.at<uchar>(i,j) != labelRszd.at<uchar>(i,j+1))){  // the object has end1ed but the hole has NOT end1ed
							// this means the hole covers the object boundary or continues right up to the boundary for all we care
							// which means nothing is available on the right and nothing on the left


							end1 = j;
							hLen = end1 - begin1 + 1;

							// nothing on the left and nothing on the right

							caseNo = 12;
							case12++;


						}  // end1 of else if for when the object has end1ed but the hole has NOT end1ed for case 12



						else if((depthIn.at<uchar>(i,j) != 0) && (labelRszd.at<uchar>(i,j) != labelRszd.at<uchar>(i,j+1))){  // the object has end1ed and the hole has end1ed
							// this means BOTH the hole and the object have end1ed
							// which means nothing is available on the left, and one pixel is available on the right


							end1 = j -1;
							hLen = end1 - begin1 + 1;

							if(hLen == 1){ // one pixel hole. nothing is available on the left and one pixel available on the right ... case 9
								caseNo = 9;
								case9++;

							}
							else { // nothing on the left and some but not enough on the right .. case 10
								caseNo = 10;
								case10++;

							}
						}  // end1 of else if for when both the object has end1ed and the hole has end1ed



					} // end1 of if for when a condition in the "while" was not met and NO case number is assigned yet



				} // end1 of else if for when the hole begin1s at the very begin1ning of the line



//endregion






//region CaseFilling

				if (caseNo == 0){
					printf("No case number has been determined. Something is wrong\n");
					exit (EXIT_FAILURE);
				}

				else if (caseNo == 1){ // solving case 1


					cEnd1 = begin1 - 1;
					cBegin1 = begin1 - hLen;

					// checking to see if the information on the left has any holes or not

					int leftAllow = 1; // if this variable is one it means the info on the left is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							leftAllow = 0;
					}

					if(leftAllow == 1) { // this means the info on th left is good so we do it





						for (int tindex = 0; tindex < hLen; tindex++) {

							depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1)) +
																	  depthIn.at<uchar>(i, (cBegin1 + tindex)) -
																	  depthIn.at<uchar>(i, (cBegin1 + tindex - 1));

							if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));


							}
							else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



							}

							else {

								if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

									depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

								}


							}

						} // end1 of for loop
					} // end1 of if for when the info on th left is good so we do it




				}  // end1 of if for solving case 1







				else if (caseNo == 2){ // solving case 2


					cEnd1 = begin1 - 1;
					cBegin1 = begin1 - hLen;


					// checking to see if the information on the left has any holes or not

					int leftAllow = 1; // if this variable is one it means the info on the left is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							leftAllow = 0;
					}

					if(leftAllow == 1) { // this means the info on th left is good so we do it



						if (hLen != 1) {




							for (int tindex = 0; tindex < hLen; tindex++) {

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1)) +
																		  depthIn.at<uchar>(i, (cBegin1 + tindex + 1)) -
																		  depthIn.at<uchar>(i, (cBegin1 + tindex));

								if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {

									depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



								}
								else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

									depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



								}
								else {

									if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

									}


								}

							}  // end1 of for loop
						}

						else
							depthIn.at<uchar>(i, (begin1)) = depthIn.at<uchar>(i, (begin1 - 1));


					}// end1 of if for when the info on th left is good so we do it







				}  // end1 of if for solving case 2




				else if ((caseNo == 4)){ // solving case 4

					cEnd1 = begin1 - 1;
					cBegin1 = begin1 - hLen;



					// checking to see if the information on the left has any holes or not

					int leftAllow = 1; // if this variable is one it means the info on the left is good enough
					for (int tindex = -1 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							leftAllow = 0;
					}


					if(leftAllow == 1) { // this means the info on th left is good so we do it



						if (hLen == 1)
							depthIn.at<uchar>(i, begin1) =
									(depthIn.at<uchar>(i, begin1 - 1) + depthIn.at<uchar>(i, begin1 + 1)) / 2;

						else { // the hole is longer than one pixel

							// first assign the left most pixel in the hole
							depthIn.at<uchar>(i, begin1) = depthIn.at<uchar>(i, cEnd1) + depthIn.at<uchar>(i, cBegin1) - depthIn.at<uchar>(i, (cBegin1 - 1));
							// first assign the left most pixel in the hole


							int solved = 0; // for checking to see if holes have been filled. If it is zero, it means holes have not been solved yet

							// calculating the difference between the two sides of the hole and the two sides of the sample area
							int diffH = (depthIn.at<uchar>(i, (end1 + 1)) - depthIn.at<uchar>(i, (begin1 - 1)));
							int diffC = (depthIn.at<uchar>(i, (cEnd1 + 1)) - depthIn.at<uchar>(i, (cBegin1 - 1)));
							double slope;


							int t1 = depthIn.at<uchar>(i, begin1 - 1);
							int t2 = depthIn.at<uchar>(i, end1 + 1);
							int t3 = depthIn.at<uchar>(i, cBegin1 - 1);
							int t4 = depthIn.at<uchar>(i, cEnd1 + 1);




							int upDownLineSolved = 0; // we first assume that the top cannot be used and then we check

							if( (i != 0) && (labelRszd.at<uchar>(i - 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
								(labelRszd.at<uchar>(i - 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
								(depthIn.at<uchar>(i - 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
								(depthIn.at<uchar>(i - 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the top line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i - 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i - 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex)) || (i == 0)
											) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the top line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i - 1, begin1 + tindex);
									}

								} // end1 of if for when we can use and copy from the top line

							}

							else if( (i != depthIn.rows) && (labelRszd.at<uchar>(i + 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
									 (labelRszd.at<uchar>(i + 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
									 (depthIn.at<uchar>(i + 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
									 (depthIn.at<uchar>(i + 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the bottom line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i + 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i + 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the bottom line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i + 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}





							// calculating the slope and doing the filling

							if(((diffC == 0) && (diffH != 0) && (upDownLineSolved == 0)) || ((diffC == 1) && (diffH != 0) && (upDownLineSolved == 0)) ){
								// the sample area may be flat but the hole is definitely not flat


								int cFlat = 1; // we assume the sample area is flat and then we check
								for (int tindex = 0; tindex < hLen; tindex++){
									if(abs(depthIn.at<uchar>(i,cBegin1 + tindex) - depthIn.at<uchar>(i,cBegin1)) > 1 )
										//if(depthIn.at<uchar>(i,cBegin1 + tindex) != depthIn.at<uchar>(i,cBegin1))


										cFlat = 0;
								}

								if(cFlat == 1){ // the sample area is definitely flat


									if(diffH < 0){
										diffH = diffH * -1;
									}

									int HoldFirstValue = depthIn.at<uchar>(i, begin1 - 1);
									int limit = (int) (hLen/abs(diffH));

									if(limit >= 1) {


										for (int fIndex = 0; fIndex < diffH; fIndex++) {

											for (int tindex = 0; tindex < limit; tindex++) {

												depthIn.at<uchar>(i, (fIndex * limit) + (begin1 + tindex)) = HoldFirstValue;


											} // end1 of inner for

											if (HoldFirstValue < depthIn.at<uchar>(i, end1 + 1))
												HoldFirstValue++;
											else if (HoldFirstValue > depthIn.at<uchar>(i, end1 + 1))
												HoldFirstValue--;

										}  // end1 of outer for

										solved = 1;

									} // end1 of if limit is greater than or equal to one

									else{

										caseNo = 11;
										case11++;



									}

								} // end1 of if for when the sample area is definitely flat


							}// end1 of if for when the sample area may be flat but the hole is definitely not flat


							if ((solved == 0 && times == 3 && (upDownLineSolved == 0)) || (solved == 0 && times == 2 && (upDownLineSolved == 0)) ){



								if (diffH == diffC)
									slope = 1;
								else if (diffH == 0)
									slope = 1 / ((double) diffC);
								else if (diffC == 0)
									slope = (double) diffH;
								else
									slope = (double) diffH / (double) diffC;





								//for testing purpose, we added the following
								int t1 = depthIn.at<uchar>(i, begin1 - 1);
								int t2 = depthIn.at<uchar>(i, end1 + 1);
								int t3 = depthIn.at<uchar>(i, cBegin1 - 1);
								int t4 = depthIn.at<uchar>(i, cEnd1 + 1);



								for (int tindex = -1; tindex < hLen + 1; tindex++) { // moving the stuff from the image matrix to the temp array

									tempArray[cBegin1 + tindex] = (double)(depthIn.at<uchar>(i, cBegin1 + tindex));

								}// end1 of for for moving the stuff from the image matrix to the temp array

								tempArray[begin1] = tempArray[cEnd1] + tempArray[cBegin1] - tempArray[cBegin1 - 1]; //initial value of begin1


								for (int tindex = 0; tindex < hLen; tindex++) { // calculating


									tempArray[begin1 + tindex] = tempArray[cEnd1 + tindex] + ((tempArray[cBegin1 + tindex] - tempArray[cBegin1 + tindex - 1]) * slope);

								}// end1 of for for calculating



								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff back from the temp array to the image matrix


									depthIn.at<uchar>(i, (begin1 + tindex)) = (uchar) tempArray[begin1 + tindex];

									if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



									}
									else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));


									}

									else{
										if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

											depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

										}

									}




								}  // end1 of for loop for moving the stuff back from the temp array to the image matrix




							}  // end1 of if


						} // end1 of else for when the hole is longer than one pixel


					} // end1 of if for when the info on the left is good do we do it





				}  // end1 of if for solving case 4



				else if ((caseNo == 5)){  // solving case 5

					cEnd1 = begin1 - 1;
					cBegin1 = begin1 - hLen;



					// checking to see if the information on the left has any holes or not

					int leftAllow = 1; // if this variable is one it means the info on the left is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							leftAllow = 0;
					}

					if(leftAllow == 1) { // this means the info on th left is good so we do it


						if (hLen == 1)
							depthIn.at<uchar>(i, begin1) =
									(depthIn.at<uchar>(i, begin1 - 1) + depthIn.at<uchar>(i, begin1 + 1)) / 2;

						else { // the hole is longer than one pixel


							// first assign the left most pixel in the hole
							depthIn.at<uchar>(i, begin1) = depthIn.at<uchar>(i, cEnd1) + depthIn.at<uchar>(i, (cBegin1 + 1)) -	depthIn.at<uchar>(i, cBegin1);
							// first assign the left most pixel in the hole

							int solved = 0; // for checking to see if holes have been filled. If it is zero, it means holes have not been solved yet


							// calculating the difference between the two sides of the hole and the two sides of the sample area
							int diffH = (depthIn.at<uchar>(i, (end1 + 1)) - depthIn.at<uchar>(i, (begin1)));
							int diffC = (depthIn.at<uchar>(i, (cEnd1 + 1)) - depthIn.at<uchar>(i, (cBegin1)));
							double slope;



							int upDownLineSolved = 0; // we first assume that the top cannot be used and then we check

							if( (i != 0) && (labelRszd.at<uchar>(i - 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
								(labelRszd.at<uchar>(i - 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
								(depthIn.at<uchar>(i - 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
								(depthIn.at<uchar>(i - 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the top line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i - 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i - 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the top line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i - 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}

							else if( (i != depthIn.rows) && (labelRszd.at<uchar>(i + 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
									 (labelRszd.at<uchar>(i + 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
									 (depthIn.at<uchar>(i + 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
									 (depthIn.at<uchar>(i + 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the bottom line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i + 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i + 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the bottom line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i + 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}









							// calculating the slope and doing the filling

							if((diffC == 0) && (diffH != 0) && (upDownLineSolved == 0)){ // the sample area may be flat but the hole is definitely not flat


								int cFlat = 1; // we assume the sample area is flat and then we check
								for (int tindex = 0; tindex < hLen; tindex++){
									if(depthIn.at<uchar>(i,cBegin1 + tindex) != depthIn.at<uchar>(i,cBegin1))
										cFlat = 0;
								}

								if(cFlat == 1){ // the sample area is definitely flat


									if(diffH < 0){
										diffH = diffH * -1;
									}


									int HoldFirstValue = depthIn.at<uchar>(i, cBegin1);
									int limit = (int) (hLen/diffH);

									for( int fIndex = 0 ; fIndex < diffH ; fIndex++){

										for( int  tindex = 0 ; tindex < limit ; tindex++ ){


											depthIn.at<uchar>(i, (fIndex*limit)+(begin1 + tindex)) = HoldFirstValue;


										} // end1 of inner for

										if(HoldFirstValue < depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue++;
										else if (HoldFirstValue > depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue--;



									}  // end1 of outer for

									solved = 1;

								} // end1 of if for when the sample area is definitely flat


							}// end1 of if for when the sample area may be flat but the hole is definitely not flat


							if (solved == 0 && times == 3 && (upDownLineSolved == 0)){




								if (diffH == diffC)
									slope = 1;
								else if (diffH == 0)
									slope = 1 / ((double) diffC);
								else if (diffC == 0)
									slope = (double) diffH;
								else
									slope = (double) diffH / (double) diffC;







								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff from the image matrix to the temp array

									tempArray[cBegin1 + tindex] = (double)(depthIn.at<uchar>(i, cBegin1 + tindex));

								}// end1 of for for moving the stuff from the image matrix to the temp array

								tempArray[begin1] = tempArray[cEnd1] + tempArray[cBegin1 + 1] - tempArray[cBegin1]; //initial value of begin1


								for (int tindex = 0; tindex < hLen; tindex++) { // calculating

									tempArray[begin1 + tindex] = tempArray[cEnd1 + tindex] + ((tempArray[cBegin1 + tindex + 1] - tempArray[cBegin1 + tindex]) * slope);

								}// end1 of for for calculating



								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff back from the temp array to the image matrix


									depthIn.at<uchar>(i, (begin1 + tindex)) = (uchar) tempArray[begin1 + tindex];

									if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



									}
									else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));


									}

									else{
										if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

											depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

										}
									}




								}  // end1 of for loop for moving the stuff back from the temp array to the image matrix




							}  // end1 of if


						} // end1 of else for when the hole is longer than one pixel


					} // end1 of if for when the info on the left is good do we do it




				}  // end1 of if for solving case 5

				else if ((caseNo == 6)){ // solving case 6

					cBegin1 = end1 + 1;
					cEnd1 = end1 + hLen;

					// checking to see if the information on the right has any holes or not

					int rightAllow = 1; // if this variable is one it means the info on the right is good enough
					for (int tindex = 0 ; tindex < hLen + 1; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							rightAllow = 0;
					}

					if(rightAllow == 1) { // this means the info on the right is good so we do it

						if (hLen == 1)
							depthIn.at<uchar>(i, begin1) =
									(depthIn.at<uchar>(i, begin1 - 1) + depthIn.at<uchar>(i, (begin1 + 1))) / 2;

						else { // the hole is longer than one pixel

							// first assign the right most pixel in the hole
							depthIn.at<uchar>(i, end1) = depthIn.at<uchar>(i, cBegin1) + depthIn.at<uchar>(i, cEnd1) -	depthIn.at<uchar>(i, (cEnd1 + 1));
							// first assign the right most pixel in the hole

							int solved = 0; // for checking to see if holes have been filled. If it is zero, it means holes have not been solved yet



							// calculating the difference between the two sides of the hole and the two sides of the sample area
							int diffH = (depthIn.at<uchar>(i, (end1 + 1)) - depthIn.at<uchar>(i, (begin1 - 1)));
							int diffC = (depthIn.at<uchar>(i, (cEnd1 + 1)) - depthIn.at<uchar>(i, (cBegin1 - 1)));
							double slope;



							int upDownLineSolved = 0; // we first assume that the top cannot be used and then we check

							if( (i != 0) && (labelRszd.at<uchar>(i - 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
								(labelRszd.at<uchar>(i - 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
								(depthIn.at<uchar>(i - 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
								(depthIn.at<uchar>(i - 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the top line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i - 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i - 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the top line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i - 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}

							else if( (i != depthIn.rows) && (labelRszd.at<uchar>(i + 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
									 (labelRszd.at<uchar>(i + 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
									 (depthIn.at<uchar>(i + 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
									 (depthIn.at<uchar>(i + 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the bottom line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i + 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i + 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the bottom line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i + 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}






							// calculating the slope and doing the filling

							if((diffC == 0) && (diffH != 0) && (upDownLineSolved == 0)){ // the sample area may be flat but the hole is definitely not flat


								int cFlat = 1; // we assume the sample area is flat and then we check
								for (int tindex = 0; tindex < hLen; tindex++){
									if(depthIn.at<uchar>(i,cBegin1 + tindex) != depthIn.at<uchar>(i,cBegin1))
										cFlat = 0;
								}

								if(cFlat == 1){ // the sample area is definitely flat


									if(diffH < 0){
										diffH = diffH * -1;
									}


									int HoldFirstValue = depthIn.at<uchar>(i, cBegin1);
									int limit = (int) (hLen/diffH);

									for( int fIndex = 0 ; fIndex < diffH ; fIndex++){

										for( int  tindex = 0 ; tindex < limit ; tindex++ ){

											depthIn.at<uchar>(i, (fIndex*limit)+(begin1 + tindex)) = HoldFirstValue;


										} // end1 of inner for

										if(HoldFirstValue < depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue++;
										else if (HoldFirstValue > depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue--;



									}  // end1 of outer for

									solved = 1;

								} // end1 of if for when the sample area is definitely flat


							}// end1 of if for when the sample area may be flat but the hole is definitely not flat


							if (solved == 0 && times == 3 && (upDownLineSolved == 0)){




								if (diffH == diffC)
									slope = 1;
								else if (diffH == 0)
									slope = 1 / ((double) diffC);
								else if (diffC == 0)
									slope = (double) diffH;
								else
									slope = (double) diffH / (double) diffC;



								for (int tindex = 0; tindex < hLen + 1; tindex++) { // moving the stuff from the image matrix to the temp array

									tempArray[cBegin1 + tindex] = (double)(depthIn.at<uchar>(i, cBegin1 + tindex));

								}// end1 of for for moving the stuff from the image matrix to the temp array

								tempArray[end1] = tempArray[cBegin1] + tempArray[cEnd1] - tempArray[cEnd1 + 1]; //initial value of end1



								for (int tindex = 0; tindex < hLen; tindex++) { // calculating

									tempArray[end1 - tindex] = tempArray[cBegin1 - tindex] + ((tempArray[cEnd1 - tindex] - tempArray[cEnd1 - tindex + 1]) * slope);



								}// end1 of for for calculating



								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff back from the temp array to the image matrix


									depthIn.at<uchar>(i, (begin1 + tindex)) = (uchar) tempArray[begin1 + tindex];

									if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {


										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



									}
									else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



									}

									else{
										if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

											depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

										}
									}



								}  // end1 of for loop for moving the stuff back from the temp array to the image matrix







							}  // end1 of if


						} // end1 of else for when the hole is longer than one pixel


					} // end1 of if for when the info on the left is good do we do it



				}  // end1 of if for solving case 6


				else if ((caseNo == 7)){ // solving case 7

					cBegin1 = end1 + 1;
					cEnd1 = end1 + hLen;


					// checking to see if the information on the right has any holes or not

					int rightAllow = 1; // if this variable is one it means the info on the right is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							rightAllow = 0;
					}

					if(rightAllow == 1) { // this means the info on the right is good so we do it

						if (hLen == 1)
							depthIn.at<uchar>(i, begin1) = (depthIn.at<uchar>(i, begin1 - 1) + depthIn.at<uchar>(i, (begin1 + 1))) / 2;

						else { // the hole is longer than one pixel

							// first assign the right most pixel in the hole
							depthIn.at<uchar>(i, end1) =	depthIn.at<uchar>(i, cBegin1) + depthIn.at<uchar>(i, (cEnd1 - 1)) - depthIn.at<uchar>(i, (cEnd1));
							// first assign the right most pixel in the hole

							int solved = 0; // for checking to see if holes have been filled. If it is zero, it means holes have not been solved yet


							// calculating the difference between the two sides of the hole and the two sides of the sample area
							int diffH = (depthIn.at<uchar>(i, (end1)) - depthIn.at<uchar>(i, (begin1 - 1)));
							int diffC = (depthIn.at<uchar>(i, (cEnd1)) - depthIn.at<uchar>(i, (cBegin1 - 1)));
							double slope;



							int upDownLineSolved = 0; // we first assume that the top cannot be used and then we check

							if( (i != 0) && (labelRszd.at<uchar>(i - 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
								(labelRszd.at<uchar>(i - 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
								(depthIn.at<uchar>(i - 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
								(depthIn.at<uchar>(i - 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the top line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i - 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i - 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the top line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i - 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}

							else if( (i != depthIn.rows) && (labelRszd.at<uchar>(i + 1, begin1 - 1) == labelRszd.at<uchar>(i,begin1 - 1)) &&
									 (labelRszd.at<uchar>(i + 1, end1 + 1) == labelRszd.at<uchar>(i,end1 + 1)) &&
									 (depthIn.at<uchar>(i + 1, begin1 - 1) == depthIn.at<uchar>(i,begin1 - 1)) &&
									 (depthIn.at<uchar>(i + 1, end1 + 1) == depthIn.at<uchar>(i,end1 + 1))){
								// here, we want to copy the stuff from the bottom line if the conditions are right

								upDownLineSolved = 1;
								for (int tindex = 0 ; tindex < hLen ; tindex++) {
									if ((depthIn.at<uchar>(i + 1, begin1 + tindex) == 0) ||
										(labelRszd.at<uchar>(i + 1, begin1 + tindex) !=
										 labelRszd.at<uchar>(i, begin1 + tindex))) {
										upDownLineSolved = 0;

									}
								}

								if(upDownLineSolved == 1){ // we can use and copy from the bottom line

									for (int tindex = 0 ; tindex < hLen ; tindex++) {

										depthIn.at<uchar>(i, begin1 + tindex) = depthIn.at<uchar>(i + 1, begin1 + tindex);
									}


								} // end1 of if for when we can use and copy from the top line

							}






							// calculating the slope and doing the filling


							if((diffC == 0) && (diffH != 0) && (upDownLineSolved == 0)){ // the sample area may be flat but the hole is definitely not flat


								int cFlat = 1; // we assume the sample area is flat and then we check
								for (int tindex = 0; tindex < hLen; tindex++){
									if(depthIn.at<uchar>(i,cBegin1 + tindex) != depthIn.at<uchar>(i,cBegin1))
										cFlat = 0;
								}

								if(cFlat == 1){ // the sample area is definitely flat


									if(diffH < 0){
										diffH = diffH * -1;
									}


									int HoldFirstValue = depthIn.at<uchar>(i, cBegin1);
									int limit = (int) (hLen/diffH);

									for( int fIndex = 0 ; fIndex < diffH ; fIndex++){

										for( int  tindex = 0 ; tindex < limit ; tindex++ ){

											depthIn.at<uchar>(i, (fIndex*limit)+(begin1 + tindex)) = HoldFirstValue;



										} // end1 of inner for

										if(HoldFirstValue < depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue++;
										else if (HoldFirstValue > depthIn.at<uchar>(i, end1 + 1))
											HoldFirstValue--;



									}  // end1 of outer for

									solved = 1;

								} // end1 of if for when the sample area is definitely flat


							}// end1 of if for when the sample area may be flat but the hole is definitely not flat


							if (solved == 0  && times == 3 && (upDownLineSolved == 0)){




								if (diffH == diffC)
									slope = 1;
								else if (diffH == 0)
									slope = 1 / ((double) diffC);
								else if (diffC == 0)
									slope = (double) diffH;
								else
									slope = (double) diffH / (double) diffC;







								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff from the image matrix to the temp array

									tempArray[cBegin1 + tindex] = (double)(depthIn.at<uchar>(i, cBegin1 + tindex));

								}// end1 of for for moving the stuff from the image matrix to the temp array

								tempArray[end1] = tempArray[cBegin1] + tempArray[cEnd1 - 1] - tempArray[cEnd1]; //initial value of end1



								for (int tindex = 0; tindex < hLen; tindex++) { // calculating

									tempArray[end1 - tindex] = tempArray[cBegin1 - tindex] + ((tempArray[cEnd1 - tindex - 1] - tempArray[cEnd1 - tindex]) * slope);



								}// end1 of for for calculating



								for (int tindex = 0; tindex < hLen; tindex++) { // moving the stuff back from the temp array to the image matrix


									depthIn.at<uchar>(i, (begin1 + tindex)) = (uchar) tempArray[begin1 + tindex];

									if (depthIn.at<uchar>(i, (begin1 + tindex)) <= 1) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));


									}
									else if (depthIn.at<uchar>(i, (begin1 + tindex)) >= 255) {

										depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));



									}
									else{
										if( abs(depthIn.at<uchar>(i, (begin1 + tindex)) - depthIn.at<uchar>(i, (begin1 + tindex - 1)) > 10   )){

											depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 + tindex - 1));

										}
									}




								}  // end1 of for loop for moving the stuff back from the temp array to the image matrix






							}  // end1 of if


						} // end1 of else for when the hole is longer than one pixel


					} // end1 of if for when the info on the left is good do we do it





				}  // end1 of if for solving case 7


				else if (caseNo == 8){ // solving case 8

					cBegin1 = end1 + 1;
					cEnd1 = end1 + hLen;

					// checking to see if the information on the right has any holes or not

					int rightAllow = 1; // if this variable is one it means the info on the right is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							rightAllow = 0;
					}

					if(rightAllow == 1) { // this means the info on the right is good so we do it



						if (hLen != 1)
							for (int tindex = 0; tindex < hLen; tindex++) {



								depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1)) +
																		depthIn.at<uchar>(i, (cEnd1 - tindex)) -
																		depthIn.at<uchar>(i, (cEnd1 - tindex + 1));

								if (depthIn.at<uchar>(i, (end1 - tindex)) <= 1) {

									depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));


								}
								else if (depthIn.at<uchar>(i, (end1 - tindex)) >= 255) {

									depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));


								}
								else{
									if( abs(depthIn.at<uchar>(i, (end1 - tindex)) - depthIn.at<uchar>(i, (end1 - tindex + 1)) > 10   )){

										depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));

									}

								}

							} // end1 of for loop

						else
							depthIn.at<uchar>(i, (end1)) = depthIn.at<uchar>(i, (end1 + 1));

					}// end1 of if for when the info in the right is good and we do it







				}  // end1 of if for solving case 8



				else if ((caseNo == 9)){ // solving case 9

					cBegin1 = end1 + 1;
					cEnd1 = end1 + hLen;


					// checking to see if the information on the right has any holes or not

					int rightAllow = 1; // if this variable is one it means the info on the right is good enough
					for (int tindex = 0 ; tindex < hLen ; tindex++){
						if((depthIn.at<uchar>(i,(cBegin1 + tindex)) == 0) || (labelRszd.at<uchar>(i,(cBegin1 + tindex)) != labelRszd.at<uchar>(i,begin1) ))
							rightAllow = 0;
					}

					if(rightAllow == 1) { // this means the info on the right is good so we do it


						if (hLen != 1)
							for (int tindex = 0; tindex < hLen; tindex++) {

								depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (cBegin1 - tindex)) +
																		depthIn.at<uchar>(i, (cEnd1 - tindex - 1)) -
																		depthIn.at<uchar>(i, (cEnd1 - tindex));

								if (depthIn.at<uchar>(i, (end1 - tindex)) <= 1) {

									depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));



								}
								else if (depthIn.at<uchar>(i, (end1 - tindex)) >= 255) {

									depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));



								}
								else{
									if( abs(depthIn.at<uchar>(i, (end1 - tindex)) - depthIn.at<uchar>(i, (end1 - tindex + 1)) > 10   )){

										depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 - tindex + 1));

									}
								}

							} // end1 of for loop
						else
							depthIn.at<uchar>(i, (end1)) = depthIn.at<uchar>(i, (end1 + 1));

					}// end1 of if for wehen the info on the right is good and we do it






				}  // end1 of if for solving case 9


				// for the rest of the cases, we ignore them on the first run. on the second run we flip the image and do the whole thing again to see if we can solve the problem vertically.
				// After the second run is done also, we check to see if any holes are left, and if not all holes are filled in two runs, we perform the desparate solutions in the third run.


				if (times == 3){ // the desperate cases in the third run






					if(caseNo == 11){ //solving case 11




						if(hLen == 1)
							depthIn.at<uchar>(i, begin1) = (depthIn.at<uchar>(i, (begin1 - 1)) + depthIn.at<uchar>(i, (begin1 + 1))) / 2;


						else { // if the hole is longer than one pixel

							// if for checking to see if cubic interpolation is possible
							if (((begin1 - 2) >= 0) && ((end1 + 2) < depthIn.cols) &&
								(labelRszd.at<uchar>(i, (begin1 - 2)) == labelRszd.at<uchar>(i, begin1)) &&
								(labelRszd.at<uchar>(i, (end1 + 2)) == labelRszd.at<uchar>(i, end1))) {


								double y0 = (double) depthIn.at<uchar>(i, (begin1 - 2));
								double y1 = (double) depthIn.at<uchar>(i, (begin1 - 1));
								double y2 = (double) depthIn.at<uchar>(i, (end1 + 1));
								double y3 = (double) depthIn.at<uchar>(i, (end1 + 2));
								double a0 = -0.5 * y0 + 1.5 * y1 - 1.5 * y2 + 0.5 * y3;
								double a1 = y0 - 2.5 * y1 + 2 * y2 - 0.5 * y3;
								double a2 = -0.5 * y0 + 0.5 * y2;
								double a3 = y1;
								double coeff = 1 / ((double) hLen);

								for (double interi = 1; interi <= hLen; interi++) {

									double mu = (coeff * interi) * (coeff * interi);

									depthIn.at<uchar>(i, (begin1 - 1 + interi)) = (a0 * (coeff * interi) * mu) + (a1 * mu) + (a2 * (coeff * interi)) + a3;

								} // end1 of for


							} // end1 of else if for checking to see if cubic interpolation is possible



							else { // we perform the linear interpolation


								// perform the linear interpolation
								double y1 = (double) depthIn.at<uchar>(i, (begin1 - 1));
								double y2 = (double) depthIn.at<uchar>(i, (end1 + 1));
								double coeff = 1 / ((double) hLen);

								for (double interi = 1; interi <= hLen; interi++)
									depthIn.at<uchar>(i, (begin1 - 1 + interi)) = (int) ((y1 * (1 - (interi * coeff))) + (y2 * (interi * coeff)));

							} // end1 of else for when we perform the linear interpolation

						} // end1 of else for when if the hole is longer than one pixel




					} // end1 of if for solving case 11


					else if((caseNo == 3) && (times == 3) ){ //solving case 3

						if((labelRszd.at<uchar>((i - 1), (begin1 - 1)) == labelRszd.at<uchar>(i, (begin1 - 1))) &&
						   (labelRszd.at<uchar>((i - 1), end1) == labelRszd.at<uchar>(i, end1))				  &&
						   (i != 0)){ // checking to see if the upper line is available


							for(int tindex = 0 ; tindex < hLen ; tindex++){

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>((i - 1), (begin1 + tindex)) + depthIn.at<uchar>((i), (begin1 + tindex - 1)) -
																		  depthIn.at<uchar>(i - 1, (begin1 + tindex - 1));

								if(depthIn.at<uchar>(i,(begin1 + tindex)) <= 1){

									depthIn.at<uchar>(i,(begin1 + tindex)) = depthIn.at<uchar>(i,(begin1 + tindex - 1)) ;

								}
								else if(depthIn.at<uchar>(i,(begin1 + tindex)) >= 255){

									depthIn.at<uchar>(i,(begin1 + tindex)) = depthIn.at<uchar>(i,(begin1 + tindex - 1)) ;


								}

							} // end1 of for loop

						} // end1 of if for checking to see if the upper line is available


						else if((labelRszd.at<uchar>(i, (begin1 - 2)) == labelRszd.at<uchar>(i, begin1)) 	&&
								(depthIn.at<uchar>(i, (begin1 - 2)) != 0)){ // checking to see if TWO pixels on the left are available



							for(int tindex = 0 ; tindex < hLen ; tindex++){

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 - 1 + tindex)) + depthIn.at<uchar>(i, (begin1 + tindex - 2)) -
																		  depthIn.at<uchar>(i, (begin1 + tindex - 1));

								if(depthIn.at<uchar>(i,(begin1 + tindex)) <= 1){

									depthIn.at<uchar>(i,(begin1 + tindex)) = depthIn.at<uchar>(i,(begin1 + tindex - 1)) ;


								}
								else if(depthIn.at<uchar>(i,(begin1 + tindex)) >= 255){

									depthIn.at<uchar>(i,(begin1 + tindex)) = depthIn.at<uchar>(i,(begin1 + tindex - 1));


								}

							} // end1 of for loop

						} // end1 of if for checking to see if TWO pixels on the left are available


						else{ // this happens when only one pixel is available on the left


							for(int tindex = 0 ; tindex < hLen ; tindex++)
								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>(i, (begin1 - 1 + tindex));

						} // end1 of else for when only one pixel is available on the left




					} // end1 of else if for solving case 3

					else if((caseNo == 10)){ //solving case 10

						if((i != 0) && (labelRszd.at<uchar>((i - 1), (end1)) == labelRszd.at<uchar>(i, (end1))) &&
						   (labelRszd.at<uchar>((i - 1), begin1) == labelRszd.at<uchar>(i, begin1)) &&
						   (depthIn.at<uchar>((i - 1), (end1 + 1)) == depthIn.at<uchar>((i), (end1 + 1)))) { // checking to see if we can directly copy from the upper line

							// checking to see if the upper line does not have any holes

							int upLine = 1;
							for (int tindex = 0; tindex < hLen; tindex++) {
								if ((depthIn.at<uchar>((i - 1), (begin1 + tindex)) == 0) || (labelRszd.at<uchar>((i - 1), (begin1 + tindex)) != (labelRszd.at<uchar>((i), (begin1 + tindex)))))
									upLine = 0; // which means there are holes in the upper line
							} // end1 of if for checking to se if the upper line does not have any holes

							if (upLine == 1) { // the upper has no holes


								for (int tindex = 0; tindex < hLen; tindex++) {
									depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>((i - 1), (begin1 + tindex));

								}

							}  // end1 of if for when the upper line has no holes

						}// end1 of if for when we can directly copy from the upper line

						else if((i != 0) && (labelRszd.at<uchar>((i - 1), (end1 + 1)) == labelRszd.at<uchar>(i, (end1 + 1))) &&
								(labelRszd.at<uchar>((i - 1), begin1) == labelRszd.at<uchar>(i, begin1)) &&(labelRszd.at<uchar>((i - 1), (end1)) == labelRszd.at<uchar>((i - 1), (end1 + 1)))
								){

							// checking to see if the upper line is available

							int upLine = 1;
							for(int tindex = 0 ; tindex < hLen ; tindex++){
								if((depthIn.at<uchar>((i - 1), (begin1 + tindex)) == 0) || (labelRszd.at<uchar>((i - 1), (begin1 + tindex)) != (labelRszd.at<uchar>((i), (begin1 + tindex)))))
									upLine = 0; // which means there are holes in the upper line
							}

							if(upLine == 1) {



								for (int tindex = 0; tindex < hLen; tindex++) {

									depthIn.at<uchar>(i, (end1 - tindex)) =
											depthIn.at<uchar>((i - 1), (end1 - tindex)) +
											depthIn.at<uchar>((i), (end1 - tindex + 1)) -
											depthIn.at<uchar>(i - 1 , (end1 - tindex + 1));

									if(depthIn.at<uchar>(i,(end1 - tindex)) <= 1){

										depthIn.at<uchar>(i,(end1 - tindex)) = depthIn.at<uchar>(i,(end1 - tindex + 1));


									}
									else if(depthIn.at<uchar>(i,(end1 - tindex)) >= 255){

										depthIn.at<uchar>(i,(end1 - tindex)) = depthIn.at<uchar>(i,(end1 - tindex + 1));


									}

								} // end1 of for loop

							}
						} // end1 of if for checking to see if the upper line is available


						else if(((end1 + 2) < depthIn.cols) && (labelRszd.at<uchar>(i, (end1 + 2)) == labelRszd.at<uchar>(i, end1)) 	&&
								(depthIn.at<uchar>(i, (end1 + 2)) != 0)){ // checking to see if TWO pixels on the right are available


							for(int tindex = 0 ; tindex < hLen ; tindex++){

								depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1  + 1 - tindex)) + depthIn.at<uchar>(i, (end1 - tindex + 2)) -
																		depthIn.at<uchar>(i, (begin1 - tindex + 1));

								if(depthIn.at<uchar>(i,(end1 - tindex)) <= 1){

									depthIn.at<uchar>(i,(end1 - tindex)) = depthIn.at<uchar>(i,(end1 - tindex + 1));

								}
								else if(depthIn.at<uchar>(i,(end1 - tindex)) >= 255){

									depthIn.at<uchar>(i,(end1 - tindex)) = depthIn.at<uchar>(i,(end1 - tindex + 1));


								}

							} // end1 of for loop

						} // end1 of if for checking to see if TWO pixels on the right are available


						else{ // this happens when only one pixel is available on the right


							for(int tindex = 0 ; tindex < hLen ; tindex++)
								depthIn.at<uchar>(i, (end1 - tindex)) = depthIn.at<uchar>(i, (end1 + 1 - tindex));

						} // end1 of else for when only one pixel is available on the right


					} // end1 of else if for solving case 10

					else if((caseNo == 12)){ //solving case 12

						if((labelRszd.at<uchar>((i - 1), end1) == labelRszd.at<uchar>(i, end1))		&&
						   (labelRszd.at<uchar>((i - 1), begin1) == labelRszd.at<uchar>(i, begin1))	&&
						   (i != 0)){ // checking to see if the upper line is available

							for(int tindex = 0 ; tindex < hLen ; tindex++){

								depthIn.at<uchar>(i, (begin1 + tindex)) = depthIn.at<uchar>((i - 1), (begin1 + tindex));

							} // end1 of for loop

						} // end1 of if for checking to see if the upper line is available


					} // end1 of else if for solving case 12



				} // end1 of if for the desperate cases in the third run



//endregion




				caseNo = 0;




			} // end1 of if for when we have reached a hole in a line

		}// end1 of the main for loop over the columns
	}// end1 of the main for loop over the rows


	return depthIn;  // end1 of program
}// end1 of main
