#include <dpp/colors.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <dpp/unicode_emoji.h>
#include <dpp/dpp.h>
#include <random>
#include <regex>
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
int64_t isoToTime (std::string &s) {
    return ((s[11] - '0') * 10 + s[12] - '0') * 60 * 60 + ((s[14] - '0') * 10 + s[15] - '0') * 60 + (s[17] - '0') * 10 + s[18];
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
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                std::tm *ptm = std::localtime(&now_c);
                char current_time[32];
                std::strftime(current_time, 32, "%FT%T%z", ptm);
                if (flag) {
                    auto last_grow = db.queryValue<std::string>("select last_grow from users where user_id=$1", std::to_string(name.usr.id));
                    std::string current_timestr = "";
                    for(int i = 0; i < 32; i++) {
                        current_timestr.push_back(current_time[i]);
                    }
                    int64_t diff = abs(isoToTime(last_grow) - isoToTime(current_timestr));
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
                    db.exec("insert into users values($1, $2, $3, $4)", std::to_string(name.usr.id), name.usr.username, 1, current_time);
                }
            }
            else {
                
                event.reply("<@" + std::to_string(name.usr.id) + "> , твое семечко арбуза не проросло");
            }
        } 
        else if (event.custom_id == "2") {
            event.reply("Отправь никнейм получателя");
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
    bot.on_message_create([&bot](const dpp::message_create_t& event){
        std::string message_ = event.msg.content;
        std::regex regYes("(Д|д)(А|а)[\\s!?.,;:-]*$", std::regex_constants::icase);
        if (std::regex_search(message_, regYes)) {
            event.reply(wordsYes[rnd() % 3], true);
        }
        std::regex regNo("(Н|н)(Е|е)(Т|т)[\\s!?.,;:-]*$", std::regex_constants::icase);
        if (std::regex_search(message_, regNo)) {
            event.reply(wordsNo[rnd() % 3], true);
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