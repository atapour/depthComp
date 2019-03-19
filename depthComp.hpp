// *****************************************************************************
/*
   DepthComp : efficient depth filling appraoch built on segmentation

   Requirements: depth image / segmented image

   Implementation of DepthComp - Atapour-Abarghouei / Breckon, Proc. BMVC 2017.

   Author : Amir Atapour-Abarghouei, amir.atapour-abarghouei@durham.ac.uk

   Copyright (c) 2017 Engineering and Computing Sciences, Durham University
   License : GPL - http://www.gnu.org/licenses/gpl.html

 ********************************************************************************
 */

#ifndef depthComp_hpp
#define depthComp_hpp

// *****************************************************************************

#include <iostream>
#include <stdlib.h>
#include <opencv2/photo/photo.hpp>
// #include <opencv2/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <math.h>

#include <fstream>
#include <chrono>

using  ns = std::chrono::microseconds;
using get_time = std::chrono::steady_clock;

using namespace cv;
using namespace std;

// *****************************************************************************

class DepthComp {
private:

int holeFound;

//counting case numbers

int case1;
int case2;
int case3;
int case4;
int case5;
int case6;
int case7;
int case8;
int case9;
int case10;
int case11;
int case12;


int holesExist;     // used to check if holes still exist after each run
int times;          // shows how many times we have iterated over the image

// worker function to perform each pass of the completion process

Mat identFillHolesPass(Mat depthIn, const Mat& labelIn);

public:

DepthComp();
~DepthComp();

// perform pre-processing on the depth image prior to completion
// depthIn - input depth (N.B. modified inplace)
// labelIn - input segmented label image
// depthNormalize - perform depth normalization (stretching) for visualization
// return value - pre-processed depth image

Mat preProcess(Mat depthIn, const Mat& labelIn, bool depthNormalize=true);

// perform post-processing on the depth image after completion if required
// depthIn - input depth (N.B. modified inplace)
// labelIn - input segmented label image
// inpaintLeftOvers - perform post filling of non-parametrically unresolvable cases if needed
// return value - post-processed depth image

Mat postProcess(Mat depthIn, const Mat& labelIn, bool inpaintLeftOvers=true);

// perform the identification and filling of depth holes
// depthIn - input depth (N.B. modified inplace)
// labelIn - input segmented label image (N.B. modified inplace)
// logStats - log the statistics of the completion process to file data.txt
// return value - completed output depth image

Mat identFillHoles(Mat depthIn, const Mat labelIn, bool logStats=false);

};

#endif

// *****************************************************************************
