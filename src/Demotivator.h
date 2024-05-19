#pragma once

#include <httplib.h>
#include <random>
#include <string>

inline std::mt19937 rnd(100500);
const int len = 93;
inline std::string createDemotivator() {
    std::string image_url = "https://github.com/LinkCatList/krosh-ds-bot/blob/main/images/" + std::to_string(rnd() % len + 1) + ".png"; // Ссылка на картинку
    std::string top_text = "camel_case";
    std::string bottom_text = "верблюжий падеж";

    std::string url = "https://api.memegen.link/images/demotivational/" + top_text + "/" + bottom_text + "?url=" + image_url;
    std::replace(url.begin(), url.end(), ' ', '_'); // Замена пробелов

    httplib::Client cli("api.memegen.link");
    auto res = cli.Get(url.c_str());

    if (res && res->status == 200) {
        std::ofstream file(image_url, std::ios::binary);
        file.write(res->body.c_str(), res->body.size());
        std::cout << "Мем сохранен в файл " + image_url << std::endl;
    } else {
        std::cerr << "Ошибка: " << res->status << " - " << res->reason << std::endl;
    }
    return image_url;
}
