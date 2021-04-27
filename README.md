# Camera-Based-2d-Feature-Tracking
Using Opencv and C++ to perform Keypoint detection, description, matching, and analyzation of different methods. 
Matching on a set of images of cars all located in images directory. 

My best choice and explanation: 
* FAST Detector -> This detector finds sufficient amount of keypoints in the shortest amount of time. It compares a few pixels in the circle around a pixel and uses them to determine if that pixel is a keypoint.

* Brief Descriptor -> This descriptor is binary which compares pixels in a path to quickly form a unique configuration of 0s and 1s. Therefore, it was the fastest descriptor. 

* BF Matching -> Brute Force compared distances between keypoint descriptors faster than the FLANN. This is probably b/c the keypoints is not in very high quanitities and forming a binary tree might still take longer time.

* SELKNN -> While this matching step was slower than NN since it involved extra computation to remove ambigous points, it is a more stable practice since it leads to more accurate matches.

Next choices:
* BRISK and BRIEF -> Most keypoints and fastest descriptor
* FAST and BRISK -> FAST found keypoints with little time and worked fast with a BRISK descriptor.

Close:
* Harris and BRIEF -> less but visualized more accurate keypoints and fastest descriptor

## Important Note: My results and captured data are all in the csv file attatched to the github.


