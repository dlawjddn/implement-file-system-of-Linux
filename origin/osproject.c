// aws 연결 : https://wooono.tistory.com/371 - OSManager adduser 방법
// 인바운드 규칙에 모든 팀원 탄력적 ip + 팀장 내 ip 추가
// ubuntu에 연결하는 방법 https://serina-the-best.tistory.com/16
// 무조건 ec2에 올리고 실행해보기... local 환경이랑 많아 다르더라..
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

time_t ltime;
struct tm* today;


int Mkdir(DirectoryTree* TreeDir, char* cmd);
int rm(DirectoryTree* TreeDir, char* cmd);
int cd(DirectoryTree* TreeDir, char* cmd);
int pwd(DirectoryTree* TreeDir, Stack* StackDir, char* cmd);
int chmod_(DirectoryTree* TreeDir, char* cmd);
int find(DirectoryTree* TreeDir, char* cmd);
void pasingCommand(DirectoryTree* TreeDir, char* cmd);
void printHedder(DirectoryTree* TreeDir, Stack* StackDir);

//grep
void grep(char* Word_Search, char* f_name);
void grep2(char* Word_Search, char* f_name);


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
int fileReadWrite(DirectoryTree* TreeDir, char* fName, int o);

//chmod
int ModeConvers(DirectoryTree* TreeDir, int mode, char* NameDir);
void ModeConversAll(TreeNode* NodeDir, int mode);

//chown
int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag);
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
DirectoryTree* Linux;
Stack* dStack;
UserList* UsersList;
FILE* Dir;
FILE* User;


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
        cmd[strlen(cmd) - 1] = '\0'; // null을 처리해서 문자열의 끝을 알려줌
        pasingCommand(Linux, cmd);
    }
    return 0;
}

//utility
int ModeToPermission(TreeNode* NodeDir)
{
    char buf[4];
    int tmp;
    int j;

    for (int i = 0; i < 9; i++)
        NodeDir->permission[i] = 0;

    sprintf(buf, "%d", NodeDir->mode);

    for (int i = 0; i < 3; i++) {
        tmp = buf[i] - '0';
        j = 2;

        while (tmp != 0) {
            NodeDir->permission[3 * i + j] = tmp % 2;
            tmp /= 2;
            j--;
        }
    }
    return 0;
}

void PermissionPrint(TreeNode* NodeDir)
{
    char rwx[4] = "rwx";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (NodeDir->permission[3 * i + j] == 1)
                printf("%c", rwx[j]);
            else
                printf("-");
        }
    }
}

void NodeRemove(TreeNode* NodeDir)
{
    free(NodeDir);
}

void DirRemove(TreeNode* NodeDir)
{
    if (NodeDir->RightChild != NULL) {
        DirRemove(NodeDir->RightChild);
    }
    if (NodeDir->LeftChild != NULL) {
        DirRemove(NodeDir->LeftChild);
    }

    NodeDir->LeftChild = NULL;
    NodeDir->RightChild = NULL;
    NodeRemove(NodeDir);
}

TreeNode* DirExistion(DirectoryTree* TreeDir, char* NameDir, char type)
{
    //variables
    TreeNode* returnNode = NULL;

    returnNode = TreeDir->current->LeftChild;

    while (returnNode != NULL) {
        if (strcmp(returnNode->name, NameDir) == 0 && returnNode->type == type)
            break;
        returnNode = returnNode->RightChild;
    }
    return returnNode;
}

char* TakeDir(char* PathDir)
{
    char* tmpPath = (char*)malloc(MAX_DIR);
    char* str = NULL;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];

    strncpy(tmp, PathDir, MAX_DIR);
    str = strtok(PathDir, "/");
    while (str != NULL) {
        strncpy(tmp2, str, MAX_DIR);
        str = strtok(NULL, "/");
    }
    strncpy(tmpPath, tmp, strlen(tmp) - strlen(tmp2) - 1);
    tmpPath[strlen(tmp) - strlen(tmp2) - 1] = '\0';

    return tmpPath;
}
//mycp
int mycp(DirectoryTree* TreeDir, char* sName, char* oName) {

    char buf[1024];
    int in, out;
    int nread;

    if (access(sName, F_OK) != 0) {
        printf("Not Exist Original File\n");
        return -1;
    }
    if (strcmp(sName, oName) == 0) {
        printf("tar.\n");
        return -1;
    }

    in = open(sName, O_RDONLY); // Original File
    out = open(oName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);//File to make
    nread = read(in, buf, sizeof(buf)); //읽은 만큼 nread가 증가함
    write(out, buf, nread);          //read만큼 write함

    MakeDir(TreeDir, oName, 'f');

    return 0;
}

//save & load
void TakePath(DirectoryTree* TreeDir, TreeNode* NodeDir, Stack* StackDir)
{
    TreeNode* tempNode = NULL;
    char tmp[MAX_DIR] = "";

    tempNode = NodeDir->Parent;

    if (tempNode == TreeDir->root) {
        strcpy(tmp, "/");
    }
    else {
        while (tempNode->Parent != NULL) {
            Push(StackDir, tempNode->name);
            tempNode = tempNode->Parent;
        }
        while (EmptyTrue(StackDir) == 0) {
            strcat(tmp, "/");
            strcat(tmp, Pop(StackDir));
        }
    }
    fprintf(Dir, " %s\n", tmp);
}

void NodeWrite(DirectoryTree* TreeDir, TreeNode* NodeDir, Stack* StackDir)
{
    fprintf(Dir, "%s %c %d ", NodeDir->name, NodeDir->type, NodeDir->mode);
    fprintf(Dir, "%d %d %d %d %d %d %d", NodeDir->SIZE, NodeDir->UID, NodeDir->GID, NodeDir->month, NodeDir->day, NodeDir->hour, NodeDir->min);

    if (NodeDir == TreeDir->root)
        fprintf(Dir, "\n");
    else
        TakePath(TreeDir, NodeDir, StackDir);

    if (NodeDir->RightChild != NULL) {
        NodeWrite(TreeDir, NodeDir->RightChild, StackDir);
    }
    if (NodeDir->LeftChild != NULL) {
        NodeWrite(TreeDir, NodeDir->LeftChild, StackDir);
    }
}

int MakeMultiDir(DirectoryTree* TreeDir, char* directoryName, char type, char* AddValues) {
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));
    TreeNode* tempNode = NULL;

    if (OwnPermission(TreeDir->current, 'w') != 0) {
        printf("mkdir: '%s' Can not create directory: Permission denied.\n", directoryName);
        free(NewNode);
        return -1;
    }
    if (strcmp(directoryName, ".") == 0 || strcmp(directoryName, "..") == 0) {
        printf("mkdir: '%s' Can not create directory.\n", directoryName);
        free(NewNode);
        return -1;
    }
    tempNode = DirExistion(TreeDir, directoryName, type);
    if (tempNode != NULL) {
        printf("%s", type == 'd' ? "mkdir" : "touch");
        printf(": '%s' Can not create directory: File exists.\n", directoryName);
        free(NewNode);
        return -1;
    }
    // get time
    time(&ltime);
    today = localtime(&ltime);

    // initialize NewNode
    NewNode->LeftChild = NULL;
    NewNode->RightChild = NULL;

    // set NewNode
    strncpy(NewNode->name, directoryName, MAX_NAME);
    if (type == 'f') {
        NewNode->type = 'f';
        // rw-r--r--
        NewNode->mode = 644;
        NewNode->SIZE = 0;
    }
    else if (directoryName[0] == '.') {
        NewNode->type = 'd';
        // rwxr-xr-x
        NewNode->mode = 755;
        NewNode->SIZE = 4096;
    }
    else {
        NewNode->type = 'd';
        // rwx------
        NewNode->mode = 700;
        NewNode->SIZE = 4096;
    }

    if (AddValues) {
        if (type == 'd') {
            if (strlen(AddValues) == 3) {
                NewNode->mode = atoi(AddValues);
            }
        }
    }

    ModeToPermission(NewNode);
    NewNode->UID = UsersList->current->UID;
    NewNode->GID = UsersList->current->GID;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->min = today->tm_min;
    NewNode->Parent = TreeDir->current;

    if (TreeDir->current->LeftChild == NULL) {
        TreeDir->current->LeftChild = NewNode;
    }
    else {
        tempNode = TreeDir->current->LeftChild;

        while (tempNode->RightChild != NULL) {
            tempNode = tempNode->RightChild;
        }
        tempNode->RightChild = NewNode;
    }

    return 0;

}

