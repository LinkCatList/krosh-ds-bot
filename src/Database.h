#pragma once

#include <pqxx/result>
#include <pqxx/transaction>
#include <pqxx/zview>

#include <memory>
#include <optional>

class Database
{
public:
    explicit Database(pqxx::zview connectionUri);

    ~Database();

    Database(const Database &) = delete;
    Database(Database &&) noexcept;

    Database & operator=(const Database &) = delete;
    Database & operator=(Database &&) noexcept;

    [[nodiscard]] const pqxx::connection & getConnection() const noexcept;

    [[nodiscard]] pqxx::connection & getConnection() noexcept;

    // Generic query.
    template <typename... Args>
    [[nodiscard]] pqxx::result query(pqxx::zview sql, Args &&... args);

    // Expect `expectedRows` rows.
    template <typename... Args>
    [[nodiscard]] pqxx::result queryRows(size_t expectedRows, pqxx::zview sql, Args &&... args);

    // Expect no rows.
    template <typename... Args>
    pqxx::result exec(pqxx::zview sql, Args &&... args);

    // Expect a single row.
    template <typename... Args>
    [[nodiscard]] pqxx::row queryRow(pqxx::zview sql, Args &&... args);

    // Expect a single row or no rows.
    template <typename... Args>
    [[nodiscard]] std::optional<pqxx::row> queryRowOpt(pqxx::zview sql, Args &&... args);

    // Expect a single row with a single value.
    template <typename Type, typename... Args>
    [[nodiscard]] Type queryValue(pqxx::zview sql, Args &&... args);

    // Expect a single row with a single value or no rows.
    template <typename Type, typename... Args>
    [[nodiscard]] std::optional<Type> queryValueOpt(pqxx::zview sql, Args &&... args);

private:
    static void _checkRowsCount(size_t expectedRows, const pqxx::result & result);
    static void _checkSingleOrNoRows(const pqxx::result & result);
    static void _checkSingleColumn(const pqxx::row & row);

private:
    std::unique_ptr<pqxx::connection> _connection;
};

template <typename... Args>
pqxx::result Database::query(pqxx::zview sql, Args &&... args)
{
    pqxx::work tr(getConnection());
    pqxx::result result = tr.exec_params(sql, std::forward<Args>(args)...);
    tr.commit();
    return result;
}

template <typename... Args>
pqxx::result Database::queryRows(size_t expectedRows, pqxx::zview sql, Args &&... args)
{
    pqxx::result result = query(sql, std::forward<Args>(args)...);
    _checkRowsCount(expectedRows, result);
    return result;
}

template <typename... Args>
pqxx::result Database::exec(pqxx::zview sql, Args &&... args)
{
    return queryRows(0, sql, std::forward<Args>(args)...);
}

template <typename... Args>
pqxx::row Database::queryRow(pqxx::zview sql, Args &&... args)
{
    pqxx::result result = queryRows(1, sql, std::forward<Args>(args)...);
    return std::move(result).front();
}

template <typename... Args>
std::optional<pqxx::row> Database::queryRowOpt(pqxx::zview sql, Args &&... args)
{
    pqxx::result result = query(sql, std::forward<Args>(args)...);
    _checkSingleOrNoRows(result);
    if (result.empty()) {
        return std::nullopt;
    }
    return std::move(result).front();
}

template <typename Type, typename... Args>
Type Database::queryValue(pqxx::zview sql, Args &&... args)
{
    pqxx::row row = queryRow(sql, std::forward<Args>(args)...);
    _checkSingleColumn(row);
    return std::move(row).front().as<Type>();
}

template <typename Type, typename... Args>
std::optional<Type> Database::queryValueOpt(pqxx::zview sql, Args &&... args)
{
    std::optional<pqxx::row> row = queryRowOpt(sql, std::forward<Args>(args)...);
    return std::move(row).transform([](pqxx::row && row) {
        _checkSingleColumn(row);
        return std::move(row).front().as<Type>();
    });
}
