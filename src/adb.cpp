#include <iostream>
#include "../header/adb.h"
#include <cstdio>
#include <sstream>
#include <utility>
ADB::ADB(const std::string& devices) : devices(devices) {}
std::pair<std::string , int> ADB::ADB_MODE(const std::string& cmd) {
  std::string full_cmd = "adb " + cmd + " 2>&1";
  FILE* pipe = _popen(full_cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "popen() failed! Command: " << full_cmd << std::endl;
    return {"popen failed",-1};
  }
  std::string result;
  char buffer[256];
  while(fgets(buffer,sizeof(buffer),pipe)!= nullptr) {
    result += buffer;
  }
  int exit_code = _pclose(pipe);
  return {result,exit_code};
}
std::vector<std::string> ADB::ADB_DEVICES() {
    auto [adb_output,exit_code] = ADB_MODE("devices");
    if (exit_code != 0) {
        return {};
    }
    std::istringstream iss(adb_output);
    std::vector<std::string> devices_list;
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
            if (status == "device" && !serial.empty()) {
            devices_list.push_back(serial);
          }
        }
    }
  return devices_list;
}
int ADB::ADB_CLICK(const int& x,const int& y) {
  std::ostringstream click;
  click << "-s " << devices << " shell input tap " << x << " " << y;
  auto [result,exit_code] = ADB_MODE(click.str());
  return exit_code;
}
std::string ADB::ADB_SWIPE(const int& x1 , const int& y1 , const int& x2 , const int& y2) {
  std::ostringstream iss;
  iss << "-s " << devices << " shell input swipe " << x1 << " " << y1 << " " << x2 << " " << y2;
  auto [result,exit_code] = ADB_MODE(iss.str());
  return result;
}
std::string ADB::ADB_TEXT(const std::string& text) {
  std::ostringstream input;
  input << "-s " << devices << " shell input text " << text;
  std::string result = ADB_MODE(input.str());
  return result;
}
void ADB::ADB_KEY(const std::string& key) {
  std::ostringstream out;
  out << "-s " << devices << " shell input keyevent " << key;
  std::string result = ADB_MODE(out.str());
}
std::string ADB::ADB_DUMP() {
  std::string shell = "-s " + devices + " shell uiautomator dump /sdcard/my_ui_dump.xml";
  std::string pull = "-s " + devices + " pull /sdcard/my_ui_dump.xml .";
  std::string result = ADB_MODE(shell);
  if (result.find("UI hierarchy dumped") == std::string::npos && result.find("Success") == std::string::npos) {
    std::cerr << "Dump thất bại: " << result << std::endl;
    return "";
  }
  std::string result2 = ADB_MODE(pull);
  if (result2.find("file pulled") == std::string::npos) {
      std::cerr << "Pull thất bại: " << result2 << std::endl;
      return "";
    }
  return result2;
}