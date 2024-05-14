#ifndef DBSETUP_H
#define DBSETUP_H

#include "sqlite_orm.h" //ORM Wrapper
using namespace sqlite_orm;

struct Passwords {
    std::string Category;
    std::string Branch;
    std::string Tag;
    std::string URL;
    std::string Username;
    std::string PW;
    std::string Notes;
};

using dbType = decltype(
    make_storage("Passwords.db",
    make_table("AllPasswords",
        make_column("Category", &Passwords::Category),
        make_column("Branch", &Passwords::Branch),
        make_column("Tag", &Passwords::Tag),
        make_column("URL", &Passwords::URL),
        make_column("Username", &Passwords::Username),
        make_column("Password", &Passwords::PW),
        make_column("Notes", &Passwords::Notes)
    )));

#endif // DBSETUP_H
