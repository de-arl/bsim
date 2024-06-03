# Installation
This is a step by step guide to install the software.
It explains the installation under Ubuntu 24.04 LTS.
All dependencies are available in the Ubuntu package manager.

## Headless version
1. Clone the repository:
> `$ git clone https://github.com/de-arl/bsim`

2. Create build directory
> `$ cd bsim && mkdir build && cd build`

3. Prepare the installation
> `$ ccmake ..`

4. Configure
> In the ccmake menu, set the CMAKE_BUILD_TYPE to 'Release'.
> Press `c` to configure and `g` to generate the makefiles.

5. Build and install
> `$ make install`

Now the headless version is available in the install directory bsim/bsim.

## Animation
The animation part has more dependencies, if all of them are installed, you can choose 'ENABLE_ANIMATION' in the ccmake configuration window and build and install. 

## Dependencies
### Build toolchain
Git:
> `sudo apt install git`

Compiler g++:
> `sudo apt install g++`

Make:
> `sudo apt install make`

Cmake:
> `sudo snap install --classic cmake`

### Animation
GLFW: OpenGL Graphics Library Framework [(website)](https://www.glfw.org/) [source](https://github.com/glfw/glfw.git)
> `sudo apt install libglfw3-dev`

GLM: OpenGL math library [(source)](https://github.com/g-truc/glm)
> `sudo apt install libglm-dev`

WebP Codec [(website)](https://developers.google.com/speed/webp) [source](https://chromium.googlesource.com/webm/libwebp)
> `sudo apt install libwebp-dev`

Freetype [(website)](https://www.freetype.org/)
> `sudo apt install libfreetype-dev

FFmpeg [(website)](https://ffmpeg.org/)
> `sudo apt install ffmpeg`

x265 Codec [(website)](https://www.videolan.org/developers/x265.html)
> `sudo apt install libx265-dev`

### Performance measurement
You can enable the PAPI module to measure the performance of the simulation.
Either select 'PAPI_HL' (high-level API) or 'PAPI_LL' (low-level API) in the ccmake configuration window.

Depends on:
PAPI: Performance Application Programming Interface PAPI [(website)](https://icl.utk.edu/papi/)
> `sudo apt install libpapi-dev`
 
