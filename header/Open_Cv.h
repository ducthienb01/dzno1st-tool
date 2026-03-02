#ifndef OPEN_CV_UTILS_H
#define OPEN_CV_UTILS_H

#include <string>

bool find_image_on_screen(const std::string& screen_path,
                          const std::string& template_path,
                          double threshold = 0.8);

#endif