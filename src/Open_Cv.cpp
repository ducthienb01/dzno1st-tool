#include "../header/Open_Cv.h"
#include <opencv2/opencv.hpp>
#include <iostream>

bool find_image_on_screen(const std::string& screen_path,
                          const std::string& template_path,
                          double threshold)
{
    cv::Mat screen = cv::imread(screen_path);
    cv::Mat templ  = cv::imread(template_path);

    if (screen.empty() || templ.empty()) {
        std::cerr << "Không đọc được ảnh!\n";
        return false;
    }

    cv::Mat result;
    cv::matchTemplate(screen, templ, result, cv::TM_CCOEFF_NORMED);

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;

    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    std::cout << "Độ khớp cao nhất: " << maxVal << std::endl;

    return maxVal >= threshold;
}