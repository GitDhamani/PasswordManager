# Encrypted SQLite Password Manager with Multi Factor Authentication

This is my encrypted password manager with multi-factor authentication. When the program first launches you are given a QR Token which you can scan through your phone's authenticator app. This will generate a one time password code that changes every 30 seconds.

Once you've chosen a password and entered in your OTP code, your account is set up and you're then able to start storing passwords. Even if someone manages to get hold of your password, they still cannot log in without an OTP code so this adds an extra layer of authentication.

The information is held inside an SQLite database, encrypted with AES256 Cipher Block Chaining using a key unique to your system. No passwords or login information is kept in plaintext, but are instead hashed using SHA1.

You can use the program to store as many passwords as you like, putting them in their own categories and branches. You can edit the names of the branches and passwords as you wish. You can also generate random secure passwords if you don't want to think of one yourself.

Whenever you need to retrieve a password, you can either find it by navigating through the tree structure, or just by clicking the find password menu option. You can then click the URL / Username / Password buttons to have those individual pieces of information copied to the clipboard respectively.

You can also change the OTP token seed if you have your own hardware token that you prefer to use.

![Thumb](https://github.com/GitDhamani/PasswordManager/assets/111576658/13cb55d7-8378-40aa-8d94-71bfafd27525)
