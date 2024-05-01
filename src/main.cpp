#include <cctype>
#include <dpp/colors.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <dpp/unicode_emoji.h>
#include <dpp/dpp.h>
#include <random>
#include <regex>
#include <chrono>
#include "Database.h"
#include "../include/laserpants/dotenv/dotenv.h"

std::mt19937 rnd(100500);
const std::vector<std::string> wordsYes = {"виндa", "проводa", "бородa"};
const std::vector<std::string> wordsNo = {"солнышка ответ", "питониста аргумент", "насри в пакет"};

struct User {
    std::string user_id;
    std::string user_name;
    int count_watermelons;
};

struct Post {
    std::string user_id;
    std::string msg_id;
    bool is_active;
};

int64_t isoToTime (std::string &s) {
    return ((s[11] - '0') * 10 + s[12] - '0') * 60 * 60 + ((s[14] - '0') * 10 + s[15] - '0') * 60 + (s[17] - '0') * 10 + s[18] - '0';
}

bool is_id (std::string &name) {
    for (const auto &c : name) {
        if (isalpha(c)) {
            return false;
        }
    }
    return true;
}

int main() {
    dotenv::init();
    const std::string URL = std::getenv("URL");
    Database db(URL);
    
    const std::string BOT_TOKEN = std::getenv("BOT_TOKEN");
    dpp::cluster bot(BOT_TOKEN, dpp::i_default_intents | dpp::i_message_content);
    
    bot.on_log(dpp::utility::cout_logger());
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        if (event.command.get_command_name() == "start") {
            dpp::embed embed = dpp::embed()
                .set_color(dpp::colors::red_fox)
                .set_title("Krosh")
                .set_url("https://youtu.be/dQw4w9WgXcQ?si=h7Fn3QO4zJN-1SSR")
                .set_author("Krosh", "https://youtu.be/dQw4w9WgXcQ?si=h7Fn3QO4zJN-1SSR", "https://github.com/LinkCatList/krosh-ds-bot/blob/main/img/latest.png?raw=true")
                .set_description("Dm me tomat")
                .set_thumbnail("https://github.com/LinkCatList/krosh-ds-bot/blob/main/img/latest.png?raw=true")
                .set_image("https://github.com/LinkCatList/krosh-ds-bot/blob/main/img/latest.png?raw=true")
                .set_footer(
                    dpp::embed_footer()
                    .set_icon("https://github.com/LinkCatList/krosh-ds-bot/blob/main/img/latest.png?raw=true")
                )
                .set_timestamp(time(0));
            dpp::message msg(event.command.channel_id, embed);
            event.reply(msg);
        }
        if (event.command.get_command_name() == "farm") {
            dpp::message msg(event.command.channel_id, "Выбери действие");
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Посадить арбуз")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::watermelon)
                        .set_style(dpp::cos_primary)
                        .set_id("1")
                )
            );
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Подарить арбуз")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::watermelon)
                        .set_style(dpp::cos_primary)
                        .set_id("2")
                )
            );
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Статистика арбузов")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::watermelon)
                        .set_style(dpp::cos_primary)
                        .set_id("3")
                )
            );
            event.reply(msg);
        }
    });
    
    bot.on_button_click([&bot, &db](const dpp::button_click_t& event) {
        auto name = event.command;
        if (event.custom_id == "1") {
            int god = rnd();
            bool success = (god % 2 == 0) || (god % 3 == 0);
            if (success) {
                bool flag = db.queryValue<bool>("select exists(select 1 from users where user_id=$1)", std::to_string(name.usr.id));
                auto current_time = time(NULL);

                if (flag) {
                    auto last_grow = db.queryValue<int64_t>("select last_grow from users where user_id=$1", std::to_string(name.usr.id));
                    // std::cout << current_time << " " << last_grow << "\n";
                    int64_t diff = current_time - last_grow;
                    if (diff >= (int64_t)60 * (int64_t)60) {
                        int count_watermelons = db.queryValue<int>("select count_watermelons from users where user_id=$1", std::to_string(name.usr.id));
                        db.exec("update users set count_watermelons=$1, last_grow=$2 where user_id=$3", count_watermelons + 1, current_time, std::to_string(name.usr.id));
                        event.reply("<@" + std::to_string(name.usr.id) + "> успешно посадил один арбуз");
                    }
                    else {
                        event.reply("<@" + std::to_string(name.usr.id) + "> потерпи еще " + std::to_string(60 - diff / 60) + " минут до следующей посадки арбуза");
                    }
                }
                else {
                    db.exec("insert into users values($1, $2, $3, $4)", std::to_string(name.usr.id), std::string(name.usr.username), "1", current_time);
                    event.reply("<@" + std::to_string(name.usr.id) + "> успешно посадил один арбуз");
                }
            }
            else {
                bool flag = db.queryValue<bool>("select exists(select 1 from users where user_id=$1)", std::to_string(name.usr.id));
                if (!flag) {
                    event.reply("<@" + std::to_string(name.usr.id) + "> , твое семечко арбуза не проросло");
                    return;
                }
                auto current_time = time(NULL);
                auto last_grow = db.queryValue<time_t>("select last_grow from users where user_id=$1", std::to_string(name.usr.id));

                // std::cout << current_time << " " << last_grow << "\n";
                int64_t diff = current_time - last_grow;
                if (diff >= (int64_t)60 * (int64_t)60) {
                    event.reply("<@" + std::to_string(name.usr.id) + "> , твое семечко арбуза не проросло");
                }
                else {
                    event.reply("<@" + std::to_string(name.usr.id) + "> потерпи еще " + std::to_string(60 - diff / 60) + " минут до следующей посадки арбуза");
                }
            }
        } 
        else if (event.custom_id == "2") {
            event.reply("Отправь никнейм получателя");
            bool exists = db.queryValue<bool>("select exists(select 1 from post where user_id=$1)", std::to_string(name.usr.id));
            if (!exists) {
                db.exec("insert into post values($1, true)", std::to_string(name.usr.id));
            }
            else {
                db.exec("update post set is_active=true where user_id=$1", std::to_string(name.usr.id));
            }
        } 
        else if (event.custom_id == "3") {
            std::vector<User> stat;
            auto rows = db.query("select user_id, user_name, count_watermelons from users order by count_watermelons desc limit 10");
            for (const auto &row :rows) {
                stat.push_back({row["user_id"].as<std::string>(), row["user_name"].as<std::string>() , row["count_watermelons"].as<int>()});
            }

            dpp::embed embed = dpp::embed()
            .set_title("Статистка по арбузам!")
            .set_color(dpp::colors::magenta_pink);
            for (auto [id, name, count] : stat) {
                embed.add_field(name, std::to_string(count));
            }
            dpp::message msg(event.command.channel_id, embed);
            event.reply(msg);
        }
    });

    bot.on_message_create([&bot, &db](const dpp::message_create_t& event){
        std::string message_ = event.msg.content;
        std::regex regYes("(Д|д)(А|а)[\\s!?.,;:-]*$", std::regex_constants::icase);
        if (std::regex_search(message_, regYes)) {
            event.reply(wordsYes[rnd() % 3], true);
        }

        std::regex regNo("(Н|н)(Е|е)(Т|т)[\\s!?.,;:-]*$", std::regex_constants::icase);
        if (std::regex_search(message_, regNo)) {
            event.reply(wordsNo[rnd() % 3], true);
        }

        std::string author_id = std::to_string(event.msg.author.id);
        bool exists = db.queryValue<bool>("select exists(select 1 from post where user_id=$1)", author_id);
        if (exists) {
            bool flag = db.queryValue<bool>("select is_active from post where user_id=$1", author_id);
            if (flag) {
                std::string user_ping_id = message_.substr(2, 18);
                if (author_id == user_ping_id) {
                    db.exec("update post set is_active=false where user_id=$1", author_id);
                    event.reply("пользователь <@" + user_ping_id + "> успешно получил 1 арбуз");
                    return;
                }
                if (!is_id(user_ping_id)) {
                    event.reply("такого пользователя не существует");
                    db.exec("update post set is_active=false where user_id=$1", author_id);
                    return;
                }
                bool exists_id = db.queryValue<bool>("select exists(select 1 from users where user_id=$1)", user_ping_id);
                if (!exists_id) {
                    event.reply("такого пользователя не существует");
                }
                else {
                    auto row1 = db.queryRow("select count_watermelons from users where user_id=$1", author_id);
                    int64_t author_cnt_watermelons  = row1["count_watermelons"].as<int64_t>();

                    if (author_cnt_watermelons < 1) {
                        event.reply("у тебя недостаточно арбузов");
                    } 
                    else {
                        auto row2 = db.queryRow("select user_name, count_watermelons from users where user_id=$1", user_ping_id);
                        std::string user_ping_name = row2["user_name"].as<std::string>();
                        int64_t user_cnt_watermelons = row2["count_watermelons"].as<int64_t>();
                        db.exec("update users set count_watermelons=$1 where user_id=$2", user_cnt_watermelons + 1, user_ping_id);
                        db.exec("update users set count_watermelons=$1 where user_id=$2", author_cnt_watermelons - 1, author_id);
                        event.reply("пользователь <@" + user_ping_id + "> успешно получил 1 арбуз");
                    }
                }
                db.exec("update post set is_active=false where user_id=$1", author_id);
            }
        }

    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
            bot.global_command_create(dpp::slashcommand("start", "hello!", bot.me.id));
            bot.global_command_create(dpp::slashcommand("farm", "open your farm", bot.me.id));
        }  
    });
 
    bot.start(dpp::st_wait);
}