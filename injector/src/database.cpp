#include "database.hpp"
#include "fault.hpp"
#include <iostream>
#include <sqlite3.h>

using namespace std;

void chaos::database::save_result(string& db_name, unique_ptr<fault::result>& result)
{
    sqlite3* db;

    if (sqlite3_open(db_name.c_str(), &db))
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db,
        "INSERT INTO result (iteration, hostname, exited, return_code, signaled, term_sig, fault_count, max_heap_size, max_stack_size, stdout, stderr, runtime, timeout) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        -1, &stmt, nullptr))
    {
        cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, result->iteration))
    {
        cerr << "Failed to bind iteration: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_text(stmt, 2, result->hostname, -1, SQLITE_TRANSIENT))
    {
        cerr << "Failed to bind hostname: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 3, result->exited))
    {
        cerr << "Failed to bind exited: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 4, result->return_code))
    {
        cerr << "Failed to bind return_code: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 5, result->signaled))
    {
        cerr << "Failed to bind signaled: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 6, result->term_sig))
    {
        cerr << "Failed to bind term_sig: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 7, result->fault_count))
    {
        cerr << "Failed to bind fault_count: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int64(stmt, 8, result->max_heap_size))
    {
        cerr << "Failed to bind max_heap_size: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int64(stmt, 9, result->max_stack_size))
    {
        cerr << "Failed to bind max_stack_size: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_blob(stmt, 10, result->stdout, strlen(result->stdout), SQLITE_TRANSIENT))
    {
        cerr << "Failed to bind stdout: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_blob(stmt, 11, result->stderr, strlen(result->stderr), SQLITE_TRANSIENT))
    {
        cerr << "Failed to bind stderr: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int64(stmt, 12, result->runtime))
    {
        cerr << "Failed to bind runtime: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    if (sqlite3_bind_int(stmt, 13, result->timeout))
    {
        cerr << "Failed to bind timeout: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        cerr << "Failed to commit: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
