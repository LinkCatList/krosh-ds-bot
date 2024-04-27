#include "Database.h"

#include <fmt/format.h>
#include <pqxx/pqxx>

Database::Database(pqxx::zview connectionUri)
    : _connection(std::make_unique<pqxx::connection>(connectionUri))
{}

Database::~Database() = default;

Database::Database(Database &&) noexcept = default;

Database & Database::operator=(Database &&) noexcept = default;

const pqxx::connection & Database::getConnection() const noexcept
{
    return *_connection;
}

pqxx::connection & Database::getConnection() noexcept
{
    return *_connection;
}

void Database::initSchema()
{
    pqxx::work tr(getConnection());

    tr.exec0(R"(
        create table if not exists users2 (
            user_id bigint primary key not null,
            user_name text,
            count_watermelons int not null default 0
        )
    )");

    // tr.exec0(R"(
    //     CREATE TABLE IF NOT EXISTS offenses (
    //         offense_id SERIAL PRIMARY KEY NOT NULL,
    //         user_id BIGINT NOT NULL,
    //         offense_type VARCHAR(40) NOT NULL,
    //         offense_created TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    //         offense_expires TIMESTAMP DEFAULT NULL NULL,
    //         bypass TIMESTAMP DEFAULT NULL NULL
    //     )
    // )");

    // tr.exec0(R"(
    //     CREATE INDEX IF NOT EXISTS offenses_user_id_index
    //         ON offenses (user_id)
    // )");

    tr.commit();
}

void Database::_checkRowsCount(size_t expectedRows, const pqxx::result & result)
{
    if (size_t actualRows = result.size(); actualRows != expectedRows) {
        throw pqxx::unexpected_rows(
            fmt::format("Expected {} row(s) of data from parameterised query, got {}.", expectedRows, actualRows)
        );
    }
}

void Database::_checkSingleOrNoRows(const pqxx::result & result)
{
    if (size_t rows = result.size(); rows > 1) {
        throw pqxx::unexpected_rows(fmt::format("Expected at most one row of data, got {}.", rows));
    }
}

void Database::_checkSingleColumn(const pqxx::row & row)
{
    if (size_t columns = row.size(); columns != 1) {
        throw pqxx::usage_error(fmt::format("Queried single value from result with {} columns.", columns));
    }
}
