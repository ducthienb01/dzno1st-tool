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
  std::pair<std::string, int> res = ADB_MODE("devices");
    std::string adb_output = res.first;
    int exit_code   = res.second;
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
  auto res = ADB_MODE(click.str());
  return res.second;
}
int ADB::ADB_SWIPE(const int& x1 , const int& y1 , const int& x2 , const int& y2) {
  std::ostringstream iss;
  iss << "-s " << devices << " shell input swipe " << x1 << " " << y1 << " " << x2 << " " << y2;
  auto res = ADB_MODE(iss.str());
  return res.second;
}
int ADB::ADB_TEXT(const std::string& text) {
  std::ostringstream input;
  input << "-s " << devices << " shell input text " << text;
  auto res = ADB_MODE(input.str());
  return res.second;
}
int ADB::ADB_KEY(const std::string& key) {
  std::ostringstream out;
  out << "-s " << devices << " shell input keyevent " << key;
  auto res = ADB_MODE(out.str());
  return res.second;
}
int ADB::ADB_DUMP() {
  std::string shell = "-s " + devices + " shell uiautomator dump /sdcard/my_ui_dump.xml";
  std::string pull_cmd = "-s " + devices + " pull /sdcard/my_ui_dump.xml .";
  auto dump_res = ADB_MODE(shell);
  std::string dump_out = dump_res.first;
  int dump_exit = dump_res.second;
  if (dump_exit != 0 || dump_out.find("UI hierarchy dumped") == std::string::npos && dump_out.find("Success") == std::string::npos) {
        std::cerr << "Dump thất bại: " << dump_out << std::endl;
        return dump_exit ? dump_exit : 1;  // trả exit_code hoặc 1 nếu fail im lặng
    }
   auto pull_res = ADB_MODE(pull_cmd);
   std::string pull_out = pull_res.first;
   int pull_exit = pull_res.second;
   if (pull_exit != 0 || pull_out.find("file pulled") == std::string::npos) {
        std::cerr << "Pull thất bại: " << pull_out << std::endl;
        return pull_exit ? pull_exit : 1;
    }
  return 0;
}