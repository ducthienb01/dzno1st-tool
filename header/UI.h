#ifndef UI_H
#define UI_H
#include <iostream>
#include "../header/golikenew.h"
void show_authorization_input(std::string& authorization);
std::string show_welcome_and_account_info(const GolikeClient& client);
std::string input_cookie(const std::string& name);
void handle_account(GolikeClient& g, int& account_id, std::string& account_name, std::string& cookie_out);
void AUTO_JOB(const GolikeClient& client);
#endif