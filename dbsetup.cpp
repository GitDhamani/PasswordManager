#include "dbsetup.h"
#include <QDebug>
#include <QMessageBox>
#include "sqlite3mc.h"

dbType storage = make_storage("Passwords.db",
    make_table("AllPasswords",
        make_column("Category", &Passwords::Category),
        make_column("Branch", &Passwords::Branch),
        make_column("Tag", &Passwords::Tag),
        make_column("URL", &Passwords::URL),
        make_column("Username", &Passwords::Username),
        make_column("Password", &Passwords::PW),
        make_column("Notes", &Passwords::Notes)
    ));

void SetupTable(std::string pw)
{
    // This is the catch connect for when the Schema is synced above. We install the key here.
    storage.on_open = [pw](sqlite3* db)
    {
        int rc = sqlite3_key(db, pw.c_str(), strlen(pw.c_str()));  //Insert Encryption Kwy
        storage.open_forever(); //Means that on_open() will be called once and the database will be kept open

        //Check to make sure Password is correct
        const char* sql = "SELECT * FROM sqlite_master";  //Dummy Data as a test
        char* errmsg;
        rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        if (rc != SQLITE_OK)
        {
            sqlite3_free(errmsg);
            sqlite3_close(db);
            qDebug() << "Password Incorrect.";
            //QMessageBox::information(0, "Error", "Password incorrect.");
            exit(1);
        }
    };

    // Create the table if it doesn't exist
    //This must come after the connect statement above.
    //This is actually the command which triggers the encryption key to be installed.
    storage.sync_schema();
}
