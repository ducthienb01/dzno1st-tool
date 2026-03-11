#ifndef OPEN_CV_UTILS_H
#define OPEN_CV_UTILS_H

#include <string>
#include <opencv2/opencv.hpp>
struct MatchResult {
    bool found;
    double score;
    cv::Point location;
    cv::Point center;  // góc trên-trái của template khớp
};
class OPENCV {
    MatchResult find_image_on_screen(const std::string& screen_path,const std::string& template_path,double threshold = 0.8);
    void simulateMouseClick(int& x, int& y, bool rightclick = false);
};
#endif