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
