#include <iostream>
#include <thread>
#include <chrono>
#include "../header/instagram.h"
#include "./header/golike.h"
#include "./header/loadjob.h"
#include "./header/UI.h"
#include "./header/TDS.h"
#include <windows.h>
#include <curl/curl.h>
#include "../header/golikenew.h"
#include <regex>
#include <pybind11/embed.h>
namespace py = pybind11;
int main() {
  py::scoped_interpreter guard{};
  curl_global_init(CURL_GLOBAL_ALL);
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  std::string authorization;
  show_authorization_input(authorization);
  auto client = std::make_unique<GolikeClient>(authorization);
  show_welcome_and_account_info(*client);
  auto me = client->ME();
  auto fbptr = std::make_unique<FACEBOOK>(*client);
  client->setStrategy(std::move(fbptr));
  int account_id;
  std::string account_name, cookiess;
  handle_account(*client,account_id,account_name,cookiess);
  auto fb = dynacmic_cast<FACEBOOK*>(client->getStrategy());
  
  curl_global_cleanup();
  return 0;
}