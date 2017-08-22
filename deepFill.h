/* 
 DepthComp : efficient depth filling appraoch built on segmentation

 Requirements: depth image / segmented image
					
 Usage: ./holeFilling <path_to_depth_image> <path_to_segmented_image>

 Author : Amir Atapour-Abarghouei, amir.atapour-abarghouei@durham.ac.uk

 Copyright (c) 2017 Engineering and Computing Sciences, Durham University
 License : GPL - http://www.gnu.org/licenses/gpl.html

***********************************************************************************
*/

#ifndef deepFill_hpp
#define deepFill_hpp

#include <iostream>
#include <stdlib.h>
#include "opencv2/photo/photo.hpp"
#include <opencv/cv.h>
#include <cv.h>
#include <opencv/highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<fstream>
#include <chrono>

using  ns = std::chrono::microseconds;
using get_time = std::chrono::steady_clock ;


using namespace cv;
using namespace std;


class DepthFill{
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


    int holesExist;// this is used to check if holes still exist after each run
    int times; // this shows how many times we have run this over the image


    DepthFill();
    ~DepthFill();
    Mat preProcess(Mat, const Mat&);
    Mat identFillHoles(Mat depthIn, const Mat& labelIn);
    void writeImg(const String& imgName, const String& preFix, const Mat& mat);


};

#endif
