# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv_modules.hpp"
#include <stdio.h>
//# include "opencv2/nonfree/features2d.hpp"

using namespace cv;

//void readme();

/**
 * @function main
 * @brief Main function
 */
int main(int argc, char** argv)
{
    //if (argc != 3)
    //{
    //    readme(); return -1;
    //}
    

    //Mat img_1 = imread("1.png", IMREAD_COLOR);//IMREAD_GRAYSCALE
    //Mat img_2 = imread("2.png", IMREAD_COLOR);//IMREAD_GRAYSCALE

    Mat img_1 = imread("box.png", IMREAD_COLOR);//IMREAD_GRAYSCALE
    Mat img_2 = imread("box_in_scene.png", IMREAD_COLOR);//IMREAD_GRAYSCALE

    if (!img_1.data || !img_2.data)
    {
        printf(" --(!) Error reading images \n"); return -1;
    }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    Ptr<FeatureDetector> detector = ORB::create();

    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector->detect(img_1, keypoints_1);
    detector->detect(img_2, keypoints_2);

    //-- Step 2: Calculate descriptors (feature vectors)
    Ptr<DescriptorExtractor> descriptor = ORB::create();
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
    Mat descriptors_1, descriptors_2;

    descriptor->compute(img_1, keypoints_1, descriptors_1);
    descriptor->compute(img_2, keypoints_2, descriptors_2);

    Mat outimg1;
    drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("ORB", outimg1);

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    //FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher->match(descriptors_1, descriptors_2, matches);

    double max_dist = 0; double min_dist = 10000;

    //-- Quick calculation of max and min distances between keypoints
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    std::vector< DMatch > good_matches;

    for (int i = 0; i < descriptors_1.rows; i++)
    {
        if (matches[i].distance <= max(2 * min_dist, 0.02))
        {
            good_matches.push_back(matches[i]);
        }
    }

    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches(img_1, keypoints_1, img_2, keypoints_2,
        good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
        std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- Show detected matches
    imshow("Good Matches", img_matches);

    for (int i = 0; i < (int)good_matches.size(); i++)
    {
        printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
    }

    waitKey(0);

    return 0;
}

/**
 * @function readme
 */
void readme()
{
    printf(" Usage: ./ORB_BruteForceMatcher <img1> <img2>\n");
}

