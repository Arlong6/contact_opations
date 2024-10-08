if run contac.c -> compile 指令
gcc -o contact_manager contac.c $(pkg-config --cflags --libs sqlite3)   


跑分開的code 可以輸入指令： 
make
./main
