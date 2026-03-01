#include <iostream>
#include <thread>
#include <chrono>
#include "../header/instagram.h"
#include "./header/golike.h"
#include "./header/loadjob.h"
#include "./header/UI.h"
#include "./header/TDS.h"
#include <windows.h>
int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  std::string authorization;
  show_authorization_input(authorization);
  Golike g(authorization);
  show_welcome_and_account_info(g);
  std::string inplat;
  show_platform_menu(inplat);
  std::string name, cookie;
  long long choosed;
  show_account(g, inplat, choosed, name, cookie);
  Loadjob load(g, cookie);
  load.run(inplat, choosed);
  curl_global_cleanup();
  return 0;
}