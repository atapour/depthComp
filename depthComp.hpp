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
#include <opencv/cv.h>
#include <opencv/highgui.h>
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

public:
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

DepthComp();
~DepthComp();

// perform pre-processing on the depth image prior to completion
// depthIn - input depth
// labelIn - input segmented label image
// depthNormalize - perform depth normalization (stretching) for visualization
// return value - pre-processed depth image

Mat preProcess(Mat depthIn, const Mat& labelIn, bool depthNormalize=true);

// perform the identification and filling of depth holes
// depthIn - input depth
// labelIn - input segmented label image
// return value - completed output depth image

Mat identFillHoles(Mat depthIn, const Mat& labelIn);

};

#endif

// *****************************************************************************