void* threadmkdir(void* arg) {
    ThreadArg* pArgThreading = ((ThreadArg*)arg);
    DirectoryTree* pdirectoryTree = pArgThreading->MultiDirTree;
    DirectoryTree* ppreTree;
    char* command = pArgThreading->command;
    char* filename = pArgThreading->AddValues;
    char* PathCopy = pArgThreading->PathCopy;

    TreeNode* tempNode = pdirectoryTree->current;
    char tmp[MAX_DIR];
    char pStr[MAX_DIR];
    char tmpStr[MAX_DIR];
    char directoryName[MAX_DIR];
    int directoryNameLength = 0;
    int isDirectoryExist;

    strncpy(tmp, command, MAX_DIR);

    if (strstr(command, "/") == NULL) {
        MakeMultiDir(pdirectoryTree, command, 'd', pArgThreading->AddValues);
    }
    else if (pArgThreading->AddValues && strlen(pArgThreading->AddValues) == 1) {
        int tmpLen = strlen(tmp), i = 0;
        if (tmp[0] == '/') {
            pdirectoryTree->current = pdirectoryTree->root;
            i = 1;
        }
        if (tmp[tmpLen - 1] == '/') {
            tmpLen -= 1;
        }
        for (; i < tmpLen; i++) {
            pStr[i] = tmp[i];
            pStr[i + 1] = 0;
            directoryName[directoryNameLength++] = tmp[i];
            if (tmp[i] == '/') {
                directoryName[--directoryNameLength] = 0;
                strncpy(tmpStr, pStr, i - 1);
                isDirectoryExist = CurrentMove(pdirectoryTree, directoryName);
                if (isDirectoryExist == -1) {
                    MakeMultiDir(pdirectoryTree, directoryName, 'd', NULL);
                    isDirectoryExist = CurrentMove(pdirectoryTree, directoryName);
                }
                directoryNameLength = 0;

            }

        }
        directoryName[directoryNameLength] = 0;
        MakeMultiDir(pdirectoryTree, directoryName, 'd', NULL);
        pdirectoryTree->current = tempNode;
    }
    else {
        char* pgetdirectory = TakeDir(command);
        isDirectoryExist = PathMove(pdirectoryTree, pgetdirectory);
        if (isDirectoryExist != 0) {
            printf(": %s : No such file or directory. \n", pgetdirectory);
        }
        else {
            char* str = strtok(tmp, "/");
            char* pdirectoryname;
            while (str != NULL) {
                pdirectoryname = str;
                str = strtok(NULL, "/");

            }
            MakeMultiDir(pdirectoryTree, pdirectoryname, 'd', pArgThreading->AddValues);
            pdirectoryTree->current = tempNode;
        }
    }
    pthread_exit(NULL);
}


void SaveDir(DirectoryTree* TreeDir, Stack* StackDir)
{
    Dir = fopen("Directory.txt", "w");
    NodeWrite(TreeDir, TreeDir->root, StackDir);
    fclose(Dir);
}

int NodeRead(DirectoryTree* TreeDir, char* tmp)
{
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));
    TreeNode* tempNode = NULL;
    char* str;

    NewNode->LeftChild = NULL;
    NewNode->RightChild = NULL;
    NewNode->Parent = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    NewNode->type = str[0];
    str = strtok(NULL, " ");
    NewNode->mode = atoi(str);
    ModeToPermission(NewNode);
    str = strtok(NULL, " ");
    NewNode->SIZE = atoi(str);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->min = atoi(str);

    str = strtok(NULL, " ");
    if (str != NULL) {
        str[strlen(str) - 1] = '\0';
        PathMove(TreeDir, str);
        NewNode->Parent = TreeDir->current;

        if (TreeDir->current->LeftChild == NULL) {
            TreeDir->current->LeftChild = NewNode;
        }
        else {
            tempNode = TreeDir->current->LeftChild;
            while (tempNode->RightChild != NULL)
                tempNode = tempNode->RightChild;
            tempNode->RightChild = NewNode;
        }
    }
    else {
        TreeDir->root = NewNode;
        TreeDir->current = TreeDir->root;
    }
    return 0;
}

DirectoryTree* DirLoad()
{
    DirectoryTree* TreeDir = (DirectoryTree*)malloc(sizeof(DirectoryTree));
    char tmp[MAX_LENGTH];
    Dir = fopen("Directory.txt", "r");

    while (fgets(tmp, MAX_LENGTH, Dir) != NULL) {
        NodeRead(TreeDir, tmp);
    }

    fclose(Dir);
    TreeDir->current = TreeDir->root;
    return TreeDir;
}

//mkdir
DirectoryTree* InitializeTree()
{
    DirectoryTree* TreeDir = (DirectoryTree*)malloc(sizeof(DirectoryTree));
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));

    time(&ltime);
    today = localtime(&ltime);
    strncpy(NewNode->name, "/", MAX_NAME);

    NewNode->type = 'd';
    NewNode->mode = 755;
    ModeToPermission(NewNode);
    NewNode->UID = UsersList->head->UID;
    NewNode->GID = UsersList->head->GID;
    NewNode->SIZE = 4096;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->min = today->tm_min;
    NewNode->Parent = NULL;
    NewNode->LeftChild = NULL;
    NewNode->RightChild = NULL;

    TreeDir->root = NewNode;
    TreeDir->current = TreeDir->root;

    return TreeDir;
}

int MakeDir(DirectoryTree* TreeDir, char* NameDir, char type)
{
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));
    TreeNode* tempNode = NULL;

    if (OwnPermission(TreeDir->current, 'w') != 0) {
        printf("mkdir: '%s' Can not create directory: Permission denied.\n", NameDir);
        free(NewNode);
        return -1;
    }
    if (strcmp(NameDir, ".") == 0 || strcmp(NameDir, "..") == 0) {
        printf("mkdir: '%s' Can not create directory\n", NameDir);
        free(NewNode);
        return -1;
    }
    tempNode = DirExistion(TreeDir, NameDir, type);
    if (tempNode != NULL && tempNode->type == 'd') {
        printf("mkdir: '%s' Can not create directory: File exists\n", NameDir);
        free(NewNode);
        return -1;
    }
    time(&ltime);
    today = localtime(&ltime);

    NewNode->LeftChild = NULL;
    NewNode->RightChild = NULL;

    strncpy(NewNode->name, NameDir, MAX_NAME);
    if (NameDir[0] == '.') {
        NewNode->type = 'd';
        //rwx------
        NewNode->mode = 700;
        NewNode->SIZE = 4096;
    }
    else if (type == 'd') {
        NewNode->type = 'd';
        //rwxr-xr-x
        NewNode->mode = 755;
        NewNode->SIZE = 4096;
    }
    else {
        NewNode->type = 'f';
        //rw-r--r--
        NewNode->mode = 644;
        NewNode->SIZE = 0;
    }
    ModeToPermission(NewNode);
    NewNode->UID = UsersList->current->UID;
    NewNode->GID = UsersList->current->GID;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->min = today->tm_min;
    NewNode->Parent = TreeDir->current;

    if (TreeDir->current->LeftChild == NULL) {
        TreeDir->current->LeftChild = NewNode;
    }
    else {
        tempNode = TreeDir->current->LeftChild;

        while (tempNode->RightChild != NULL) {
            tempNode = tempNode->RightChild;
        }
        tempNode->RightChild = NewNode;
    }

    return 0;
}

//rm
int RemoveDir(DirectoryTree* TreeDir, char* NameDir)
{
    TreeNode* DelNode = NULL;
    TreeNode* tempNode = NULL;
    TreeNode* prevNode = NULL;

    tempNode = TreeDir->current->LeftChild;

    if (tempNode == NULL) {
        printf("rm: Can not remove '%s': No such file or directory\n", NameDir);
        return -1;
    }

    if (strcmp(tempNode->name, NameDir) == 0) {
        TreeDir->current->LeftChild = tempNode->RightChild;
        DelNode = tempNode;
        if (DelNode->LeftChild != NULL)
            DirRemove(DelNode->LeftChild);
        NodeRemove(DelNode);
    }
    else {
        while (tempNode != NULL) {
            if (strcmp(tempNode->name, NameDir) == 0) {
                DelNode = tempNode;
                break;
            }
            prevNode = tempNode;
            tempNode = tempNode->RightChild;
        }
        if (DelNode != NULL) {
            prevNode->RightChild = DelNode->RightChild;

            if (DelNode->LeftChild != NULL)
                DirRemove(DelNode->LeftChild);
            NodeRemove(DelNode);
        }
        else {
            printf("rm: Can not remove '%s': No such file or directory\n", NameDir);
            return -1;
        }
    }
    return 0;
}

//cd
int CurrentMove(DirectoryTree* TreeDir, char* PathDir)
{
    TreeNode* tempNode = NULL;
    if (strcmp(PathDir, ".") == 0) {
    }
    else if (strcmp(PathDir, "..") == 0) {
        if (TreeDir->current != TreeDir->root) {
            TreeDir->current = TreeDir->current->Parent;
        }
    }
    else {

        tempNode = DirExistion(TreeDir, PathDir, 'd');
        if (tempNode != NULL) {
            TreeDir->current = tempNode;
        }
        else
            return -1;
    }
    return 0;
}

