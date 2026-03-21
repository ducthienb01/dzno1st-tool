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
  auto client = std::make_unique<GolikeClient>(authorization, "golike");
  show_welcome_and_account_info(*client);
  auto golike = std::make_unique<FACEBOOK>(*client);
  client->setStrategy(std::move(golike));
  auto me = client->ME();
  curl_global_cleanup();
  return 0;
}