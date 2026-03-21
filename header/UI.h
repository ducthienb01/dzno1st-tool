#ifndef UI_H
#define UI_H
#include <iostream>
#include "../header/golikenew.h"
void show_authorization_input(std::string& authorization);
void show_welcome_and_account_info(const GolikeClient& client);
std::string choose_platform();
void show_account(Golike& g,std::string& choice,long long& select,std::string& name,std::string& cookies);
#endif