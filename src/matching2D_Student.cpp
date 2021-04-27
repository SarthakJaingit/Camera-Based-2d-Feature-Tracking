#include <numeric>
#include <stdexcept>
#include "matching2D.hpp"

using namespace std;

//Current Project Bugs: Some Descriptors not working some pairs work. All the the pairs with brisk work. 

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        int normType; 
        if (descriptorType.compare("DES_HOG") == 0){
            normType = cv::NORM_L2; 
        }else{
            normType = cv::NORM_HAMMING; 
        }
        
        matcher = cv::BFMatcher::create(normType, crossCheck);

        cout << "Brute Force Matching" << endl; 
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        // Implement this matching 
        if (descSource.type() != CV_32F || descRef.type() != CV_32F){
            descSource.convertTo(descSource, CV_32F); 
            descRef.convertTo(descRef, CV_32F); 
        }

        matcher = cv::FlannBasedMatcher::create(); 
        cout << "Flann based Matching" << endl; 
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)
        
        double t = (double)cv::getTickCount(); 
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency(); 
        cout << selectorType << " selecting best match in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        vector<vector<cv::DMatch>> knn_matches; 
        double t = (double)cv::getTickCount(); 
        matcher -> knnMatch(descSource, descRef, knn_matches, 2); 

        float ratio_thresh = 0.8f; 
        
        for (size_t i = 0; i < knn_matches.size(); ++i){
            //remove ambigous points that are too similiar to the image intensities of other best matched points
            //Every point should have one matched point that greatly outperforms the distance between other keypoints
            if (knn_matches[i][0].distance < knn_matches[i][1].distance * ratio_thresh){
                matches.push_back(knn_matches[i][0]); 
            } 
        }

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency(); 
        cout << selectorType << " selecting best match in " << 1000 * t / 1.0 << " ms" << endl;
        
    }
}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    cv::Ptr<cv::DescriptorExtractor> extractor;
    if (descriptorType.compare("BRISK") == 0)
    {

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = cv::BRISK::create(threshold, octaves, patternScale);
    }
    else if (descriptorType.compare("BRIEF") == 0){
        extractor = cv::xfeatures2d::BriefDescriptorExtractor::create(); 
    }
    else if (descriptorType.compare("ORB") == 0){

        extractor = cv::ORB::create(); 
    }
    else if (descriptorType.compare("AKAZE") == 0){
        extractor = cv::AKAZE::create(); 
    }
    else if (descriptorType.compare("FREAK") == 0){
        extractor = cv::xfeatures2d::FREAK::create();
    }
    else if (descriptorType.compare("SIFT") == 0){
        extractor = cv::SIFT::create(); 
    }
    else{
        throw std::invalid_argument("fast descriptor type " + descriptorType + " not supported by code"); 
    }
    //for describing keypoints
    // perform feature description
    double t = (double)cv::getTickCount();
    extractor->compute(img, keypoints, descriptors);

    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
}

void detKeypointsModern(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, std::string detectorType, bool bVis)
{
    cv::Ptr<cv::Feature2D> detector;
    double t = (double)cv::getTickCount(); 

    if (detectorType.compare("BRISK") == 0)
    {

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        detector = cv::BRISK::create(threshold, octaves, patternScale);
        detector -> detect(img, keypoints); 
    }
    else if(detectorType.compare("FAST") == 0){

        int threshold = 30; 
        detector = cv::FastFeatureDetector::create(threshold);
        detector -> detect(img, keypoints); 
    }
    else if (detectorType.compare("ORB") == 0){

        detector = cv::ORB::create(); 
        detector -> detect(img, keypoints); 
    }
    else if (detectorType.compare("AKAZE") == 0){

        detector = cv::AKAZE::create(); 
        detector -> detect(img, keypoints); 

    }
    else if (detectorType.compare("SIFT") == 0){

        detector = cv::SIFT::create(); 
        detector -> detect(img, keypoints); 

    }
    else{ 
        throw std::invalid_argument("fast descriptor type " + detectorType + " not supported by code"); 
    }

    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << detectorType << " with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = detectorType + " results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey();
    }
    
}


// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsHarris(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis){


    // Debugged Issues is that when Point(x, y) the row you are on is the y and the column you are on is the x

    int blocksize = 2; 
    int aperture_size = 3;
    double k = 0.04;

    int thresh = 100; 

    cv::Mat dst, dst_norm, dst_norm_scaled; 
    dst = img.clone();  

    double t = (double)(cv::getTickCount()); 
    cv::cornerHarris(img, dst, blocksize, aperture_size, k); 

    cv::normalize(dst, dst_norm,0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    for (size_t i = 0; i < dst_norm.rows; ++i){
        for (size_t j = 0; j < dst_norm.cols; ++j){
            if (dst_norm.at<float>(i, j) > thresh){ 
                cv::KeyPoint newKeyPoint; 
                newKeyPoint.pt = cv::Point2f(j, i); 
                newKeyPoint.size = 2 * aperture_size; 
                keypoints.push_back(newKeyPoint); 
                
            }
        }
    }

    t = ((double)(cv::getTickCount()) - t) / cv::getTickFrequency(); 
    cout << "Harris detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = dst_norm_scaled.clone(); 
        string windowName = "Harris Corner Detection Results";
        cv::drawKeypoints(dst_norm_scaled, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey();
    }

}