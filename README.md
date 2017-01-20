# MotiveCapturedVolume
Tool to compute the volume captured by a group of Optitrack cameras.

The captured volume is computed dividing the workspace in voxels, which are individually tested against the frustum of each camera. From the GUI, a user can set voxel definition and minimum number of cameras from which a voxel needs to be seen.

The application is able to export point clouds and 3D models in PLY format.

## dependencies
* OpenFrameworks 0.9.x
* ofxMotive ( https://github.com/magiclab/ofxMotive )
* ofxUI ( https://github.com/rezaali/ofxUI )

The Motive API is Windows only, so this application runs on Windows.

## license
GPL v.3

##screenshots
![alt text](https://raw.githubusercontent.com/magiclab/MotiveCapturedVolume/master/screen0.png "setup screen")

![alt text](https://raw.githubusercontent.com/magiclab/MotiveCapturedVolume/master/screen1.png "voxels view")

![alt text](https://raw.githubusercontent.com/magiclab/MotiveCapturedVolume/master/screen2.png "convex hull view")
