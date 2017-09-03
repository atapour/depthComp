# DepthComp: Real-time Depth Image Completion Based on Prior Semantic Scene Segmentation

Requires [OpenCV](http://www.opencv.org), C++, and CMake.

![](https://i.imgur.com/96QX8o3.jpg)

## Method:

_"We address plausible hole filling in depth images in a computationally lightweight methodology that leverages recent advances in semantic scene segmentation. Firstly, we perform such segmentation over a co-registered color image, commonly available from stereo depth sources, and non-parametrically fill missing depth values based on a multipass basis within each semantically labeled scene object. Within this formulation, we identify a bounded set of explicit completion cases in a grammar inspired context that can be performed effectively and efficiently to provide highly plausible localized depth continuity via a case-specific non-parametric completion approach. Results demonstrate that this approach has complexity and efficiency comparable to conventional interpolation techniques but with accuracy analogous to contemporary depth filling approaches. Furthermore, we show it to be capable of fine depth relief completion beyond that of both contemporary approaches in the field and computationally comparable interpolation strategies."_

[[Atapour-Abarghouei, Breckon, Proc. BMVC, 2017](http://breckon.eu/toby/publications/papers/abarghouei17depthcomp.pdf)]

---

## Reference implementation:
Produces a completed depth (disparity) map output image based on a depth (disparity) input with holes and a corresponding segmented label image.
* The input depth map will be preprocessed beforehand and small speckles of invalid depth will be removed (see code for settings).
* The input segmented image (produced by any method) is required as an input in addition to the depth - segmented examples can be generated from RGB images via [SegNet](http://mi.eng.cam.ac.uk/projects/segnet/) (Kendel at al, 2015).
* The quality of the output depends on the quality of the segmented image.
* The code can be run using the test harness: ./depthComp <path_to_depth_image> <path_to_segmented_image>
* Example images are provided in the Examples sub-directory.
* A generic interface C++ object is provided within the depthComp.{cpp|.hpp} files.

```

DepthComp (c) Amir Atapour-Abarghouei, 2017
GPL - http://www.gnu.org/licenses/gpl.html

Compilation and Run Instructions for the Example Code:

$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./depthComp ./../Examples/city-disp.png ./../Examples/city-label.png)
```

The output results are written in the 'Examples' directory (with depth output using the same quantization range as the input):
* the file with the suffix "-PROCESSED" is the despeckled depth image.
* the file with the suffix "-FILLED" is the filled depth image.
* The file "data.txt" contains information about run-time and number of cases.

---

## Example:
[![Video Example](https://i.imgur.com/ZlOPibl.jpg)](https://vimeo.com/224513553 "Video Example - Click to Play")

Video Example - click image above to play.

---

## Reference:

[DepthComp: Real-time Depth Image Completion Based on  Semantic Scene Segmentation](http://breckon.eu/toby/publications/papers/abarghouei17depthcomp.pdf)
(A. Atapour-Abarghouei, T.P. Breckon), In Proc. British Machine Vision Conference, 2017. [[pdf](http://breckon.eu/toby/publications/papers/abarghouei17depthcomp.pdf)] [[demo](https://vimeo.com/224513553)]

```
@InProceedings{abarghouei17depthcomp,
  author = 	 {Atapour-Abarghouei, A. and Breckon, T.P.},
  title = 	 {DepthComp: Real-time Depth Image Completion Based on Prior Semantic Scene Segmentation},
  booktitle = 	 {Proc. British Machine Vision Conference},
  pages = 	 {208.1-208.13},
  year = 	 {2017},
  month = 	 {September},
  publisher =    {BMVA},
  keywords =     {depth filling, RGB-D, surface relief, hole filling, surface completion, 3D texture, depth completion, depth map, disparity hole filling},
  url = 	 {http://community.dur.ac.uk/toby.breckon/publications/papers/abarghouei17depthcomp.pdf}
}
```
---
