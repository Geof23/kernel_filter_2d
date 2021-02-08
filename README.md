## Filtering Images with 2d Kernels

[Introduction](#introduction)

[Prerequisites](#prerequisites)

[Building](#building)

[Running the tool](#how-to-run-it)

[How it works](#how-it-works)

[Summary](#summary)

### <a name="introduction">Introduction</a>

This complete example (written in `c++20`)allows one to explore the application of 2d kernels
for image processing.  Complete details can be found in the [Wikipedia article:
Kernel_(image_processing)](https://en.wikipedia.org/wiki/Kernel_(image_processing)).
This is very self-contained and you can build it in minutes from a
`Windows 10` system (even with little programming experience).

The program built with this project will read a target image (tested
with several `.jpg` and `.png` files; it has support also for `.tiff` and
`.gif`), apply the filters, and write an image for each filter.
It currently contains the kernels from the Wiki article.  Other ones are simple to
define, as they consist of a square matrix (defined as a list of values)
and a `global coefficient`.

Simply put, filter kernels apply a function to each pixel, which is some combination
of it and neighbors of a given distance.  The operation performed is a
`matrix convolution` between the kernel matrix and the submatrix of pixels
centered at the target pixel, of the same dimensions as the kernel.

The general form is this:

![MatrixConvolutionMath](https://wikimedia.org/api/rest_v1/media/math/render/svg/0f0005a6520eaab36f9b30520b640663d669d891)

Where the matrix `x[mn]` is a submatrix of the target image, n = m, and the target pixel
is at [m/2][n/2].  The matrix `y[mn]` is the kernel to be applied.

Sample results:

| kernel name         | Image                                                                                   |
|---------------------|-----------------------------------------------------------------------------------------|
| Identity            | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Edge D              | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Edge L              | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Edge C              | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Sharpen             | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Box blur            | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Gaussian blur 3x3   | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Gaussian blur 5x5   | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |
| Unsharp masking 5x5 | !(ident)[https://github.com/Geof23/kernel_filter_2d/blob/main/doc/pinball_Identity.jpg] |

### <a name="prerequisites">Prerequisites</a>

The c++ code and dependent submodule are portable code and should work on a number of
platforms, but it is only configured to run on Linux at the moment, and tested
on `Windows 10 WSL` (`Ubuntu 20.04.1 LTS`).  [As a side note, the addition of a
top-level CMakeLists.txt would enable automatic building on many more platforms).
It is very easy to set up (all the system dependencies are available with `apt`).

* Install [Win 10 WSL Ubuntu 20](https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab)

* Install the system depedencies
  * git
  * CMake
  * GNU g++ 10
  * image developer libraries (for jpeg, gif, tiff and png)

To install these, you may use the `apt` package management system with one
convenient command, ran from the WSL Ubuntu bash shell:

```
sudo apt-get install git cmake g++-10 libjpeg-dev libgif-dev libtiff-dev libpng-dev
```

Answer `y` when prompted, and soon everything will be configured.


### <a name="building">Building the project</a>

In order to get this project going, once you have the environment and the system dependencies installed,
you must clone the project, configure the submodule, and perform the actual build.

The image processing library that this project depends on, `sail`, is built with `CMake`.  The core project,
at this time, is built with a GNU Makefile.  However, there is a bash script included that will do
the heavy lifting.

Continuing in the Ubuntu bash shell, hoose a directory to build in and clone the Github project:

```
cd /mnt/c
mkdir -p git
cd git
git clone https://github.com/Geof23/kernel_filter_2d.git
```

You will also need to configure the submodule:

```
cd kernel_filter_2d
git submodule update --init --recursive
```

Now you are ready to perform the actual build:

```
./build.sh
```

You will see `CMake` run on `sail`; also, a couple of libraries will be renamed.

Then the script will run `make` on the included `Makefile`.

Assuming you didn't encounter any errors, you should be ready to run the program.

If you run into problems, you may be able to solve them by installing a missing
package, using `apt`.  For more complex problems, you may create an issue
[here](https://github.com/Geof23/kernel_filter_2d/issues).


### <a name="how-to-run-it">Running the project</a>

Now you can run the project, applying the kernels to an image of your choosing.

As previously mentioned, the project was tested on several `.jpeg` and `.png` format
files; there is also support for `.tiff` and `.gif`.

It is configured to work with images that allow loading and storing in `r8g8b8a8` format,
which means that each pixel consists of 32 bits of information, with 8 bits for each
color and alpha channel (rgba).  There may be formats that won't work in its current
configuration, perhaps if the images of too high or low a color depth, or if they
use color palattes (I'm not sure of all of `sail`'s capabilities and how it will
respond in all cases to my request for the format `SAIL_PIXEL_FORMAT_BPP32_RGBA`).

Anyway, to run the program, from the command line in the bash shell, run this:

```
./kernel_filter_2d [path to image -- such as /mnt/c/Users/[username]/Pictures/my_selfie.jpg]
```

You should see something like this:

```
loaded /mnt/c/Users/geofs/OneDrive/Pictures/pinball.jpg, 1024x683, jpg
applying Identity
applying EdgeD
applying EdgeL
applying EdgeC
applying Sharpen
applying BoxBlur
applying GaussianBlur_3x3
applying GaussianBlur_5x5
applying UnsharpMasking_5x5g
```

### <a name="how-it-works">How the program works</a>

The program is written in a single `.cpp` file, `kernel_filter_2d.cpp`, that contains
the `#include` statement for the dependent headers (from both the c++ library and sail),
a few structs, a class, and the `main` function.

It is built with the `c++20` specification, in
order to take advantage of the `span` class, which provides a reference to a contiguous sequence
of objects in memory.  This is used to refer to the rectangle around each pixel in order to
apply the kernel.  This program also takes advantage of templates, which allow you to
abstract the datatypes used in your code.  

The primary class that does the work to load an image, apply the filter, and then save
the processed file is called `filter`.  If you go to the bottom of the source code file
(use an editor of your choice, such as (`emacs`)[https://xkcd.com/378/], `vim`, or
`VS Code` (beyond the scope of this README, but there are many helpful articles on
The Google)), you will find the `main` function:

```
int
main(int argc, char* argv[]){
  sail_set_log_barrier(SAIL_LOG_LEVEL_SILENCE);
  sail_init_with_flags(SAIL_FLAG_PRELOAD_CODECS);
  //kernels: {{kernel matrix}, matrix order, global coefficient, name}
  vector<kernel<intp_t>> kernels =
    {
...
      "UnsharpMasking_5x5"}
    };
  filter f(argv[1]);
  for(auto& k: kernels) f.process(k).write();
}
```

It does some configuration for the image processing library, and then defines
a `vector` container, named `kernels`,  along with its contents (in
brace initializer format).

Beneath that is the declaration of our `filter` object, `f`, and the call to its constructor,
passing along the path to the source image that we pass in through the command line.

Then, each `kernel` from the `kernels` vector is iterated over and passed into `f.process`,
followed by a call to `write`.

The `filter` class, when constructed from the source image path, loads the image and stores
its pixels into the object `srcPixels`.  It also stores some information about the image
that is used to write the modified version (when `write` is called).

The `process` method (of `filter`) iterates over all of the pixels in the source image,
creating a new image in the object `dpixels`.  This object stores the pixels at a higher
resolution than the source image in order to provide some headroom for processing
(for instance, some kernels can produce pixel values that are too large to fit in 8 bits
per channel) and also to allow for negative values.  Usually a kernel that contains large coefficients or is large, so that a lot
of products are stored in each pixel, also have a small 'global coefficient' that is multiplied
with this final sum, and may enable the pixel values to fit back in the 8 bit/channel pixels that we are writing for our
filtered image (and possibly 'flip' negative values to the other side of zero).

However, in cases where the final processed pixels are too large our otherwise out of range for
the destination image format, the values are 'clamped' to fit in 8 bit values (0 - 255).  This means
that there is no effort to scale the range of the pixels to fit in the target precision (which is another
approach)-- intermediate values
over 255 are written as 255, and intermediate values less than 0 are written as 0 to the destination image.

It is the `write` method that examines each intermediate pixel, and writes it to the destination
image that we pass back to `sail` for writing to the filesystem.

Another design choice made in this program was how to handle edge pixels, where there are no matching
pixels for all of the kernel matrix entries.  This program simply does not process the edge pixels where
this is a problem (again, refer to `filter`'s `process` method).

You may modify the kernel specifications and add your own.  The `kernels` vector, as mentioned, is
written in brace initializer list format, such that: `{{kernel values}, order, gCoefficient, label}`.
The kernel values are defined as a 1d list, but processed as a 2d *square* matrix (so it is important
that the number of values you provide are a *square* value (i.e., the square root of that number is
an integer value)).  This square root of the kernel value count is the order of the matrix, and it needs
to be included, along with the global coefficient (technically the order is sqrt(n)xsqrt(n), with n
being the number of values).

If you modify the program, you will need to rebuild it before running it to test your changes.
To do this, simply issue the command `make` to the bash shell, from the root project directory.

### <a name="summary">Summary</a>

This project separately applies a series of 2d kernels to an image, and writes each of the results
as a new image.

It is a lot of fun to see how these filters work.  Some images may be too blury to begin with
to see the results of blur filters (you can increase the magnitude of the kernel matrix coefficients
to have a greater effect, or expand the size of the kernels to be 5x5, 7x7, etc, keeping the dimensions
odd, so that there is a center entry to provide symmetry about each target pixel).
Also, some images will lack well defined edges for the edge filters to
detect, and you may be left with little more than a black image product.

This is an introduction to convolution kernels.  They are important in many areas of computation,
such as convolutional neural networks, in order to extract features from data sets in Machine Learning
applications.  There are [many](https://towardsdatascience.com/intuitively-understanding-convolutions-for-deep-learning-1f6f42faee1)
articles available on the subject
