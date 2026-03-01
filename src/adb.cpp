#include <iostream>
#include "../header/adb.h"
#include <cstdio>
#include <sstream>
ADB::ADB(const std::string& devices) : devices(devices) {};
std::string ADB::ADB_MODE(const std::string& cmd) {
    std::string full_cmd = "adb " + cmd;
    FILE* pipe = _popen(full_cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "popen() failed! Command: " << full_cmd << std::endl;
        return "";
    }
    std::string result;
    char buffer[256];
    while(fgets(buffer,sizeof(buffer),pipe)!= nullptr) {
      result += buffer;
    }
    _pclose(pipe);
    return result;
}
std::vector<std::string> ADB::ADB_DEVICES() {
    std::string output = ADB_MODE("devices");
    std::istringstream iss(output);
    std::vector<std::string> result;
    std::string line;
    bool started = false;
    while(std::getline(iss,line)) {
      if (line.find("List of devices attached") != std::string::npos) {
          started = true;
          continue;
    }
    if (!started) continue;
    size_t tab_pos = line.find('\t');
    if (tab_pos != std::string::npos) {
            std::string serial = line.substr(0, tab_pos);
            std::string status = line.substr(tab_pos + 1);

            serial.erase(0, serial.find_first_not_of(" \t"));  // trim left
            serial.erase(serial.find_last_not_of(" \t") + 1);  // trim right
            if (status.find("device") != std::string::npos) {
                if (!serial.empty()) {
                    result.push_back(serial);
                }
            }
        }
    }
  return result;
}
std::string ADB::ADB_CLICK(const int& x,const int& y) {
  std::ostringstream click;
  click << "-s " << devices << " shell input tap " << x << " " << y;
  std::string result = ADB_MODE(click.str());
  return result;
}
std::string ADB::ADB_SWIPE(const int& x1 , const int& y1 , const int& x2 , const int& y2) {
  std::ostringstream iss;
  iss << "-s " << devices << " shell input swipe " << x1 << " " << y1 << " " << x2 << " " << y2;
  std::string result = ADB_MODE(iss.str());
  return result;
}
std::string ADB::ADB_TEXT(const std::string& text) {
  std::ostringstream input;
  input << "-s " << devices << " shell input text " << text;
  std::string result = ADB_MODE(input.str());
  return result;
}
std::string ADB::ADB_KEY(const std::string& key) {
  std::ostringstream out;
  out << "-s " << devices << " shell input keyevent " << key;
  std::string result = ADB_MODE(out.str());
  return result;
}