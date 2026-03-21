#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include "../header/golikenew.h"
#include <windows.h>
// ================= INPUT AUTH =================
void show_authorization_input(std::string& authorization) {
    while (true) {
        std::cout << "Nhập authorization: ";
        std::getline(std::cin, authorization);
        if (!authorization.empty() &&
            authorization.rfind("Bearer ", 0) == 0) {
            std::system("clear");
            break;
        }
        std::cout << "Authorization không hợp lệ!\n";
    }
}
// ================= SHOW INFO =================
void show_welcome_and_account_info(const GolikeClient& client) {
    auto me = client.ME();
    std::cout << std::string(20, ' ') << "Welcome\n";
    std::cout << std::string(30, '-') << "\n";
    std::cout << "Thông Tin Tài Khoản\n";
    std::cout << "ID   : " << me.id << "\n";
    std::cout << "Name : " << me.name << "\n";
    std::cout << "Coin : " << me.coin << "\n";
    std::cout << std::string(30, '-') << "\n";
}
// ================= CHOOSE PLATFORM =================
std::string choose_platform() {
    std::string choice;
    std::cout << "Loại Hình JOB:\n";
    std::cout << "1. Instagram\n";
    std::cout << "2. Tiktok\n";
    std::cout << "3. Facebook\n";
    std::cout << "Nhập lựa chọn: ";
    std::cin >> choice;
    std::cin.ignore();
    std::system("clear");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (choice == "1") return "instagram";
    if (choice == "2") return "tiktok";
    if (choice == "3") return "facebook";
    return "";
}
// ================= CHOOSE ACCOUNT =================
// size_t choose_account_index(size_t max) {
//     std::string input;
//     size_t index = 0;
//     while (true) {
//         std::cout << "Nhập tài khoản chạy JOB: ";
//         std::cin >> input;
//         try {
//             index = std::stoul(input);
//             if (index >= 1 && index <= max) {
//                 std::cin.ignore();
//                 return index - 1;
//             }
//         } catch (...) {}
//         std::cout << "Không hợp lệ, nhập lại!\n";
//     }
// }
// // ================= INPUT COOKIE =================
// std::string input_cookie(const std::string& name) {
//     std::string cookie;
//     while (true) {
//         std::cout << "Nhập cookie cho tài khoản [" << name << "]: ";
//         std::getline(std::cin, cookie);
//         if (!cookie.empty() && cookie.length() > 100) {
//             std::system("clear");
//             return cookie;
//         }
//         std::cout << "Cookie không hợp lệ!\n";
//     }
// }
// // ================= MAIN ACCOUNT FLOW =================
// void handle_account(Golike& g,const std::string& platform,long long& account_id,std::string& account_name,std::string& cookie_out) {
//     auto accounts = g.Account(platform);
//     if (accounts.empty()) {
//         std::cout << "Không có tài khoản!\n";
//         return;
//     }
//     // show list
//     for (size_t i = 0; i < accounts.size(); ++i) {
//         std::cout << i + 1 << ". [" << accounts[i].id << "] - [" << accounts[i].name << "]\n";
//     }
//     std::cout << std::string(30, '-') << "\n";
//     // chọn account
//     size_t index = choose_account_index(accounts.size());
//     const auto& selected = accounts[index];

//     account_id = selected.id;
//     account_name = selected.name;

//     std::system("clear");
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
//     // nhập cookie
//     cookie_out = input_cookie(account_name);
//     std::this_thread::sleep_for(std::chrono::milliseconds(500));
// }
