#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

#define MAX_BUFFER 512
#define MAX_LENGTH 200
#define MAX_DIR 50
#define MAX_NAME 20
#define MAX_THREAD 50

//User
typedef struct UserNodetag {
    char name[MAX_NAME];
    char dir[MAX_DIR];
    int UID;
    int GID;
    int year;
    int month;
    int wday;
    int day;
    int hour;
    int min;
    int sec;
    struct UserNodetag* LinkNode;
}UserNode;

//Tree
typedef struct TreeNodetag {
    char name[MAX_NAME];
    char type;
    int mode;
    int permission[9];
    int SIZE;
    int UID;
    int GID;
    int month;
    int day;
    int hour;
    int min;
    struct TreeNodetag* Parent;
    struct TreeNodetag* LeftChild;
    struct TreeNodetag* RightChild;
}TreeNode;


typedef struct DirectoryTreetag {
    TreeNode* root;
    TreeNode* current;
}DirectoryTree;

typedef struct Usertag {
    int topUID;
    int topGID;
    UserNode* head;
    UserNode* tail;
    UserNode* current;
}UserList;

typedef struct Threadtag {
    DirectoryTree* MultiDirTree;
    char* command;
    char* PathCopy;
    char* AddValues;
    DirectoryTree* NodeFileName;
} ThreadArg;



//stack using linked list
typedef struct StackNodetag {
    char name[MAX_NAME];
    struct StackNodetag* LinkNode;
}StackNode;


typedef struct Stacktag {
    StackNode* TopNode;
    int cnt;
}Stack;

int Mkdir(DirectoryTree* TreeDir, char* cmd);
int rm(DirectoryTree* TreeDir, char* cmd);
int cd(DirectoryTree* TreeDir, char* cmd);
int pwd(DirectoryTree* TreeDir, Stack* StackDir, char* cmd);
int chmod_(DirectoryTree* TreeDir, char* cmd);
int find(DirectoryTree* TreeDir, char* cmd);
void pasingCommand(DirectoryTree* TreeDir, char* cmd);
void printHedder(DirectoryTree* TreeDir, Stack* StackDir);

//grep
char* deleteSpace(char* s);
void grep(char* Word_Search, char* f_name);
void grep2(char* Word_Search, char* f_name);
void grep_v(char* findWord, char* findFile);
void grep_i(char* findWord, char* findFile);

//utility
int ModeToPermission(TreeNode* NodeDir);
void PermissionPrint(TreeNode* NodeDir);
void NodeRemove(TreeNode* NodeDir);
void DirRemove(TreeNode* NodeDir);
TreeNode* DirExistion(DirectoryTree* TreeDir, char* NameDir, char type);
char* TakeDir(char* PathDir);

//save & load
void TakePath(DirectoryTree* TreeDir, TreeNode* NodeDir, Stack* StackDir);
void NodeWrite(DirectoryTree* TreeDir, TreeNode* NodeDir, Stack* StackDir);
void SaveDir(DirectoryTree* TreeDir, Stack* StackDir);
int NodeRead(DirectoryTree* TreeDir, char* tmp);
DirectoryTree* DirLoad();

//mycp
int mycp(DirectoryTree* TreeDir, char* sName, char* oName);

//mkdir
DirectoryTree* InitializeTree();
int MakeDir(DirectoryTree* TreeDir, char* NameDir, char type);
void* threadmkdir(void* arg);
int MakeMultiDir(DirectoryTree* TreeDir, char* directoryName, char type, char* AddValues);

//rm
int RemoveDir(DirectoryTree* TreeDir, char* NameDir);

//cd
int CurrentMove(DirectoryTree* TreeDir, char* PathDir);
int PathMove(DirectoryTree* TreeDir, char* PathDir);

//pwd
int PrintPath(DirectoryTree* TreeDir, Stack* StackDir);

//ls
char* GetGID(TreeNode* dirNode);
char* GetUID(TreeNode* dirNode);
void ls(DirectoryTree* TreeDir);
void ls_a(DirectoryTree* TreeDir);
void ls_l(DirectoryTree* TreeDir);
void ls_al(DirectoryTree* TreeDir);

//cat
int Concatenate(DirectoryTree* TreeDir, char* fName, int o);

//chmod
int ModeConvers(DirectoryTree* TreeDir, int mode, char* NameDir);
void ModeConversAll(TreeNode* NodeDir, int mode);

//chown
int ChangeOwner(DirectoryTree* TreeDir, char* userName, char* dirName);
void ChangeOwnerAll(TreeNode* NodeDir, char* userName);
int chown_(DirectoryTree* dirTree, char* cmd);

//find
int DirRead(DirectoryTree* TreeDir, char* tmp, char* NameDir, int o);
void DirFind(DirectoryTree* TreeDir, char* NameDir, int o);

//user
void UserWrite(UserList* ListUser, UserNode* NodeUser);
void UserListSave(UserList* ListUser);
int UserRead(UserList* ListUser, char* tmp);
UserList* UserListLoad();
UserNode* UserExistion(UserList* ListUser, char* NameUser);

int OwnPermission(TreeNode* NodeDir, char o);
void Login(UserList* ListUser, DirectoryTree* TreeDir);

//stack
int EmptyTrue(Stack* StackDir);
Stack* StackInitializaion();
int Push(Stack* StackDir, char* NameDir);
char* Pop(Stack* StackDir);

//time
void TakeMonth(int i);
void TakeWeekDay(int i);

//global variable
extern DirectoryTree* Linux;
extern Stack* dStack;
extern UserList* UsersList;
extern FILE* Dir;
extern FILE* User;
extern time_t ltime;
extern struct tm* today;
