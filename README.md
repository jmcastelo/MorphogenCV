# MorphogenCV

An interactive tool to simulate videofeedback, a phenomenon that consists of connecting a videocamera to a television in such a way that the television shows whatever the camera is capturing, and pointing the camera to the television. Thus a feedback loop is established. Out of this arrangement intriguing shapes and patterns of light emerge that evolve in time and space. In them chaos and order can coexist, sometimes one imposing its forms on the other. The use of MorphogenCV is to generate these patterns or forms, which may seem to be endowed with life.

## Screenshots

<img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_01.png"/> <img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_02.png"/> <img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_03.png"/> <img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_04.png"/> <img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_05.png"/> <img width="128" height="128" src="https://raw.githubusercontent.com/jmcastelo/MorphogenCV/master/screenshots/screenshot_06.png"/>

## Pipelines

Instead of a single feedback loop, MorphogenCV is able to process images in several parallel loops, called pipelines. A pipeline is simply a sequence of operations that are performed on an image, in order. The output image of an operation is the input image of the next one. Once the processing of the operations of the pipelines is made, the resulting images are blended into a global output image. Finally, the global output image is fed back to each pipeline. This establishes parallel loops that iteratively process the image.

## Operations

A list of supported operations follows:

* Blur: bilateral filter
* Blur: Gaussian
* Blur: homogeneous
* Blur: median
* Canny edge detection
* Contrast/brightness adjustment
* Deblur filter
* Equalize histogram
* Filter with 3x3 kernel
* Gamma correction
* Laplacian
* Mix channels
* Morphology operations
    * Erode
    * Dilate
    * Opening
    * Closing
    * Gradient
    * Top hat
    * Black hat
* Radial remap
* Rotation/scaling
* Sharpen
* Shift hue

## Description of the user interface

The user interface is divided into two separate windows: the control window and the global output image window. The first one presents three tabs: general, pipelines and plots. These tabs contain the controls to change the options and parameters of the system, and take actions on it The second one displays the resulting global output image. This window can be zoomed in and out and its controls are quite self-descriptive. 

### General tab

#### Seed image

In order for the pipelines to start processing, an initial seed image must be supplied. In MorphogenCV, this is an image with random colored or grayscale pixels. The seed image can be fed to the pipelines when a button is pressed. This is useful whenever the global output image gets all black while fiddling with the parameters.

#### Configurations

It is possible to save configurations of the pipelines, with all their operations and parameters, to XML files with the extension .morph that can be loaded later. Several configurations are provided so that you can start exploring from them.

#### Main

The pipelines processing can be paused and resumed, and the time interval between succesive iterations can be chosen (in milliseconds). The images MorphogenCV employs are square and their size can be determined.

#### Video capture

Videos of the evolution of the patterns can be captured in AVI format. The number of frames per second can be chosen. MPEG-1 codec is used. On Windows, the `opencv_videoio_ffmpeg430_64.dll` library is needed for this utility to work.

### Pipelines tab

#### Pipelines

Pipelines can be created, selected and destroyed. Their blend factors can be chosen and their sum is always guaranteed to be 1. They represent the relative contribution of each pipeline output image to the blended global output image. All the blend factors can be made equal if the user wants to. A pipeline is created with a zero blend factor. When a pipeline is destroyed, the blend factors of the remaining pipelines are recomputed so that their sum is 1.

#### Pipeline operations

Operations can be inserted and removed from a pipeline, and their order inside their containing pipeline can be changed by drag and drop) Note that the order of operations usually determines the form and dynamics of the resulting global output image.

#### Parameters

Generally, each operation has a set of adjustable parameters. Fiddling with them modifies the result of the operation on the image and as a consequence, determines the form and dynamics of the global output image. Whenever a numeric parameter is edited, the user must press the return key to make the change effective. There is also a slider for adjusting continous parameters, whose minimum an maximum values can be chosen. Good luck looking for a nice pattern!

### Plots tab

MorphogenCV provides the user with plots of several computed quantities which can be shown or hidden by the press of a button.

#### Full image plots

Plots involving the whole image include:

* Histogram of the BGR color channels.
* Discrete Fourier transform.
* Color intensity plot: evolution in time of the BGR color channels.
* Color-space plot: 2D scatter plot with selectable axes corresponding to the BGR color channels. If the image size is large, this plot can take much time to draw, so use with care.

#### Single pixel plots

Plots involving a single pixel (selectable on the image window) include:

* Color intensity plot: evolution in time of the selected pixel's BGR color channels.
* Color-space plot: 2D curve plot with selectable axes corresponding to the selected pixel's BGR color channels.