int PathMove(DirectoryTree* TreeDir, char* PathDir)
{
    TreeNode* tempNode = NULL;
    char tmpPath[MAX_DIR];
    char* str = NULL;
    int val = 0;

    strncpy(tmpPath, PathDir, MAX_DIR);
    tempNode = TreeDir->current;
    //if input is root
    if (strcmp(PathDir, "/") == 0) {
        TreeDir->current = TreeDir->root;
    }
    else {
        //if input is absolute path
        if (strncmp(PathDir, "/", 1) == 0) {
            if (strtok(PathDir, "/") == NULL) {
                return -1;
            }
            TreeDir->current = TreeDir->root;
        }
        //if input is relative path
        str = strtok(tmpPath, "/");
        while (str != NULL) {
            val = CurrentMove(TreeDir, str);
            //if input path doesn't exist
            if (val != 0) {
                TreeDir->current = tempNode;
                return -1;
            }
            str = strtok(NULL, "/");
        }
    }
    return 0;
}

//pwd
int PrintPath(DirectoryTree* TreeDir, Stack* StackDir)
{

    TreeNode* tempNode = NULL;
    tempNode = TreeDir->current;
    //if current directory is root
    if (tempNode == TreeDir->root) {
        printf("/");
    }
    else {
        //until current directory is root, repeat Push
        while (tempNode->Parent != NULL) {
            Push(StackDir, tempNode->name);
            tempNode = tempNode->Parent;
        }
        //until stack is empty, repeat Pop
        while (EmptyTrue(StackDir) == 0) {
            printf("/");
            printf("%s", Pop(StackDir));
        }
    }
    printf("\n");
    return 0;
}

