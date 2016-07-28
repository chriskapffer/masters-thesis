# masters-thesis
Development of a hybrid object tracker for markerless-augmented reality on the iPhone

With AR being all hot and trendy and computer vision being a focus of my studies (right after game development) I decided to build a basic 'markerless' augmented reality app for my good old iPhone 4S and write about it in my masters thesis at [HTW Berlin](http://www-en.htw-berlin.de/).

What this app does is allowing the user to take a picture of a flat surface (which needs to be visualy diverse). Once the picture is taken the app switches to the video camera feed and tries to detect any key points of the picture taken earlier within the current frame of the video. If it finds enough key points (thus recognizing the target) it displays the common [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot) on top of it with the correct transformation regarding the phone's position and orientation relative to the target. This gives the impression of the pot being physically present in the scene which is what augmented reality is all about.

The computer vision part is handled by the great [OpenCV](http://opencv.org/) library. All business logic is written in C++ and encapsulated in a static library, which is used by the iPhone demo and test apps, as well as the corresponding OSX demo and test apps. Input and presentation logic are implemented in Objective-C. Sorry, no Swift yet.

## Things to improve
* update OpenCV to the latest version
* try out some new key descriptors
* avoid frame rate drop down when target is not present
* improve pose estimation of the pot to avoid skew and flickering