## Description of operations

### Blur: bilateral filter

Can reduce unwanted noise very well while keeping edges fairly sharp. However, it is very slow compared to most filters. [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#ga9d7064d478c95d60003cf839430737ed)

#### Parameters

* **Diameter**: Diameter of each pixel neighborhood that is used during filtering. If it is non-positive, it is computed from "Sigma space".
* **Sigma color**: Filter sigma in the color space. A larger value of the parameter means that farther colors within the pixel neighborhood (see "Sigma space") will be mixed together, resulting in larger areas of semi-equal color.
* **Sigma space**: Filter sigma in the coordinate space. A larger value of the parameter means that farther pixels will influence each other as long as their colors are close enough (see "Sigma color" ). When "Diameter" > 0, it specifies the neighborhood size regardless of "Sigma space". Otherwise, "Diameter" is proportional to "Sigma space".

### Blur: Gaussian

Blurs an image using a Gaussian filter. [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#gaabe8c836e97159a9193fb0b11ac52cf1)

#### Parameters

* **Kernel size**: Gaussian kernel size. Must be positive and odd. Or it can be zero and then it is computed from "Sigma".
* **Sigma**: Gaussian kernel standard deviation in both X and Y directions.

### Blur: homogeneous

Blurs an image using the normalized box filter, using a kernel all of whose elements are equal and normalized. [OpenCV function documentation](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#ga8c45db9afe636703801b0b2e440fce37)

#### Parameter

* **Kernel size**: Blurring kernel size. Must be positive and odd.

### Blur: median

Blurs an image using the median filter. [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#ga564869aa33e58769b4469101aac458f9)

#### Parameter

* **Kernel size**: Aperture linear size. Must be odd and greater than one.

### Canny edge detection

Finds edges in the input image and marks them in the output map edges using the Canny algorithm. The smallest value between "Threshold 1" and "Threshold 2" is used for edge linking. The largest value is used to find initial segments of strong edges. For more information see this [OpenCV tutorial](https://docs.opencv.org/master/da/d5c/tutorial_canny_detector.html). [OpenCV function documentation.](https://docs.opencv.org/master/dd/d1a/group__imgproc__feature.html#ga04723e007ed888ddf11d9ba04e2232de)

#### Parameters

* **Threshold 1**: First threshold for the hysteresis procedure.
* **Threshold 2**: Second threshold for the hysteresis procedure.
* **Aperture size**: Aperture size for the Sobel operator.
* **L2 gradient**: A flag, indicating whether a more accurate norm should be used to calculate the image gradient magnitude.

### Contrast/brightness adjustment

Multiples each image pixel by a "Gain" and adds "Bias" to it. Contrast is proportional to "Gain" and brightness to "Bias". For more information see this [OpenCV tutorial](https://docs.opencv.org/3.4/d3/dc1/tutorial_basic_linear_transform.html). [OpenCV function documentation.](https://docs.opencv.org/master/d3/d63/classcv_1_1Mat.html#adf88c60c5b4980e05bb556080916978b)

#### Parameters

* **Gain**: Scale factor (contrast).
* **Bias**: Added to the scaled values (brightness).

### Deblur filter

Restores a blurred image by Wiener filter. For technical information see this [OpenCV tutorial](https://docs.opencv.org/master/de/d3c/tutorial_out_of_focus_deblur_filter.html).

#### Parameters

* **Radius**: Radius of the point spread function.
* **Signal to noise ratio**: of the input image.

### Equalize histogram

Equalizes the histogram of the input image, in order to stretch out the intensity range. The algorithm normalizes the brightness and increases the contrast of the image. This operation has no parameter. For more information see this [OpenCV tutorial](https://docs.opencv.org/master/d4/d1b/tutorial_histogram_equalization.html). [OpenCV function documentation.](https://docs.opencv.org/master/d6/dc7/group__imgproc__hist.html#ga7e54091f0c937d49bf84152a16f76d6e)

### Filter with 3x3 kernel

Convolves an image with a given 3x3 kernel. For more information see this [OpenCV tutorial](https://docs.opencv.org/master/d4/dbd/tutorial_filter_2d.html). [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#ga27c049795ce870216ddfb366086b5a04)

#### Parameter

* **Kernel**: A 3x3 matrix with real values.

### Gamma correction

Corrects the brightness of an image by using a non linear transformation between the input values and the mapped output values. As this relation is non linear, the effect will not be the same for all the pixels and will depend to their original value. When "Gamma" < 1 the original dark regions will be brighter and the histogram will be shifted to the right whereas it will be the opposite with "Gamma" > 1. For more information see this [OpenCV tutorial](https://docs.opencv.org/3.4/d3/dc1/tutorial_basic_linear_transform.html).

#### Parameter

* **Gamma**: exponent of non linear mapping.

### Laplacian

Calculates the Laplacian of an image. Works best for silhouette definition if applied after a blur operation. For more information see this [OpenCV tutorial](https://docs.opencv.org/master/d5/db5/tutorial_laplace_operator.html). [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#gad78703e4c8fe703d479c1860d76429e6)

#### Parameter

* **Kernel size**: Aperture size used to compute the second-derivative filters. Must be positive and odd. When "Kernel size" = 1, the Laplacian is computed by filtering the image with a typical 3x3 aperture.

### Mix channels

Copies specified color channel (blue, green or red) from input image to specified channel (blue, green or red) of output image. [OpenCV function documentation.](https://docs.opencv.org/master/d2/de8/group__core__array.html#ga51d768c270a1cdd3497255017c4504be)

#### Parameters

* **Blue**: Destination channel for blue channel of input image.
* **Green**: Destination channel for green channel of input image.
* **Red**: Destination channel for red channel of input image.

### Morphology operations

A set of operations that process an image based on shapes. Morphological operations apply a structuring element to an input image and generate an output image. They use erosion and dilation as basic operations. For further information see these OpenCV tutorials on [erosion and dilation](https://docs.opencv.org/master/db/df6/tutorial_erosion_dilatation.html) and [several morphology operations](https://docs.opencv.org/master/d3/dbe/tutorial_opening_closing_hats.html). [OpenCV function documentation.](https://docs.opencv.org/master/d4/d86/group__imgproc__filter.html#ga67493776e3ad1a3df63883829375201f)

#### Parameters

* **Type**: "Erode", "Dilate", "Opening", "Closing", "Gradient", "Top hat" or "Black hat"
* **Shape**: Shape of the structuring element. Can be "Rectangle", "Cross" or "Ellipse".
* **Kernel size**: Size of the structuring element. Must be positive and odd.
* **Iterations**: Number of times erosion and dilation are applied.

### Radial remap

Remapping is the process of taking pixels from one place in an image and locating them in another position in a new image. MorphogenCV's possible remappings are smooth and radial, they depend on the distance of the pixel to the center of the image and are given by two types of function: linear or cosine. These functions can be increasing or decreasing, starting from the center of the image and its maximum value is given by the parameter "Amplitude". For more information see this [OpenCV tutorial](https://docs.opencv.org/master/d1/da0/tutorial_remap.html). [OpenCV function documentation.](https://docs.opencv.org/master/da/d54/group__imgproc__transform.html#gab75ef31ce5cdfb5c44b6da5f3b908ea4)

#### Parameters

* **Function**: "Linear inc.", "Linear dec.", "Cosine inc." or "Cosine dec."
* **Amplitude**: of the radial function. Can be positive or negative, indicating a mapping away or towards the center of the image.
* **Interpolation**: "Nearest neighbor", "Bilinear", "Bicubic", "Area" or "Lanczos 8x8"

### Rotation/scaling

Rotates an image by a given "Angle" and scales it by a given "Scale". Selected interpolation is applied.

#### Parameters

* **Angle**: In degrees.
* **Scale**: Must be positive.
* **Interpolation**: "Nearest neighbor", "Bilinear", "Bicubic", "Area" or "Lanczos 8x8"

### Sharpen

Sharpens an image with a low contrast mask. First it blurs the image with a Gaussian filter.

#### Parameters

* **Sigma**: Gaussian kernel standard deviation in both X and Y directions.
* **Threshold**: lowContrastMask = abs(src - blurred) < threshold
* **Amount**: How much to sharpen.

### Shift hue

Shifts the hue of an image by a given "Delta". First the image is converted from BGR to HSV, then its hue is shifted. In OpenCV, hue ranges from 0 to 179 (a total of 180 values) and each hue number represents a color of the spectrum.

#### Parameter

* **Delta**: hue increment or decrement. It ranges from -180 to 180.

## Compilation

Build dependencies: `Qt (5.14.2)`, `OpenCV (4.3.0)` and `QCustomPlot (2.0.1)`, this last library is included in the repository. Please edit the Qt project file to suit your environment requirements. To compile MorphogenCV execute `qmake` followed by `make` on Linux or `mingw32-make` on Windows with MinGW. On Windows you may need to copy the appropriate Qt and OpenCV DLLs to the directory where the binary is located in order to run it.

## License

This software is open source and available under the [GPLv3 License](LICENSE)
