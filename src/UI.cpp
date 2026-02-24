#include <iostream>
#include "./header/UI.h"
#include <thread>
#include <chrono>
void show_authorization_input(std::string& authorization) {
    while (true) {
        std::cout << "Nhập authorization:";
        std::getline(std::cin, authorization);
        std::cin.ignore();
        if (!authorization.empty() && authorization.size() >= 7 && authorization.substr(0, 7) == "Bearer ") {
        std::system("clear");
        break;
        }
        std::cout << "Authorization không hợp lệ!" << std::flush;
        continue;
    }
}
void show_welcome_and_account_info(const Golike& g) {
    auto me = g.ME();
    std::string LOGO = std::string(20, ' ') + "Wellcome";
    std::cout << LOGO << std::endl;
    std::string space1 = std::string(20, '-');
    std::cout << space1 << std::endl;
    std::string thongtin = "Thông Tin Tài Khoản";
    std::cout << thongtin << std::endl;
    std::cout << "ID: " << me.id << std::endl;
    std::cout << "Name: " << me.name << std::endl;
    std::cout << "Coin: " << me.coin << std::endl;
    std::string space2 = std::string(20, '-');
    std::cout << space2 << std::endl;
}
void show_platform_menu(std::string& choice) {
    std::string h = "Loại Hình JOB:";
    std::cout << h << std::endl;
    std::string i1 = "1.[-Instagram-]";
    std::cout << i1 << std::endl;
    std::string i2 = "2.[-Tiktok-]";
    std::cout << i2 << std::endl;
    std::string ht = "Nhập Lựa chọn của bạn : ";
    std::cout << ht << std::flush;
    std::cin >> choice;
    std::cin.ignore();
    std::system("clear");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
void show_account(Golike& g, std::string& choice, long long& select, std::string& name, std::string& cookies) {
    if (choice == "1") {
        choice = "instagram";
        auto acc = g.Account(choice);
        if (acc.empty()) return;
        for (size_t i = 0; i < acc.size();i++) {
            std::cout << i + 1 << "." << "[" << acc[i].id << "]" << "-" << "[" << acc[i].name << "]" << std::endl;
        }
        std::string st = (20, "-");
        std::cout << st << std::flush;
        std::string choices;
        size_t selects = 0;
        std::cout << "Nhập tài khoản chạy JOB :";
        std::cin >> choices;
        std::cin.ignore();
        while (true) {
            try {
                selects = std::stoul(choices);
                if (selects >= 1 && selects <= acc.size()) {
                    break;
                }
            }
            catch (std::exception& e) {
                std::cout << "So khong hop le! Hay nhap lai.\n";
            }
        }
        const auto& selected = acc[selects - 1];
        std::system("clear");
        select = selected.id;
        name = selected.name;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string cookie;
        while (true) {
            std::cout << "Nhập cookie cho tài khoản - " << name << ":" << std::flush;
            std::getline(std::cin, cookie);
            std::cin.ignore();
            if (!cookie.empty() && cookie.length() > 100 ) {
                cookies = cookie;
                std::system("clear");
                break;
            }
            std::cout << "Cookie không được để trống\n";
            continue;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}