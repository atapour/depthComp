# DepthComp: Real-time Depth Image Completion Based on Prior Semantic Scene Segmentation


Requires [OpenCV](http://www.opencv.org), C++, and CMake.


The depth map will be preprocessed beforhands and small speckles of invalid depth will be removed.

A segmented image produced by any method is required.

The quality of the output depends on the quality of the segmented image.

The quality of the output depends on the quality of the segmented image.

The code can be run using ./holeFilling <path_to_depth_image> <path_to_segmented_image>


```

DepthComp (c) Amir Atapour-Abarghouei, 2017
GPL - http://www.gnu.org/licenses/gpl.html

Compilation and Run Instructions for the Example Code:

$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./holeFilling ./../Examples/plastic-disp-hole.png ./../Examples/plastic-label.png

```



The results are written in the 'Examples' directory:

  -the file with the suffix "-PROCESSED" is the despeckled depth image.
	
  -the file with the suffix "-FILLED" is the filled depth image.

The file "data.txt" contains information about run-time and number of cases.

Developed for reasearch in:

[DepthComp: Real-time Depth Image Completion Based on Prior Semantic Scene Segmentation](http://breckon.eu/toby/publications/papers/abarghouei17depthcomp.pdf)
(A. Atapour-Abarghouei, T.P. Breckon), In Proc. British Machine Vision Conference, 2017. [[pdf](http://breckon.eu/toby/publications/papers/abarghouei17depthcomp.pdf)] [[demo](https://vimeo.com/224513553)]

