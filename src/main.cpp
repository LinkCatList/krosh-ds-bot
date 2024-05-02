#include <cctype>
#include <cstdint>
#include <dpp/colors.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <dpp/unicode_emoji.h>
#include <dpp/dpp.h>
#include <random>
#include <regex>
#include <chrono>
#include <string>
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

struct Watermelon {
    std::string watermelon_name;
    std::string watermelon_description;
    int64_t watermelon_count;
};

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
    bot.on_slashcommand([&db](const dpp::slashcommand_t& event) {
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
            bool exists = db.queryValue<bool>("select exists(select 1 from watermelons_count where user_id=$1)", std::to_string(event.command.usr.id));
            if (!exists) {
                db.exec("insert into users (user_id, user_name, last_grow, rank, cash) values ($1, $2, 0, 0, 100)", std::to_string(event.command.usr.id), 
                event.command.usr.global_name);
                db.exec("insert into watermelons_count (user_id, watermelon_id, watermelon_count) values ($1, '1', 1)", std::to_string(event.command.usr.id));
            }
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
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                    .set_label("Магазин")
                    .set_type(dpp::cot_button)
                    .set_emoji(dpp::unicode_emoji::toolbox)
                    .set_style(dpp::cos_primary)
                    .set_id("4")
                )
            );
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                    .set_label("Твой инвентарь")
                    .set_type(dpp::cot_button)
                    .set_emoji(dpp::unicode_emoji::hammer_and_pick)
                    .set_style(dpp::cos_primary)
                    .set_id("5")
                )
            );
            event.reply(msg);
        }
    });
    
    bot.on_button_click([&bot, &db](const dpp::button_click_t& event) {
        auto name = event.command;
        if (event.custom_id == "1") { // посадить арбуз
            auto rows = db.query("select watermelons.name, watermelons_count.watermelon_count from watermelons join" 
            " watermelons_count on watermelons.id = watermelons_count.watermelon_id where user_id=$1", std::to_string(name.usr.id));
            dpp::message msg(event.command.channel_id, "Доступные арбузы для посадки");
            for (const auto &row : rows) {
                if (row["watermelon_count"].as<int64_t>() > 0) { // вид арбуза можно посадить только если кол-во семян хотя бы 1
                    msg.add_component(                                             // вне зависимости от доступности
                        dpp::component().add_component(
                            dpp::component()
                            .set_label(row["name"].as<std::string>())
                            .set_type(dpp::cot_button)
                            .set_emoji(dpp::unicode_emoji::melon)
                            .set_style(dpp::cos_primary)
                            .set_id("grow " + row["name"].as<std::string>())
                        )
                    );
                }
            }
            event.reply(msg);
        } 
        else if (event.custom_id == "2") { // отправить арбуз
            auto rows = db.query("select watermelons.name, watermelons_count.watermelon_count from watermelons join" 
                " watermelons_count on watermelons.id = watermelons_count.watermelon_id where user_id=$1", std::to_string(name.usr.id));
                dpp::message msg(event.command.channel_id, "Доступные арбузы для отправки");
                for (const auto &row : rows) {
                    if (row["watermelon_count"].as<int64_t>() > 0) { // вид арбуза можно отправить только если его количество хотя бы 1
                        msg.add_component(                                             // вне зависимости от доступности
                            dpp::component().add_component(
                                dpp::component()
                                .set_label(row["name"].as<std::string>())
                                .set_type(dpp::cot_button)
                                .set_emoji(dpp::unicode_emoji::melon)
                                .set_style(dpp::cos_primary)
                                .set_id("send " + row["name"].as<std::string>())
                            )
                        );
                    }
                }
            event.reply(msg);
        } 
        else if (event.custom_id == "3") { // статистика по арбузам
            std::vector<User> stat;
            auto rows = db.query("select users.user_id, users.user_name, sum(watermelons_count.watermelon_count) "
                "as total_watermelons from users join watermelons_count on users.user_id=watermelons_count.user_id "
                "group by users.user_id, users.user_name order by total_watermelons desc limit 10");
            for (const auto &row :rows) {
                stat.push_back({row["user_id"].as<std::string>(), row["user_name"].as<std::string>(),
                    row["total_watermelons"].as<int>()});
            }

            dpp::embed embed = dpp::embed()
                .set_title("Статистка по арбузам!")
                .set_color(dpp::colors::magenta_pink);
            for (const auto &[id, name, count] : stat) {
                embed.add_field("`" + name + "`",  std::to_string(count));
            }
            dpp::message msg(event.command.channel_id, embed);
            event.reply(msg);
        }
        else if (event.custom_id == "4") { // магазин
            dpp::embed embed = dpp::embed()
            .set_title("Магазин")
            .set_color(dpp::colors::green_apple);
            dpp::message msg(event.command.channel_id, "Выбери раздел магазина");
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Арбузы")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::watermelon)
                        .set_style(dpp::cos_primary)
                        .set_id("7")
                )
            );
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Навоз")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::potato)
                        .set_style(dpp::cos_primary)
                        .set_id("8")
                )
            );
            event.reply(msg);
        }
        else if (event.custom_id == "5") {  // инвентарь пользователя
            auto rows = db.query("select watermelons.name, watermelons_count.watermelon_count"
                " from watermelons join watermelons_count on watermelons.id = watermelons_count.watermelon_id where user_id=$1",
                std::to_string(name.usr.id));

            dpp::embed embed = dpp::embed()
                .set_title("Твой инвентарь")
                .set_color(dpp::colors::magenta_pink);

            for (const auto &row : rows){
                embed.add_field(row["name"].as<std::string>(),
                    row["watermelon_count"].as<std::string>());
            }

            dpp::message msg(event.command.channel_id, embed);
            event.reply(msg);
        }
        else if (event.custom_id == "7") {  // раздел магазина - Арбузы
            auto rows = db.query("select name, description, cost from watermelons");
            dpp::embed embed = dpp::embed()
                .set_title("Магазин арбузов")
                .set_color(dpp::colors::green_apple);
            for (const auto &row : rows){
                embed.add_field("*" + row["name"].as<std::string>() + " `" + row["cost"].as<std::string>() + "🪙`" 
                + "*", "*" + row["description"].as<std::string>() + "*");
            }
            dpp::message msg(event.command.channel_id, embed);
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_type(dpp::cot_selectmenu)
                        .set_placeholder("Выбери арбуз")
                        .add_select_option(dpp::select_option("Арбуз от Ашота","1","арбуз, который продается в палатке таджиков").set_emoji(dpp::unicode_emoji::watermelon))
                        .add_select_option(dpp::select_option("Арбуз из магазина 24","2","арбуз, смотри, не отравись таким").set_emoji(dpp::unicode_emoji::watermelon))
                        .add_select_option(dpp::select_option("Арбуз из пятерочки","3","обычный арбуз, возможно вкусный").set_emoji(dpp::unicode_emoji::watermelon))
                        .add_select_option(dpp::select_option("Арбуз желтый","4","просто желтый арбуз").set_emoji(dpp::unicode_emoji::watermelon))
                        .add_select_option(dpp::select_option("Арбуз без косточек","5","удобно кушать, но сажать будет нечего").set_emoji(dpp::unicode_emoji::watermelon))
                        .add_select_option(dpp::select_option("Арбуз из Азбуки Вкуса","6","не все его могут себе позволить...").set_emoji(dpp::unicode_emoji::watermelon))
                        .set_id("select_watermelon")
                )
            );
            
            event.reply(msg);
        }
        else if (event.custom_id == "8") {  // раздел магазина - Навоз
            auto rows = db.query("select name, description, cost from manure");
            dpp::embed embed = dpp::embed()
                .set_title("Навоз")
                .set_color(dpp::colors::green_apple);
            for (const auto &row : rows){
                embed.add_field("*" + row["name"].as<std::string>() + " `" + row["cost"].as<std::string>() + "🪙`" 
                + "*", "*" + row["description"].as<std::string>() + "*");
            }
            dpp::message msg(event.command.channel_id, embed);
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_type(dpp::cot_selectmenu)
                        .set_placeholder("Выбери навоз")
                        .add_select_option(dpp::select_option("Навоз обычный","1","привезли из деревни Уфимовка").set_emoji(dpp::unicode_emoji::potato))
                        .add_select_option(dpp::select_option("Конский навоз жидкий","2","а минусы?").set_emoji(dpp::unicode_emoji::horse))
                        .add_select_option(dpp::select_option("Драконский навоз плюс-плюс","3","напрямую из-под дракона Clang").set_emoji(dpp::unicode_emoji::dragon))
                        .add_select_option(dpp::select_option("Навоз++","4","напрямую из-под антилопы GNU").set_emoji(dpp::unicode_emoji::blue_circle))
                        .add_select_option(dpp::select_option("Навоз из Азбуки Вкуса","5","его не может себе позволить даже Дима").set_emoji(dpp::unicode_emoji::diamonds))
                        .set_id("select_manure")
                )
            );
            event.reply(msg);
        }
        else if (event.custom_id.substr(0, 4) == "grow") { // запрос вида grow *название арбуза*
            int god = rnd();
            bool success = (god % 2 == 0) || (god % 3 == 0);
            if (success) {
                bool flag = db.queryValue<bool>("select exists(select 1 from users where user_id=$1)", std::to_string(name.usr.id));
                auto current_time = time(NULL);
                if (flag) {
                    auto last_grow = db.queryValue<int64_t>("select last_grow from users where user_id=$1", std::to_string(name.usr.id));
                    int64_t diff = current_time - last_grow;
                    if (diff >= (int64_t)60 * (int64_t)60) {
                        auto row = db.queryRow("select name, watermelons_count.watermelon_id, watermelons_count.watermelon_count from " 
                            "watermelons join watermelons_count on watermelons.id=watermelons_count.watermelon_id where watermelons_count.user_id=$1 "
                            "and name=$2", std::to_string(name.usr.id), event.custom_id.substr(5));
                        
                        db.exec("update watermelons_count set watermelon_count=$1 where user_id=$2 and watermelon_id=$3",
                            std::to_string(row["watermelon_count"].as<int64_t>() + 1), std::to_string(name.usr.id),
                            row["watermelon_id"].as<std::string>());
                        db.exec("update users set last_grow=$1 where user_id=$2", current_time, std::to_string(name.usr.id));
                        event.reply("<@" + std::to_string(name.usr.id) + "> успешно посадил один `" + row["name"].as<std::string>() + "`");
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

                int64_t diff = current_time - last_grow;
                if (diff >= (int64_t)60 * (int64_t)60) {
                    db.exec("update users set last_grow=$1 where user_id=$2", current_time, std::to_string(name.usr.id));
                    event.reply("<@" + std::to_string(name.usr.id) + "> , твое семечко арбуза не проросло");
                }
                else {
                    event.reply("<@" + std::to_string(name.usr.id) + "> потерпи еще " + std::to_string(60 - diff / 60) + " минут до следующей посадки арбуза");
                }
            }
        }
        else if (event.custom_id.substr(0, 4) == "send") {
            event.reply("Отправь никнейм получателя");
            bool exists = db.queryValue<bool>("select exists(select 1 from post where user_id=$1)", std::to_string(name.usr.id));
            if (!exists) {
                db.exec("insert into post values($1, true, $2)", std::to_string(name.usr.id), event.custom_id.substr(5));
            }
            else {
                db.exec("update post set is_active=true where user_id=$1", std::to_string(name.usr.id));
            }
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
            auto row = db.queryRow("select is_active, watermelon_type from post where user_id=$1", author_id);
            bool flag = row["is_active"].as<bool>();
            std::string watermelon_type = row["watermelon_type"].as<std::string>();
            if (flag) {
                std::string user_ping_id = message_.substr(2, 18);
                if (author_id == user_ping_id) {
                    db.exec("update post set is_active=false where user_id=$1", author_id);
                    event.reply("пользователь <@" + user_ping_id + "> успешно получил 1 " + watermelon_type);
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
                    auto row1 = db.queryRow("select watermelon_count from watermelons_count join watermelons "
                        "on watermelons_count.watermelon_id=watermelons.id where user_id=$1 and watermelons.name=$2", 
                        author_id, watermelon_type);
                    int64_t author_cnt_watermelons  = row1["watermelon_count"].as<int64_t>();

                    if (author_cnt_watermelons < 1) {
                        event.reply("у тебя недостаточно арбузов");
                    } 
                    else {
                        auto row2 = db.queryRow("select watermelon_count from watermelons_count join watermelons "
                            "on watermelons_count.watermelon_id=watermelons.id where user_id=$1 and watermelons.name=$2", 
                            user_ping_id, watermelon_type);
                        int64_t user_cnt_watermelons = row2["watermelon_count"].as<int64_t>();
                        db.exec("update watermelons_count set watermelon_count=$1 where user_id=$2", user_cnt_watermelons + 1, user_ping_id);
                        db.exec("update watermelons_count set watermelon_count=$1 where user_id=$2", author_cnt_watermelons - 1, author_id);
                        event.reply("пользователь <@" + user_ping_id + "> успешно получил 1 `" + watermelon_type + "`");
                    }
                }
                db.exec("update post set is_active=false where user_id=$1", author_id);
            }
        }

    });

    bot.on_select_click([&bot, &db](const dpp::select_click_t & event) {
        if (event.custom_id == "select_watermelon") {
            std::string watermelon_type = event.values[0];
            int64_t user_cash = db.queryValue<int64_t>("select cash from users where user_id=$1", std::to_string(event.command.usr.id));
            int64_t watermelon_cost = db.queryValue<int64_t>("select cost from watermelons where id=$1", watermelon_type);
            if (user_cash < watermelon_cost) {
                event.reply("Недостаточно кеша");
            }
            else {

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