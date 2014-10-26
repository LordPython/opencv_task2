#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

int threshold = 800;

cv::Mat displayMatches(cv::Mat im1, cv::Mat im2) {
    cv::SurfFeatureDetector detector(threshold);

    std::vector<cv::KeyPoint> key1, key2;

    // Detect surface features
    detector.detect(im1, key1);
    detector.detect(im2, key2);

    cv::SurfDescriptorExtractor extractor;

    cv::Mat desc1, desc2;

    // Compute surface descriptors for each feature
    extractor.compute(im1, key1, desc1);
    extractor.compute(im2, key2, desc2);

    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;

    // Find matches between features in the two images.
    matcher.match(desc1, desc2, matches);

    // Possibly do some filtering here.
    std::vector<cv::DMatch> filteredMatches;

    double max_dist = 0;
    double min_dist = 100;

    for( int i = 0; i < desc1.rows; i++ ) { 
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    for( int i = 0; i < desc1.rows; i++ ) { 
        if( matches[i].distance <= cv::max(1.5*min_dist, 0.02) ) { 
            filteredMatches.push_back(matches[i]);
        }
    }

    // Output image
    cv::Mat disp;
    // Draw feature matches to output image
    cv::drawMatches(im1, key1, im2, key2, filteredMatches, disp,
                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                    cv::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    return disp;
}

cv::Point2f findCorner(cv::Mat image) {
    cv::Size patternsize(3, 3);
    std::vector<cv::Point2f> corners;

    bool patternfound = cv::findChessboardCorners(image, patternsize, corners, 0);

    if(!patternfound) {
        std::cout << "Couldn't find chessboard" << std::endl;
        return cv::Point2f(0, 0);
    }
    
    return corners[4];
}

void projectIMG5(std::string im5_path, std::string output_path) {
    cv::namedWindow("Checkerboard");


    cv::Mat im5 = cv::imread(im5_path, CV_LOAD_IMAGE_COLOR);

    cv::Mat upperLeft(im5, cv::Rect(0,0, im5.cols/2, im5.rows/2));
    cv::Mat upperRight(im5, cv::Rect(im5.cols/2, 0, im5.cols/2, im5.rows/2));
    cv::Mat lowerLeft(im5, cv::Rect(0, im5.rows/2, im5.cols/2, im5.rows/2));
    cv::Mat lowerRight(im5, cv::Rect(im5.cols/2, im5.rows/2, im5.cols/2, im5.rows/2));

    cv::Point2f corner1 = findCorner(upperLeft);

    cv::Point2f corner2 = findCorner(upperRight);
    corner2.x += im5.cols/2;

    cv::Point2f corner3 = findCorner(lowerLeft);
    corner3.y += im5.rows/2;

    cv::Point2f corner4 = findCorner(lowerRight);
    corner4.x += im5.cols/2;
    corner4.y += im5.rows/2;

    cv::circle(im5, corner1, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(im5, corner2, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(im5, corner3, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(im5, corner4, 5, cv::Scalar(255, 0, 0), -1);

    std::vector<cv::Point2f> corners { corner1, corner2, corner3, corner4 };
    std::vector<cv::Point2f> projected { cv::Point2f(0,0),
                                         cv::Point2f(960,0),
                                         cv::Point2f(0,540),
                                         cv::Point2f(960,540) };

    cv::Mat projMat = cv::getPerspectiveTransform(corners, projected);

    cv::Mat projectedImage;

    cv::warpPerspective(im5, projectedImage, projMat, cv::Size(960,540));

    cv::imwrite(output_path, projectedImage);
}

void matchFeatures(std::string img1_path,
                   std::string img2_path,
                   std::string img3_path,
                   std::string img4_path) {
    cv::Mat im1 = cv::imread(img1_path, CV_LOAD_IMAGE_COLOR);
    cv::Mat im2 = cv::imread(img2_path, CV_LOAD_IMAGE_COLOR);
    cv::Mat im3 = cv::imread(img3_path, CV_LOAD_IMAGE_COLOR);
    cv::Mat im4 = cv::imread(img4_path, CV_LOAD_IMAGE_COLOR);

    cv::Mat matches_1_2 = displayMatches(im1, im2);
    cv::Mat matches_1_3 = displayMatches(im1, im3);
    cv::Mat matches_1_4 = displayMatches(im1, im4);
    cv::Mat matches_2_3 = displayMatches(im2, im3);
    cv::Mat matches_2_4 = displayMatches(im2, im4);
    cv::Mat matches_3_4 = displayMatches(im3, im4);

    cv::imwrite("output/matches_1-2.jpg", matches_1_2);
    cv::imwrite("output/matches_1-3.jpg", matches_1_3);
    cv::imwrite("output/matches_1-4.jpg", matches_1_4);
    cv::imwrite("output/matches_2-3.jpg", matches_2_3);
    cv::imwrite("output/matches_2-4.jpg", matches_2_4);
    cv::imwrite("output/matches_3-4.jpg", matches_3_4);
}

int main(int argc, char** argv) {
    matchFeatures("materials/img1.jpg",
                  "materials/img2.jpg",
                  "materials/img3.jpg",
                  "materials/img4.jpg");
    /*
    cv::Mat im1 = cv::imread("materials/img3.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat im2 = cv::imread("materials/img1.jpg", CV_LOAD_IMAGE_COLOR);

    cv::Mat disp = displayMatches(im1, im2);

    cv::imwrite("out.jpg", disp);
    */

    projectIMG5("materials/img5_modified.jpg", "output/projected.jpg");
    projectIMG5("materials/img5.jpg", "output/orig_projected.jpg");
}
