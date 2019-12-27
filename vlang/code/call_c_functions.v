#flag -lsqlite3
#include "sqlite3.h"

struct C.sqlite3
struct C.sqlite3_stmt

fn C.sqlite3_column_text(voidptr, int) byteptr
fn C.sqlite3_column_int(voidptr, int) int
fn C.sqlite3_open()
fn C.sqlite3_step() int
fn C.sqlite3_prepare_v2()
fn C.sqlite3_finalize()

fn main() {
        path := 'users.db'
        db := &C.sqlite3(0) // a temporary hack meaning `sqlite3* db = 0`
        C.sqlite3_open(path.str, &db)

        query := 'select count(*) from users'
        stmt := &C.sqlite3_stmt(0)
        C.sqlite3_prepare_v2(db, query.str, - 1, &stmt, 0)
        C.sqlite3_step(stmt)
        nr_users := C.sqlite3_column_int(stmt, 0)
        C.sqlite3_finalize(stmt)
        println(nr_users)
}