compile 指令
gcc -o contact_manager contac.c $(pkg-config --cflags --libs sqlite3)   