//cat
int fileReadWrite(DirectoryTree* dirTree, char* fName, int o)
{
    UserNode* tempUser = NULL;
    TreeNode* tempNode = NULL;
    FILE* fp;
    char buf[MAX_BUFFER];
    char tmpName[MAX_NAME];
    char* str;
    int tmpSIZE = 0;
    int cnt = 1;

    //file read
    if(o != 0){
        if(o == 4){
            tempUser = UsersList->head;
            while(tempUser != NULL){
                printf("%s:x:%d:%d:%s:%s\n", tempUser->name, tempUser->UID, tempUser->GID, tempUser->name, tempUser->dir);
                tempUser = tempUser->LinkNode;
            }
            return 0;
        }
        tempNode = DirExistion(dirTree,fName, 'f');

        if(tempNode == NULL){
            return -1;
        }
        fp = fopen(fName, "r");

        while(feof(fp) == 0){
            fgets(buf, sizeof(buf), fp);
            if(feof(fp) != 0){
                break;
            }
            //w/ line number
            if(o == 2){
                if(buf[strlen(buf)-1] == '\n'){
                    printf("     %d ",cnt);
                    cnt++;
                }
            }
            else if(o == 3){
                if(buf[strlen(buf)-1] == '\n' && buf[0] != '\n'){
                    printf("     %d ",cnt);
                    cnt++;
                }
            }
            fputs(buf, stdout);
        }

        fclose(fp);
    }
    //file write
    else{
        fp = fopen(fName, "w");

	while(fgets(buf, sizeof(buf), stdin)){
            fputs(buf, fp);
            //get file size
            tmpSIZE += strlen(buf)-1;
            
        }
        rewind(stdin);
        fclose(fp);

        tempNode = DirExistion(dirTree, fName, 'f');
        //if exist
        if(tempNode != NULL){
            time(&ltime);
            today = localtime(&ltime);

            tempNode->month = today->tm_mon + 1;
            tempNode->day = today->tm_mday;
            tempNode->hour = today->tm_hour;
            tempNode->min = today->tm_min;
        }
        //if file doesn't exist
        else{
            MakeDir(dirTree, fName, 'f');
        }
        //write size
        tempNode = DirExistion(dirTree, fName, 'f');
        tempNode->SIZE = tmpSIZE;
    }
    return 0;
}
//chmod
int ModeConvers(DirectoryTree* TreeDir, int mode, char* NameDir)
{
    TreeNode* tempNode = NULL;
    TreeNode* tempNode2 = NULL;
    tempNode = DirExistion(TreeDir, NameDir, 'd');
    tempNode2 = DirExistion(TreeDir, NameDir, 'f');

    if (tempNode != NULL) {
        if (OwnPermission(tempNode, 'w') != 0) {
            printf("chmod: Can not modify file '%s': Permission denied\n", NameDir);
            return -1;
        }
        tempNode->mode = mode;
        ModeToPermission(tempNode);
    }
    else if (tempNode2 != NULL) {
        if (OwnPermission(tempNode2, 'w') != 0) {
            printf("chmod: Can not modify file '%s': Permission denied\n", NameDir);
            return -1;
        }
        tempNode2->mode = mode;
        ModeToPermission(tempNode2);
    }
    else {
        printf("chmod: Can not access to '%s: There is no such file or directory\n", NameDir);
        return -1;
    }
    return 0;
}
//chown
int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag)
{
    TreeNode* tempNode = NULL;
    TreeNode* tempNode2 = NULL;
    UserNode* tempUser = NULL;

    tempNode = DirExistion(dirTree, dirName, 'd');
    tempNode2 = DirExistion(dirTree, dirName, 'f');


    if(tempNode != NULL){ //디렉토리인 경우
        if(OwnPermission(tempNode, 'w') != 0){
            printf("chown: %s: Operation not permitted\n", dirName);
            return -1;
        }
        tempUser = UserExistion(UsersList, userName);
        if(tempUser != NULL){
            if (flag == 0)
                tempNode->UID = tempUser->UID;
            else
                tempNode->GID = tempUser->GID;
        }
        else{
            printf("chown: %s: illegal user name\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if(tempNode2 != NULL){ // 파일인 경우
        if(OwnPermission(tempNode2, 'w') != 0){
            printf("chown: %s: Operation not permitted\n", dirName);
            return -1;
        }
        tempUser = UserExistion(UsersList, userName);
        if(tempUser != NULL){
            if (flag == 0)
                tempNode2->UID = tempUser->UID;
            else
                tempNode2->GID = tempUser->GID;
        }
        else{
            printf("chown: %s: illegal user name\n", userName);
            printf("Try 'chown —help' for more information.\n");
            return -1;
        }
    }
    else{
        printf("chown: %s: No such file or directory\n", dirName);
        return -1;
    }

    return 0;
}
void ChangeOwnerAll(TreeNode* NodeDir, char* userName)
{
    UserNode* tempUser = NULL;

    tempUser = UserExistion(UsersList, userName);

    if(NodeDir->RightChild != NULL){
        ChangeOwnerAll(NodeDir->RightChild, userName);
    }
    if(NodeDir->LeftChild != NULL){
        ChangeOwnerAll(NodeDir->LeftChild, userName);
    }
    NodeDir->UID = tempUser->UID;
    NodeDir->GID = tempUser->GID;
}

int chown_(DirectoryTree* dirTree, char* cmd)      
{
    TreeNode* tempNode = NULL;
    UserNode* tempUser = NULL;
    char* str;
    char tmp[MAX_NAME];

    if(cmd == NULL){
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("사용법: chown [옵션]... [소유자]... 파일...\n");
            printf("  Change the owner and/or group of each FILE to OWNER and/or GROUP.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
        }
        return -1;
    }
    else{
        strncpy(tmp, cmd, MAX_NAME);
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else{
            //그룹
            if (strstr(tmp, ":") == NULL){
                ChangeOwner(dirTree, tmp, str, 0);
            }
            else{
                char tmp2[MAX_NAME];
                strncpy(tmp2, tmp, MAX_NAME);
                char *str2 = strtok(tmp, ":");
                if (str2 != NULL && strcmp(tmp, tmp2) != 0){
                    ChangeOwner(dirTree, str2, str, 0);
                    str2 = strtok(NULL, " ");
                    if (str2 != NULL)
                        ChangeOwner(dirTree, str2, str, 1);
                }
                else if (str2 != NULL && strcmp(tmp, tmp2) == 0)
                    ChangeOwner(dirTree, str2, str, 1);
            }
        }
    }
    return 0;
}
/*
int chown_(DirectoryTree* dirTree, char* cmd)
{
    TreeNode* tempNode = NULL;
    UserNode* tempUser = NULL;
    char* str;
    char tmp[MAX_NAME];

    if(cmd == NULL){
        printf("chown: put your command line\n");
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){
        if(strcmp(cmd, "-R") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("chown: wrong operator\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            tempUser = UserExistion(UsersList, str);
            if(tempUser != NULL){
                strncpy(tmp, str, MAX_NAME);
            }
            else{
                printf("chown: wrong user: '%s'\n", str);
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("chown: put your command line\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            tempNode = DirExistion(dirTree, str, 'd');
            if(tempNode != NULL){
                if(tempNode->LeftChild == NULL)
                    ChangeOwner(dirTree, tmp, str);
                else{
                    ChangeOwner(dirTree, tmp, str);
                    ChangeOwnerAll(tempNode->LeftChild, tmp);
                }
            }
            else{
                printf("chown: '%s': there is no file or directory..\n", str);
                return -1;
            }
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: chown [옵션]... [소유자]... 파일...\n");
            printf("  Change the owner and/or group of each FILE to OWNER and/or GROUP.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            str = strtok(cmd, "-");
            if(str == NULL){
                printf("chown: put your command line\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            else{
                printf("chown: invailable option -- 5'%s'\n", str);
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
        }
    }
    else{
        strncpy(tmp, cmd, MAX_NAME);
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("chown: put your command line\n");
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else{
            ChangeOwner(dirTree, tmp, str);
        }
    }
    return 0;
}
*/
void ModeConversAll(TreeNode* NodeDir, int mode)
{
    if (NodeDir->RightChild != NULL) {
        ModeConversAll(NodeDir->RightChild, mode);
    }
    if (NodeDir->LeftChild != NULL) {
        ModeConversAll(NodeDir->LeftChild, mode);
    }
    NodeDir->mode = mode;
    ModeToPermission(NodeDir);
}
//find
int DirRead(DirectoryTree* TreeDir, char* tmp, char* NameDir, int o)
{
    char* str;
    char str2[MAX_NAME];
    if (o == 0) {
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for (int i = 0; i < 10; i++) {
            str = strtok(NULL, " ");
        }
        if (str != NULL) {
            if (strstr(str2, NameDir) != NULL) {
                str[strlen(str) - 1] = '\0';
                if (strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    else {
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for (int i = 0; i < 10; i++) {
            str = strtok(NULL, " ");
        }
        if (str != NULL) {
            if (strstr(str, NameDir) != NULL) {
                str[strlen(str) - 1] = '\0';
                if (strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    return 0;
}

void DirFind(DirectoryTree* TreeDir, char* NameDir, int o)
{
    char tmp[MAX_LENGTH];
    Dir = fopen("Directory.txt", "r");

    while (fgets(tmp, MAX_LENGTH, Dir) != NULL) {
        DirRead(TreeDir, tmp, NameDir, o);
    }
    fclose(Dir);
}
//user
void UserWrite(UserList* ListUser, UserNode* NodeUser)
{
    time(&ltime);
    today = localtime(&ltime);

    ListUser->current->year = today->tm_year + 1900;
    ListUser->current->month = today->tm_mon + 1;
    ListUser->current->wday = today->tm_wday;
    ListUser->current->day = today->tm_mday;
    ListUser->current->hour = today->tm_hour;
    ListUser->current->min = today->tm_min;
    ListUser->current->sec = today->tm_sec;

    fprintf(User, "%s %d %d %d %d %d %d %d %d %d %s\n", NodeUser->name, NodeUser->UID, NodeUser->GID, NodeUser->year, NodeUser->month, NodeUser->wday, NodeUser->day, NodeUser->hour, NodeUser->min, NodeUser->sec, NodeUser->dir);

    if (NodeUser->LinkNode != NULL) {
        UserWrite(ListUser, NodeUser->LinkNode);
    }

}

void UserListSave(UserList* ListUser)
{
    User = fopen("User.txt", "w");
    UserWrite(ListUser, ListUser->head);
    fclose(Dir);
}

int UserRead(UserList* ListUser, char* tmp)
{
    UserNode* NewNode = (UserNode*)malloc(sizeof(UserNode));
    char* str;

    NewNode->LinkNode = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->year = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->wday = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->min = atoi(str);
    str = strtok(NULL, " ");
    NewNode->sec = atoi(str);
    str = strtok(NULL, " ");
    str[strlen(str) - 1] = '\0';
    strncpy(NewNode->dir, str, MAX_DIR);

    if (strcmp(NewNode->name, "root") == 0) {
        ListUser->head = NewNode;
        ListUser->tail = NewNode;
    }
    else {
        ListUser->tail->LinkNode = NewNode;
        ListUser->tail = NewNode;
    }
    return 0;
}

UserList* UserListLoad()
{
    UserList* ListUser = (UserList*)malloc(sizeof(UserList));
    char tmp[MAX_LENGTH];
    User = fopen("User.txt", "r");

    while (fgets(tmp, MAX_LENGTH, User) != NULL) {
        UserRead(ListUser, tmp);
    }

    fclose(User);
    ListUser->current = NULL;
    return ListUser;
}

UserNode* UserExistion(UserList* ListUser, char* NameUser)
{
    UserNode* returnUser = NULL;
    returnUser = ListUser->head;

    while (returnUser != NULL) {
        if (strcmp(returnUser->name, NameUser) == 0)
            break;
        returnUser = returnUser->LinkNode;
    }
    return returnUser;
}

int OwnPermission(TreeNode* NodeDir, char o)
{
    if (UsersList->current->UID == 0)
        return 0;

    if (UsersList->current->UID == NodeDir->UID) {
        if (o == 'r') {
            if (NodeDir->permission[0] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w') {
            if (NodeDir->permission[1] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x') {
            if (NodeDir->permission[2] == 0)
                return -1;
            else
                return 0;
        }
    }
    else if (UsersList->current->GID == NodeDir->GID) {
        if (o == 'r') {
            if (NodeDir->permission[3] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w') {
            if (NodeDir->permission[4] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x') {
            if (NodeDir->permission[5] == 0)
                return -1;
            else
                return 0;
        }
    }
    else {
        if (o == 'r') {
            if (NodeDir->permission[6] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w') {
            if (NodeDir->permission[7] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x') {
            if (NodeDir->permission[8] == 0)
                return -1;
            else
                return 0;
        }
    }
    return -1;
}

void Login(UserList* ListUser, DirectoryTree* TreeDir)
{
    UserNode* tempUser = NULL;
    char NameUser[MAX_NAME];
    char tmp[MAX_DIR];

    tempUser = ListUser->head;

    printf("Users: ");
    while (tempUser != NULL) {
        printf("%s ", tempUser->name);
        tempUser = tempUser->LinkNode;
    }
    printf("\n");

    while (1) {
        printf("Login as: ");
        fgets(NameUser, sizeof(NameUser), stdin);
        NameUser[strlen(NameUser) - 1] = '\0';
        if (strcmp(NameUser, "exit") == 0) {
            break;
        }
        tempUser = UserExistion(ListUser, NameUser);
        if (tempUser != NULL) {
            ListUser->current = tempUser;
            break;
        }
        printf("'%s' Invalid user\n", NameUser);
    }
    strcpy(tmp, ListUser->current->dir);
    PathMove(TreeDir, tmp);
}
//stack
//stack function
int EmptyTrue(Stack* StackDir)
{
    if (StackDir->TopNode == NULL) {
        return -1;
    }
    return 0;
}

Stack* StackInitializaion()
{
    Stack* returnStack = (Stack*)malloc(sizeof(Stack));

    if (returnStack == NULL) {
        printf("error occurred, returnStack.\n");
        return NULL;
    }
    //initialize Stack
    returnStack->TopNode = NULL;
    return returnStack;
}

int Push(Stack* StackDir, char* NameDir)
{
    StackNode* NodeDir = (StackNode*)malloc(sizeof(StackNode));

    if (StackDir == NULL) {
        printf("error occurred, StackDir.\n");
        return -1;
    }
    if (NodeDir == NULL) {
        printf("error occurred, NodeDir.\n");
        return -1;
    }
    //set NodeDir
    strncpy(NodeDir->name, NameDir, MAX_NAME);
    NodeDir->LinkNode = StackDir->TopNode;
    //set StackDir
    StackDir->TopNode = NodeDir;
    return 0;
}

char* Pop(Stack* StackDir)
{
    StackNode* returnNode = NULL;
    if (StackDir == NULL) {
        printf("error occurred, StackDir.\n");
        return NULL;
    }
    if (EmptyTrue(StackDir) == -1) {
        //printf("Stack Empty.\n");
        return NULL;
    }
    returnNode = StackDir->TopNode;
    StackDir->TopNode = returnNode->LinkNode;

    return returnNode->name;
}

//pasingCommand command
int Mkdir(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* tempNode = NULL;
    char* str;
    int val;
    int Modetmp;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    if (cmd == NULL) {
        printf("mkdir: Invalid option\n");
        printf("Try 'mkdir --help' for more information.\n");
        return -1;
    }
    int cnt_t = 0;
    pthread_t cmd_t[MAX_THREAD];
    ThreadArg pArgThreading[MAX_THREAD];

    tempNode = TreeDir->current;
    if (cmd[0] == '-') {
        if (strcmp(cmd, "-p") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("mkdir: Invalid option\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                printf("%s : str \n", str);
                pArgThreading[cnt_t].MultiDirTree = TreeDir;
                pArgThreading[cnt_t].AddValues = ".";
                pArgThreading[cnt_t++].command = str;
                str = strtok(NULL, " ");
            }
        }
        else if (strcmp(cmd, "-m") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("mkdir: Invalid option\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3) {
                Modetmp = atoi(str);
                char* modeStr = str;

                str = strtok(NULL, " ");
                if (str == NULL) {
                    printf("mkdir: Invalid option\n");
                    printf("Try 'mkdir --help' for more information.\n");
                    return -1;
                }
                while (str != NULL) {
                    pArgThreading[cnt_t].MultiDirTree = TreeDir;
                    pArgThreading[cnt_t].AddValues = modeStr;
                    pArgThreading[cnt_t].command = str;
                    str = strtok(NULL, " ");
                }
            }
            else {
                printf("code error, Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Try --help' for more information.\n");
            return -1;
        }


        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("mkdir: Invalid option\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            else {
                printf("mkdir: Unrecognized option -- '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        str = strtok(NULL, " ");
        pArgThreading[cnt_t].MultiDirTree = TreeDir;
        pArgThreading[cnt_t].AddValues = NULL;
        pArgThreading[cnt_t++].command = cmd;

        while (str != NULL) {
            pArgThreading[cnt_t].MultiDirTree = TreeDir;
            pArgThreading[cnt_t].AddValues = NULL;
            pArgThreading[cnt_t++].command = str;
            str = strtok(NULL, " ");
        }
    }

    for (int i = 0; i < cnt_t; i++) {
        pthread_create(&cmd_t[i], NULL, threadmkdir, (void*)&pArgThreading[i]);
        pthread_join(cmd_t[i], NULL);
    }
    return 0;
}

int rm(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* NodeCurrent = NULL;
    TreeNode* tempNode = NULL;
    TreeNode* tempNode2 = NULL;
    char* str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val;

    if (cmd == NULL) {
        printf("rm: Invalid option\n");
        printf("Try 'rm --help' for more information.\n");
        return -1;
    }
    NodeCurrent = TreeDir->current;
    if (cmd[0] == '-') {
        if (strcmp(cmd, "-r") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("rm: Invalid option\n");
                printf("Try 'rm --help' for more information.\n");

                return -1;
            }
            strncpy(tmp, str, MAX_DIR);
            if (strstr(str, "/") == NULL) {
                tempNode = DirExistion(TreeDir, str, 'f');
                tempNode = (DirExistion(TreeDir, str, 'd') == NULL) ? tempNode : DirExistion(TreeDir, str, 'd');
                if (tempNode == NULL) {
                    printf("rm: Can not remove '%s': No such file or directory.\n", str);
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        printf("rm: failed to remove '%s'Can not remove directory or file: Permission denied.\n", str);
                        return -1;
                    }
                    RemoveDir(TreeDir, str);
                }
            }
            else {
                strncpy(tmp2, TakeDir(str), MAX_DIR);
                val = PathMove(TreeDir, tmp2);
                if (val != 0) {
                    printf("rm: No such file or directory '%s'\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                tempNode = DirExistion(TreeDir, tmp3, 'f');
                tempNode = (DirExistion(TreeDir, tmp3, 'd') == NULL) ? tempNode : DirExistion(TreeDir, tmp3, 'd');
                if (tempNode == NULL) {
                    printf("rm: Can not remove '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        printf("rm: failed to remove '%s' Can not remove directory or file: Permission denied.\n", tmp3);
                        TreeDir->current = NodeCurrent;
                        return -1;
                    }
                    RemoveDir(TreeDir, tmp3);
                }
                TreeDir->current = NodeCurrent;
            }
        }
        else if (strcmp(cmd, "-f") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                return -1;
            }
            strncpy(tmp, str, MAX_DIR);
            if (strstr(str, "/") == NULL) {
                tempNode = DirExistion(TreeDir, str, 'f');
                if (tempNode == NULL) {
                    printf("error: %s is not a file, %s is a directory\n", str, str);
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        return -1;
                    }
                    RemoveDir(TreeDir, str);
                }
            }
            else {
                strncpy(tmp2, TakeDir(str), MAX_DIR);
                val = PathMove(TreeDir, tmp2);
                if (val != 0) {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                tempNode = DirExistion(TreeDir, tmp3, 'f');
                if (tempNode == NULL) {
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        TreeDir->current = NodeCurrent;
                        return -1;
                    }
                    RemoveDir(TreeDir, tmp3);
                }
                TreeDir->current = NodeCurrent;
            }
        }
        else if (strcmp(cmd, "-rf") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                return -1;
            }
            strncpy(tmp, str, MAX_DIR);
            if (strstr(str, "/") == NULL) {
                tempNode = DirExistion(TreeDir, str, 'f');
                tempNode = (DirExistion(TreeDir, str, 'd') == NULL) ? tempNode : DirExistion(TreeDir, str, 'd');
                if (tempNode == NULL) {
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        return -1;
                    }
                    RemoveDir(TreeDir, str);
                }
            }
            else {
                strncpy(tmp2, TakeDir(str), MAX_DIR);
                val = PathMove(TreeDir, tmp2);
                if (val != 0) {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                tempNode = DirExistion(TreeDir, tmp3, 'f');
                tempNode = (DirExistion(TreeDir, tmp3, 'd') == NULL) ? tempNode : DirExistion(TreeDir, tmp3, 'd');
                if (tempNode == NULL) {
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                        TreeDir->current = NodeCurrent;
                        return -1;
                    }
                    RemoveDir(TreeDir, tmp3);
                }
                TreeDir->current = NodeCurrent;
            }
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Manual: rm [Option]... [<File>]...\n");
            printf("  Remove (unlink) the FILE(s).\n\n");
            printf("  Options:\n");
            printf("    -f, --force    \t ignore nonexistent files and arguments, never prompt\n");
            printf("    -r, --recursive\t remove directories and their contents recursively\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("rm: Invalid option\n");
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
            else {
                printf("rm: Unrecognized option -- '%s'\n", str);
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        strncpy(tmp, cmd, MAX_DIR);
        if (strstr(cmd, "/") == NULL) {
            tempNode = DirExistion(TreeDir, cmd, 'f');
            tempNode2 = DirExistion(TreeDir, cmd, 'd');

            if (tempNode2 != NULL) {
                printf("rm: Can not remove '%s': Is a directory\n", cmd);
                return -1;
            }
            if (tempNode == NULL) {
                printf("rm: Can not remove '%s': No such file or directory.\n", cmd);
                return -1;
            }
            else {
                if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                    printf("rm: Can not remove '%s': Permission denied\n", cmd);
                    return -1;
                }
                RemoveDir(TreeDir, cmd);
            }
        }
        else {
            strncpy(tmp2, TakeDir(cmd), MAX_DIR);
            val = PathMove(TreeDir, tmp2);
            if (val != 0) {
                printf("rm: Can not remove '%s': No such file or directory.\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tempNode = DirExistion(TreeDir, tmp3, 'f');
            tempNode2 = DirExistion(TreeDir, tmp3, 'd');

            if (tempNode2 != NULL) {
                printf("rm: Can not remove '%s': Is a directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            if (tempNode == NULL) {
                printf("rm: Can not remove '%s' No such file or directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tempNode, 'w') != 0) {
                    printf("rm: Can not remove '%s' Permission denied\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                RemoveDir(TreeDir, tmp3);
            }
            TreeDir->current = NodeCurrent;
        }
    }
    return 0;
}

int cd(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* tempNode = NULL;
    char* str = NULL;
    char tmp[MAX_DIR];
    int val;

    if (cmd == NULL) {
        strcpy(tmp, UsersList->current->dir);
        PathMove(TreeDir, tmp);
    }
    else if (cmd[0] == '-') {
        if (strcmp(cmd, "--help") == 0) {
            printf("Manual: cd directory...\n");
            printf("  Change the shell working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("cd: Invalid option\n");
                printf("Try 'cd --help' for more information.\n");
                return -1;
            }
            else {
                printf("cd: Unrecognized option -- '%s'\n", str);
                printf("Try 'cd --help' for more information.\n");

                return -1;
            }
        }
    }
    else {
        tempNode = DirExistion(TreeDir, cmd, 'd');
        if (tempNode != NULL) {
            if (OwnPermission(tempNode, 'r') != 0) {
                printf("-bash: cd: '%s': Permission denied\n", cmd);
                return -1;
            }
        }
        tempNode = DirExistion(TreeDir, cmd, 'f');
        if (tempNode != NULL) {
            printf("-bash: failed to remoeve cd: '%s': Not a directory\n", cmd);
            return -1;
        }
        val = PathMove(TreeDir, cmd);
        if (val != 0)
            printf("-bash: cd: No such file or directory '%s'\n", cmd);
    }
    return 0;
}

int pwd(DirectoryTree* TreeDir, Stack* StackDir, char* cmd)
{
    char* str = NULL;
    if (cmd == NULL) {
        PrintPath(TreeDir, StackDir);
    }
    else if (cmd[0] == '-') {
        if (strcmp(cmd, "--help") == 0) {
            printf("Manual: pwd\n");
            printf("  Print the name of the current working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("pwd: Invalid option\n");
                printf("Try 'pwd --help' for more information.\n");
                return -1;
            }
            else {
                printf("pwd: Unrecognized option -- '%s'\n", str);
                printf("Try 'pwd --help' for more information.\n");
                return -1;
            }
        }
    }

    return 0;
}

char* GetUID(TreeNode* dirNode)
{
    UserNode* tempNode = NULL;

    tempNode = UsersList->head;
    while(tempNode != NULL){
        if(tempNode->UID == dirNode->UID)
            break;
        tempNode = tempNode->LinkNode;
    }
    return tempNode->name;
}

char* GetGID(TreeNode* dirNode)
{
    UserNode* tempNode = NULL;

    tempNode = UsersList->head;
    while(tempNode != NULL){
        if(tempNode->GID == dirNode->GID)
            break;
        tempNode = tempNode->LinkNode;
    }
    return tempNode->name;
}

void ls(DirectoryTree* TreeDir) {
    int count = 0;
    TreeNode* tempNode = TreeDir->current;
    if (tempNode->LeftChild == NULL)
        printf("directory empty...\n");
    else {
        tempNode = tempNode->LeftChild;
        while (tempNode->RightChild != NULL) { // 마지막 파일이 출력 X

            if(tempNode->name[0] != '.'){ // 파일 이름 시작이 '.' 경우 -> 숨김파일
                if (strlen(tempNode->name) < 8)
                    printf("%s\t\t", tempNode->name);
                else
                    printf("%s\t", tempNode->name);
                count++;
            }
            tempNode = tempNode->RightChild;

            if (count % 5 == 0)
                printf("\n");

        }
        if(tempNode->name[0] != '.') printf("%s\t\n", tempNode->name); // 마지막 파일에 대한 출력
    }
}

void ls_a(DirectoryTree* TreeDir) {
    int count = 1;
    TreeNode* tempNode = TreeDir->current;
    if (tempNode->LeftChild == NULL) {
        printf(".\t\t..\n");
    }
    else {
        printf(".\t\t..\t\t");
        count = count + 2;
        tempNode = tempNode->LeftChild;
        while (tempNode->RightChild != NULL) {
            if (strlen(tempNode->name) < 8)
                printf("%s\t\t", tempNode->name);
            else
                printf("%s\t", tempNode->name);
            tempNode = tempNode->RightChild;
            if (count % 5 == 0)
                printf("\n");
            count++;
        }
        printf("%s\t\n", tempNode->name);
    }
}
void ls_l(DirectoryTree* TreeDir) {
    time_t timer = time(NULL);
    TreeNode* tempNode = TreeDir->current;
    TreeNode* tempNode2 = tempNode->LeftChild;
    int cnt=0;

    if(tempNode2 == NULL) cnt = 2;
    else{
        if(tempNode2->type == 'd') cnt = 3;
        else cnt = 2;

        while(tempNode2->RightChild != NULL){
            tempNode2 = tempNode2->RightChild;
            if(tempNode2->type == 'd') cnt = cnt + 1;
        }
    }
    if (tempNode->LeftChild == NULL)
        printf("directory empty\n");
    else {
        tempNode = tempNode->LeftChild;
        while (tempNode->RightChild != NULL) {
            if(tempNode->name[0] != '.') { // 숨김파일 처리
                printf("%c", tempNode->type); // 디렉토리 or 파일
                PermissionPrint(tempNode); // 권한 출력
                printf("%3d   ", cnt); // 안에 소속된 파일 개수
                printf(" %-5s%-5s", GetUID(tempNode), GetGID(tempNode)); // 유저아이디, 그룹 아이디
                printf(" %5d", tempNode->SIZE); // 파일 사이즈
                printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min); // 생성 날짜
                printf("%s\n", tempNode->name); // 파일명
            }
            tempNode = tempNode->RightChild;
        }
        printf("%c", tempNode->type);
        PermissionPrint(tempNode);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tempNode), GetGID(tempNode));
        printf(" %5d", tempNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
        printf("%s\n", tempNode->name);
    }
}
void ls_al(DirectoryTree* TreeDir) {
    time_t timer = time(NULL);
    TreeNode* tempNode = TreeDir->current;
    TreeNode* tempNode2 = tempNode->LeftChild;
    int cnt=0;

    if(tempNode2 == NULL) cnt = 2;
    else{
        if(tempNode2->type == 'd') cnt = 3;
        else cnt = 2;

        while(tempNode2->RightChild != NULL){
            tempNode2 = tempNode2->RightChild;
            if(tempNode2->type == 'd') cnt = cnt + 1;
        }
    }
    if (tempNode->LeftChild == NULL) {
        //.
        printf("%c", TreeDir->current->type);
        PermissionPrint(TreeDir->current);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tempNode), GetGID(tempNode));
        printf(" %5d", tempNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
        printf(".\n");
        //..
        if (strcmp(TreeDir->current->name, "/") == 0) {
            printf("%c", TreeDir->current->type);
            PermissionPrint(TreeDir->current);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tempNode), GetGID(tempNode));
            printf(" %5d", tempNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
            printf("..\n");
        }
        else {
            printf("%c", tempNode->Parent->type);
            PermissionPrint(tempNode->Parent);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tempNode->Parent), GetGID(tempNode->Parent));
            printf(" %5d", tempNode->Parent->SIZE);
            printf(" %d월 %2d %02d:%02d ", tempNode->Parent->month, tempNode->Parent->day, tempNode->Parent->hour, tempNode->Parent->min);
            printf("..\n");
        }
    }
    else {
        //.
        printf("%c", TreeDir->current->type);
        PermissionPrint(TreeDir->current);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tempNode), GetGID(tempNode));
        printf(" %5d", tempNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
        printf(".\n");
        //..
        if (strcmp(TreeDir->current->name, "/") == 0) {
            printf("%c", TreeDir->current->type);
            PermissionPrint(TreeDir->current);
            printf("%3d ", cnt);
            printf(" %-5s%-5s %5d", GetUID(tempNode), GetGID(tempNode), tempNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
            printf("..\n");
        }
        else {
            printf("%c", tempNode->Parent->type);
            PermissionPrint(tempNode->Parent);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tempNode->Parent), GetGID(tempNode->Parent));
            printf(" %5d", tempNode->Parent->SIZE);
            printf(" %d월 %2d %02d:%02d ", tempNode->Parent->month, tempNode->Parent->day, tempNode->Parent->hour, tempNode->Parent->min);
            printf("..\n");
        }


        tempNode = tempNode->LeftChild;
        while (tempNode->RightChild != NULL) {
            printf("%c", tempNode->type);
            PermissionPrint(tempNode);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s %5d", GetUID(tempNode), GetGID(tempNode), tempNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
            printf("%s\n", tempNode->name);
            tempNode = tempNode->RightChild;
        }
        printf("%c", tempNode->type);
        PermissionPrint(tempNode);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s %5d", GetUID(tempNode), GetGID(tempNode), tempNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tempNode->month, tempNode->day, tempNode->hour, tempNode->min);
        printf("%s\n", tempNode->name);
    }
}
int cat(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* NodeCurrent = NULL;
    TreeNode* tempNode = NULL;
    TreeNode* tempNode2 = NULL;
    char* str;
    char *tmp = (char* )malloc(sizeof(char)*strlen(cmd));
    //char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val;

    if (cmd == NULL) {
        printf("cat: Invalid option\n");
        return -1;
    }
    NodeCurrent = TreeDir->current;

    if (strcmp(cmd, ">") == 0) {  // when use option
        str = strtok(NULL, " ");
        if (str == NULL) {
            printf("cat: Invalid option?\n");
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
        strncpy(tmp, str, MAX_DIR);
        if (strstr(str, "/") == NULL) {
            if (OwnPermission(TreeDir->current, 'w') != 0) { // check write permission
                printf("cat: Can not create file '%s': No permission.\n", TreeDir->current->name);
                return -1;
            }
            tempNode = DirExistion(TreeDir, str, 'd'); // if write permission-> TsExist excute
            if (tempNode != NULL) {
                printf("cat: '%s':Is a directory.\n", str);
                return -1;
            }
            else {
                fileReadWrite(TreeDir, str, 0);
            }
        }
        else {
            strncpy(tmp2, TakeDir(str), MAX_DIR);
            val = PathMove(TreeDir, tmp2);
            if (val != 0) {
                printf("cat: '%s': No such file or directory.\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            if (OwnPermission(TreeDir->current, 'w') != 0) {
                printf("cat: Can not create file '%s': Permission denied\n", TreeDir->current->name);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            tempNode = DirExistion(TreeDir, tmp3, 'd');
            if (tempNode != NULL) {
                printf("cat: '%s': Is a directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                fileReadWrite(TreeDir, tmp3, 0);
            }
            TreeDir->current = NodeCurrent;
        }
        return 0;
    }
    else if (cmd[0] == '-') {
        if (strcmp(cmd, "-n") == 0) {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIR);
            if (strstr(str, "/") == NULL) {
                if (OwnPermission(TreeDir->current, 'w') != 0) {
                    printf("cat: Can not create file '%s': Permission denied.\n", TreeDir->current->name);
                    return -1;
                }
                tempNode = DirExistion(TreeDir, str, 'd');
                tempNode2 = DirExistion(TreeDir, str, 'f');

                if (tempNode == NULL && tempNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", str);
                    return -1;
                }
                else if (tempNode != NULL && tempNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", str);
                    return -1;
                }
                else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tempNode2->name);
                    return -1;
                }
                else {
                    fileReadWrite(TreeDir, str, 2);
                }
            }
            else {
                strncpy(tmp2, TakeDir(str), MAX_DIR);
                val = PathMove(TreeDir, tmp2);
                if (val != 0) {
                    printf("cat: '%s': No such file or directory.\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                tempNode = DirExistion(TreeDir, tmp3, 'd');
                tempNode2 = DirExistion(TreeDir, tmp3, 'f');

                if (tempNode == NULL && tempNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tempNode != NULL && tempNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tempNode2->name);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    fileReadWrite(TreeDir, tmp3, 2);
                }
                TreeDir->current = NodeCurrent;
            }
        }
        else if (strcmp(cmd, "-b") == 0) {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIR);
            if (strstr(str, "/") == NULL) {
                if (OwnPermission(TreeDir->current, 'w') != 0) {
                    printf("cat: Can not create file '%s': Permission denied\n", TreeDir->current->name);
                    return -1;
                }
                tempNode = DirExistion(TreeDir, str, 'd');
                tempNode2 = DirExistion(TreeDir, str, 'f');
                if (tempNode == NULL && tempNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", str);
                    return -1;
                }
                else if (tempNode != NULL && tempNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", str);
                    return -1;
                }
                else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tempNode2->name);
                    return -1;
                }
                else {
                    fileReadWrite(TreeDir, str, 3);
                }
            }
            else {
                strncpy(tmp2, TakeDir(str), MAX_DIR);
                val = PathMove(TreeDir, tmp2);
                if (val != 0) {
                    printf("cat: '%s': No such file or directory.\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                tempNode = DirExistion(TreeDir, tmp3, 'd');
                tempNode2 = DirExistion(TreeDir, tmp3, 'f');
                if (tempNode == NULL && tempNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tempNode != NULL && tempNode2 == NULL) {
                    printf("cat: '%s': Is a direcotry\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tempNode2->name);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    fileReadWrite(TreeDir, tmp3, 3);
                }
                TreeDir->current = NodeCurrent;
            }
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Manual: cat [<Option>]... [<File>]...\n");
            printf(" FILES are combined and send to standard output.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output line\n");
            printf("    -b, --number-nonblank\t number nonempty output line\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("cat: Invalid option\n");
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
            else {
                printf("cat: Unrecognized option -- '%s'\n", str);
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        if (strcmp(cmd, "/etc/passwd") == 0) {
            fileReadWrite(TreeDir, cmd, 4);
            return 0;
        }
        //printf("cmd :  %s\n", cmd);
        //printf("tmp:   %s\n", tmp);
        strncpy(tmp, cmd, strlen(cmd)); // 변경 부분!! cmd에 제대로 된 값 전달 X
        //printf("%s\n", tmp);
        if (strstr(cmd, "/") == NULL) { // segfault 발생 지역
            if (OwnPermission(TreeDir->current, 'w') != 0) {
                printf("cat: Can not create file '%s': Permission denied\n", TreeDir->current->name);
                return -1;
            }
            //printf("is it here?\n");
            tempNode = DirExistion(TreeDir, cmd, 'd');
            tempNode2 = DirExistion(TreeDir, cmd, 'f');
            if (tempNode == NULL && tempNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", cmd);
                return -1;
            }
            else if (tempNode != NULL && tempNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", cmd);
                return -1;
            }
            else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tempNode2->name);
                return -1;
            }
            else {
                //printf("%s\n", cmd); 파일 이름 잘 넘어감
                fileReadWrite(TreeDir, cmd, 1);
            }

        }
        else {
            strncpy(tmp2, TakeDir(cmd), MAX_DIR);
            val = PathMove(TreeDir, tmp2);
            if (val != 0) {
                printf("cat: '%s': No such file or directory.\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tempNode = DirExistion(TreeDir, tmp3, 'd');
            tempNode2 = DirExistion(TreeDir, tmp3, 'f');
            if (tempNode == NULL && tempNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else if (tempNode != NULL && tempNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else if (tempNode2 != NULL && OwnPermission(tempNode2, 'r') != 0) {
                printf("cat:  Can not open file '%s': Permission denied\n", tempNode2->name);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                fileReadWrite(TreeDir, tmp3, 1);
            }
            TreeDir->current = NodeCurrent;
        }
    }
    return 1;
}
char* deleteSpace(char* s){
    int k = 0;
    char* str = (char*)malloc(sizeof(s));

    for (int i=0;i<strlen(s);i++){
        if (s[i]!=' ') {
            str[k++] = s[i];
        }
    }
    str[k] = '\0';
    return str;
}

void grep(char* Word_Search, char* f_name) {
    int i = 1;
    char output_line[MAX_LENGTH];
    FILE* fp = fopen(f_name, "rt");
    if (fp == NULL){
        printf("Can not read the file\n");
        return ;
    }
    while (1) {
        if (feof(fp))
            break;
        else
            fgets(output_line, sizeof(output_line), fp);
        i++;
    }
    FILE* fp2 = NULL;
    fp2 = fopen(f_name, "rt");
    for (int j = 1; j < i - 1; j++) {
        fgets(output_line, sizeof(output_line), fp2);
        if (strstr(output_line, Word_Search) != NULL)
            printf("%s", output_line);
    }
    fclose(fp);
}

void grep2(char* Word_Search, char* f_name) {
    int i = 1;
    char output_line[MAX_LENGTH];
    FILE* fp = fopen(f_name, "rt");
    if (fp == NULL){
        printf("Can not read the file\n");
        return ;
    }
    while (1) {
        if (feof(fp))
            break;
        else
            fgets(output_line, sizeof(output_line), fp);
        i++;
    }
    FILE* fp2 = NULL;
    fp2 = fopen(f_name, "rt");
    for (int j = 1; j < i - 1; j++) {
        fgets(output_line, sizeof(output_line), fp2);
        if (strstr(output_line, Word_Search) != NULL)
            printf("%d: %s", j, output_line);
    }
    fclose(fp);
}
void grep_v(char* findWord, char *findFile){
    FILE* fp = fopen(findFile, "rt");
    if(fp == NULL){
        printf("Can not Exist File!\n");
        return;
    }

    int index = 0;
    char outputLine[MAX_LENGTH];
    int isNotExist[MAX_LENGTH] = {0, };
    memset(isNotExist, 0, sizeof(isNotExist));
    while(!feof(fp)){
        char* temp = fgets(outputLine, sizeof(outputLine), fp);
        if(temp == NULL) break;
        temp = deleteSpace(temp);
        if(strstr(temp, findWord) == NULL){
            isNotExist[index] = 1;
        }
        index++;  
    }
    fclose(fp);

    index = 0;
    FILE* rfp = fopen(findFile, "rt");
    while(!feof(rfp)){
        char* temp = fgets(outputLine, sizeof(outputLine), rfp);
        if(temp == NULL) break;
        if(isNotExist[index++]){
            printf("%s", temp);
        }
    }
    fclose(rfp);
}

// ðÂÊ«?Öð ?ùì??
void grep_i(char* findWord, char *findFile){
    FILE* fp = fopen(findFile, "rt");
    if(fp == NULL){
        printf("Can not Exist File!\n");
        return;
    }

    for(int i=0;i<strlen(findWord);i++){
        if(65 <= findWord[i] && findWord[i] <= 90){
                findWord[i] += 32;
            }
    }

    char outputLine[MAX_LENGTH];
    while(!feof(fp)){
        char* line = fgets(outputLine, sizeof(outputLine), fp);
        char* temp = line;
        if(temp == NULL) break;
        temp = deleteSpace(temp);
        for(int i=0;i<strlen(temp);i++){
            if(65 <= temp[i] && temp[i] <= 90){
                temp[i] += 32;
            }
        }
        if(strstr(temp, findWord) != NULL){
            printf("%s", line);
        }
    }
    fclose(fp);
}
int chmod_(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* tempNode = NULL;
    char* str;
    int tmp;

    if (cmd == NULL) {
        printf("chmod: Invalid option\n");
        printf("Try 'chmod --help' for more information.\n");
        return -1;
    }
    if (cmd[0] == '-') {
        if (strcmp(cmd, "-R") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("chmod: Invalid option\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3) {
                tmp = atoi(str);
                str = strtok(NULL, " ");
                if (str == NULL) {
                    printf("chmod: Invalid option\n");
                    printf("Try 'chmod --help' for more information.\n");
                    return -1;
                }
                tempNode = DirExistion(TreeDir, str, 'd');
                if (tempNode != NULL) {
                    if (tempNode->LeftChild == NULL)
                        ModeConvers(TreeDir, tmp, str);
                    else {
                        ModeConvers(TreeDir, tmp, str);
                        ModeConversAll(tempNode->LeftChild, tmp);
                    }
                }
                else {
                    printf("chmod: '%s': No such file or directory.\n", str);
                    return -1;
                }
            }
            else {
                printf("chmod: Invalid Mode: '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Manual: chmod [Option]... Octal Number MODE... DIRECTORY...\n");
            printf("  Change the mode of each FILE to MODE.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("chmod: Invalid option\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            else {
                printf("chmod: Unrecognized option -- '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        if (cmd[0] - '0' < 8 && cmd[1] - '0' < 8 && cmd[2] - '0' < 8 && strlen(cmd) == 3) {
            tmp = atoi(cmd);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("chmod: Invalid option\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            ModeConvers(TreeDir, tmp, str);
        }
        else {
            printf("chmod: Invalid Mode: '%s'\n", cmd);
            printf("Try 'chmod --help' for more information.\n");
            return -1;
        }
    }
    return 0;
}

int find(DirectoryTree* TreeDir, char* cmd)
{
    char* str;
    if (cmd == NULL) {
        DirFind(TreeDir, TreeDir->current->name, 1);
        return 0;
    }
    else if (cmd[0] == '-') {
        if (strcmp(cmd, "-name") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("find: Invalid option\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            DirFind(TreeDir, str, 0);
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Manual: find [<Option>]... [<File>]...\n");
            printf("\n");
            printf("  Options:\n");
            printf("    -name\t finds file by name\n");
            printf("        --help\t Display this help and exit\n");
            return -1;
        }
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("find: Invalid option\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            else {
                printf("find: Unrecognized option -- '%s'\n", str);
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        DirFind(TreeDir, cmd, 1);
    }

    return 0;
}

void pasingCommand(DirectoryTree* TreeDir, char* cmd)
{
    char* str;
    char* str1;
    char* str2;
    int val;
    if (strcmp(cmd, "") == 0 || cmd[0] == ' ') { // 입력 X
        return;
    }
    str = strtok(cmd, " ");

    if (strcmp(str, "mkdir") == 0) {
        str = strtok(NULL, " ");
        val = Mkdir(TreeDir, str);
        if (val == 0) {
            SaveDir(TreeDir, dStack);
        }
    }
    else if (strcmp(str, "cp") == 0) {
        str = strtok(NULL, " ");
        str1 = strtok(NULL, " ");
        val = mycp(TreeDir, str, str1);
        if (val == 0) {
            SaveDir(TreeDir, dStack);
        }
    }
    else if (strcmp(str, "rm") == 0) {
        str = strtok(NULL, " ");
        val = rm(TreeDir, str);
        if (val == 0) {
            SaveDir(TreeDir, dStack);
        }
    }
    else if (strcmp(str, "cd") == 0) {
        str = strtok(NULL, " ");
        cd(TreeDir, str);
    }
    else if (strcmp(str, "pwd") == 0) {
        str = strtok(NULL, " ");
        pwd(TreeDir, dStack, str);
    }
    else if (strcmp(str, "ls") == 0) {
        str = strtok(NULL, " ");
        printf("%s\n", str);
        if (str == NULL)
            ls(TreeDir);
        else if (strcmp(str, "-a") == 0)
            ls_a(TreeDir);
        else if (strcmp(str, "-l") == 0)
            ls_l(TreeDir);
        else if (strcmp(str, "-al") == 0)
            ls_al(TreeDir);
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: ls [option]\n");
            printf("  Show your list of file or directory.\n\n");
            printf("  Options:\n");
            printf("    -a : show your all files include hiding files\n");
            printf("    -l : show your files with details\n");
            printf("    -al: show your all files include hinding files with details \n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
        }
        else
            printf("Invailed option of ls check your command plz\n");
    }
    else if (strcmp(str, "cat") == 0) {
        str = strtok(NULL, " ");
        val = cat(TreeDir, str);
        if (val == 0) {
            SaveDir(TreeDir, dStack);
        }
    }
    else if (strcmp(str, "chmod") == 0) {
        str = strtok(NULL, " ");
        val = chmod_(TreeDir, str);
        if (val == 0) {
            SaveDir(TreeDir, dStack);
        }
    }
    else if(strcmp(str, "chown") == 0){
        str = strtok(NULL, " ");
        val = chown_(TreeDir, str); 
        if(val == 0){
            SaveDir(TreeDir, dStack);
        }
    }
    else if (strcmp(str, "find") == 0) {
        str = strtok(NULL, " ");
        find(TreeDir, str);
    }
    else if (strcmp(cmd, "exit") == 0) {
        printf("Logout\n");
        exit(0);
    }
    else if (strcmp(str, "grep") == 0) {
        str = strtok(NULL, " ");
        str1 = strtok(NULL, " ");
        str2 = strtok(NULL, " ");
        if (strcmp(str, "-n") == 0) grep2(str1, str2);
        else if (strcmp(str, "-i") == 0) grep_i(str1, str2);
        else if (strcmp(str, "-v") == 0) grep_v(str1, str2);
        else grep(str, str1);
    }
    else if (strcmp(str, "clear") == 0) {
        system("clear");
    }
    else {
        printf("'%s': Command not found\n", cmd);
    }
    return;
}

void printHedder(DirectoryTree* TreeDir, Stack* StackDir)
{
    TreeNode* tempNode = NULL;
    char tmp[MAX_DIR] = "";
    char tmp2[MAX_DIR] = "";
    char usr;

    if (UsersList->current == UsersList->head)
        usr = '#';
    else
        usr = '$';

    printf("%s@os-Virtualbox", UsersList->current->name);
    printf(":");
    tempNode = TreeDir->current;

    if (tempNode == TreeDir->root) {
        strcpy(tmp, "/");
    }
    else {
        while (tempNode->Parent != NULL) {
            Push(StackDir, tempNode->name);
            tempNode = tempNode->Parent;
        }
        while (EmptyTrue(StackDir) == 0) {
            strcat(tmp, "/");
            strcat(tmp, Pop(StackDir));
        }
    }

    strncpy(tmp2, tmp, strlen(UsersList->current->dir));

    if (UsersList->current == UsersList->head) {
        printf("%s", tmp);
    }
    else if (strcmp(UsersList->current->dir, tmp2) != 0) {
        printf("%s", tmp);
    }
    else {
        tempNode = TreeDir->current;
        while (tempNode->Parent != NULL) {
            Push(StackDir, tempNode->name);
            tempNode = tempNode->Parent;
        }
        Pop(StackDir);
        Pop(StackDir);
        printf("~");
        printf("/");
        printf("%s", Pop(StackDir));
        while (EmptyTrue(StackDir) == 0) {
            printf("/");
            printf("%s", Pop(StackDir));
        }
    }
    printf(" %c ", usr);
}
//time
void TakeMonth(int i)
{
    switch (i) {
    case 1:
        printf("Jan ");
        break;
    case 2:
        printf("Feb ");
        break;
    case 3:
        printf("Mar ");
        break;
    case 4:
        printf("Apr ");
        break;
    case 5:
        printf("May ");
        break;
    case 6:
        printf("Jun ");
        break;
    case 7:
        printf("Jul ");
        break;
    case 8:
        printf("Aug ");
        break;
    case 9:
        printf("Sep ");
        break;
    case 10:
        printf("Oct ");
        break;
    case 11:
        printf("Nov ");
        break;
    case 12:
        printf("Dec ");
        break;
    default:
        break;
    }
}

void TakeWeekDay(int i)
{
    switch (i) {
    case 0:
        printf("Sun ");
        break;
    case 1:
        printf("Mon ");
        break;
    case 2:
        printf("Tue ");
        break;
    case 3:
        printf("Wed ");
        break;
    case 4:
        printf("Thu ");
        break;
    case 5:
        printf("Fri ");
        break;
    case 6:
        printf("Sat ");
        break;
    default:
        break;
    }
}