#ifndef ADB_H
#define ADB_H
#include <iostream>
#include <vector>
#include <utility>
class ADB {
    private:
     std::string devices;
    public:
     ADB(const std::string& devices);
     std::pair<std::string, int> ADB_MODE(const std::string& cmd);
     std::vector<std::string> ADB_DEVICES();
     int ADB_CLICK(const int& x, const int& y);
     int ADB_SWIPE(const int& x1 , const int& y1 , const int& x2 , const int& y2);
     int ADB_TEXT(const std::string& text);
     int ADB_KEY(const std::string& key);
     int ADB_DUMP();
};
#endif