#ifndef UI_H
#define UI_H
#include <iostream>
#include "./header/golike.h"
void show_authorization_input(std::string& authorization);
void show_welcome_and_account_info(const Golike& g);
void show_platform_menu(std::string& choice);
void show_account(Golike& g,std::string& choice,long long& select,std::string& name,std::string& cookies);
#endif