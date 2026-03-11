#include "../header/Open_Cv.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "./header/Open_Cv.h"
#include "windows.h"
#include <thread>
#include <chrono>
MatchResult OPENCV::find_image_on_screen (const std::string& screen_path,const std::string& template_path,double threshold)
{
    cv::Mat screen = cv::imread(screen_path);
    cv::Mat templ  = cv::imread(template_path);

    if (screen.empty() || templ.empty()) {
        std::cerr << "Không đọc được ảnh!\n";
        return {false, 0.0, cv::Point(-1, -1)};
    }

    cv::Mat result;
    cv::matchTemplate(screen, templ, result, cv::TM_CCOEFF_NORMED);

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    std::cout << "Độ khớp cao nhất: " << maxVal << " tại vị trí: " << maxLoc << std::endl;

    bool found = (maxVal >= threshold);
    cv::Point bestLoc = maxLoc;
    int center_x = bestLoc.x + templ.cols / 2;
    int center_y = bestLoc.y + templ.rows / 2;
    cv::Point center(center_x, center_y);
    return {found, maxVal, bestLoc,center};
}
void OPENCV::simulateMouseClick (int& x, int& y, bool rightclick = false) {
    bool success = SetCursorPos(x,y);
    if (!success) {
      std::cerr << "SetCursorPos failed! Error: " << GetLastError() << std::endl;
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    INPUT inputs[2] = {0};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = rightclick ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = rightclick ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    UINT sent = SendInput(2, inputs, sizeof(INPUT));
    if (sent != 2) {
        std::cerr << "SendInput failed! Sent: " << sent << ", Error: " << GetLastError() << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}