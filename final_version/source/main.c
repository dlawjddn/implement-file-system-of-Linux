#include "../include/osproject.h"

Stack* dStack;
FILE* Dir;
FILE* User;
time_t ltime;
struct tm* today;
DirectoryTree* Linux;
UserList* UsersList;

int main()
{
    char cmd[50];
    Linux = DirLoad();
    UsersList = UserListLoad();
    dStack = StackInitializaion();

    Login(UsersList, Linux);
    UserListSave(UsersList);
    printf("*--------------Welcome to the DGU OS project system--------------*\n");
    printf("*----------------Implement File System of Linux-----------------*\n");
    printf("*----------If you want to exit, typing \"exit\" and enter----------*\n");

    while (1) {
        printHedder(Linux, dStack);
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strlen(cmd) - 1] = '\0';
        pasingCommand(Linux, cmd);
    }
    return 0;
}