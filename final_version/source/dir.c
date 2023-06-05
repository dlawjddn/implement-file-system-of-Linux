#include "../include/osproject.h"

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
    TreeNode* tmpNode = NULL;
    char tmp[MAX_DIR] = "";

    tmpNode = NodeDir->Parent;

    if (tmpNode == TreeDir->root) {
        strcpy(tmp, "/");
    }
    else {
        while (tmpNode->Parent != NULL) {
            Push(StackDir, tmpNode->name);
            tmpNode = tmpNode->Parent;
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

void SaveDir(DirectoryTree* TreeDir, Stack* StackDir)
{
    Dir = fopen("./resource/Directory.txt", "w");
    NodeWrite(TreeDir, TreeDir->root, StackDir);
    fclose(Dir);
}

int NodeRead(DirectoryTree* TreeDir, char* tmp)
{
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));
    TreeNode* tmpNode = NULL;
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
            tmpNode = TreeDir->current->LeftChild;
            while (tmpNode->RightChild != NULL)
                tmpNode = tmpNode->RightChild;
            tmpNode->RightChild = NewNode;
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
    Dir = fopen("./resource/Directory.txt", "r");

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
    TreeNode* tmpNode = NULL;

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
    tmpNode = DirExistion(TreeDir, NameDir, type);
    if (tmpNode != NULL && tmpNode->type == 'd') {
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
        tmpNode = TreeDir->current->LeftChild;

        while (tmpNode->RightChild != NULL) {
            tmpNode = tmpNode->RightChild;
        }
        tmpNode->RightChild = NewNode;
    }

    return 0;
}

//rm
int RemoveDir(DirectoryTree* TreeDir, char* NameDir)
{
    TreeNode* DelNode = NULL;
    TreeNode* tmpNode = NULL;
    TreeNode* prevNode = NULL;

    tmpNode = TreeDir->current->LeftChild;

    if (tmpNode == NULL) {
        printf("rm: Can not remove '%s': No such file or directory\n", NameDir);
        return -1;
    }

    if (strcmp(tmpNode->name, NameDir) == 0) {
        TreeDir->current->LeftChild = tmpNode->RightChild;
        DelNode = tmpNode;
        if (DelNode->LeftChild != NULL)
            DirRemove(DelNode->LeftChild);
        NodeRemove(DelNode);
    }
    else {
        while (tmpNode != NULL) {
            if (strcmp(tmpNode->name, NameDir) == 0) {
                DelNode = tmpNode;
                break;
            }
            prevNode = tmpNode;
            tmpNode = tmpNode->RightChild;
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
    TreeNode* tmpNode = NULL;
    if (strcmp(PathDir, ".") == 0) {
    }
    else if (strcmp(PathDir, "..") == 0) {
        if (TreeDir->current != TreeDir->root) {
            TreeDir->current = TreeDir->current->Parent;
        }
    }
    else {

        tmpNode = DirExistion(TreeDir, PathDir, 'd');
        if (tmpNode != NULL) {
            TreeDir->current = tmpNode;
        }
        else
            return -1;
    }
    return 0;
}

int PathMove(DirectoryTree* TreeDir, char* PathDir)
{
    TreeNode* tmpNode = NULL;
    char tmpPath[MAX_DIR];
    char* str = NULL;
    int val = 0;

    strncpy(tmpPath, PathDir, MAX_DIR);
    tmpNode = TreeDir->current;
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
                TreeDir->current = tmpNode;
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

    TreeNode* tmpNode = NULL;
    tmpNode = TreeDir->current;
    //if current directory is root
    if (tmpNode == TreeDir->root) {
        printf("/");
    }
    else {
        //until current directory is root, repeat Push
        while (tmpNode->Parent != NULL) {
            Push(StackDir, tmpNode->name);
            tmpNode = tmpNode->Parent;
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
int Concatenate(DirectoryTree* dirTree, char* fName, int o)
{
    UserNode* tmpUser = NULL;
    TreeNode* tmpNode = NULL;
    FILE* fp;
    char buf[MAX_BUFFER];
    char tmpName[MAX_NAME];
    char* str;
    int tmpSIZE = 0;
    int cnt = 1;

    //file read
    if(o != 0){
        if(o == 4){
            tmpUser = UsersList->head;
            while(tmpUser != NULL){
                printf("%s:x:%d:%d:%s:%s\n", tmpUser->name, tmpUser->UID, tmpUser->GID, tmpUser->name, tmpUser->dir);
                tmpUser = tmpUser->LinkNode;
            }
            return 0;
        }
        tmpNode = DirExistion(dirTree,fName, 'f');

        if(tmpNode == NULL){
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

        tmpNode = DirExistion(dirTree, fName, 'f');
        //if exist
        if(tmpNode != NULL){
            time(&ltime);
            today = localtime(&ltime);

            tmpNode->month = today->tm_mon + 1;
            tmpNode->day = today->tm_mday;
            tmpNode->hour = today->tm_hour;
            tmpNode->min = today->tm_min;
        }
        //if file doesn't exist
        else{
            MakeDir(dirTree, fName, 'f');
        }
        //write size
        tmpNode = DirExistion(dirTree, fName, 'f');
        tmpNode->SIZE = tmpSIZE;
    }
    return 0;
}
//chmod
int ModeConvers(DirectoryTree* TreeDir, int mode, char* NameDir)
{
    TreeNode* tmpNode = NULL;
    TreeNode* tmpNode2 = NULL;
    tmpNode = DirExistion(TreeDir, NameDir, 'd');
    tmpNode2 = DirExistion(TreeDir, NameDir, 'f');

    if (tmpNode != NULL) {
        if (OwnPermission(tmpNode, 'w') != 0) {
            printf("chmod: Can not modify file '%s': Permission denied\n", NameDir);
            return -1;
        }
        tmpNode->mode = mode;
        ModeToPermission(tmpNode);
    }
    else if (tmpNode2 != NULL) {
        if (OwnPermission(tmpNode2, 'w') != 0) {
            printf("chmod: Can not modify file '%s': Permission denied\n", NameDir);
            return -1;
        }
        tmpNode2->mode = mode;
        ModeToPermission(tmpNode2);
    }
    else {
        printf("chmod: Can not access to '%s: There is no such file or directory\n", NameDir);
        return -1;
    }
    return 0;
}
//chown
int ChangeOwner(DirectoryTree* TreeDir, char* userName, char* dirName) // TreeDir warning
{
    TreeNode* tmpNode = NULL;
    TreeNode* tmpNode2 = NULL;
    UserNode* tmpUser = NULL;

    tmpNode = DirExistion(TreeDir, dirName, 'd');
    tmpNode2 = DirExistion(TreeDir, dirName, 'f');


    if(tmpNode != NULL){
        if(OwnPermission(tmpNode, 'w') != 0){
            printf("chown: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpUser = UserExistion(UsersList, userName);
        if(tmpUser != NULL){
            tmpNode->UID = tmpUser->UID;
            tmpNode->GID = tmpUser->GID;
        }
        else{
            printf("chown: 잘못된 사용자: '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if(tmpNode2 != NULL){
        if(OwnPermission(tmpNode2, 'w') != 0){
            printf("chown: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpUser = UserExistion(UsersList, userName);
        if(tmpUser != NULL){
            tmpNode2->UID = tmpUser->UID;
            tmpNode2->GID = tmpUser->GID;
        }
        else{
            printf("chown: 잘못된 사용자: '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else{
        printf("chown: '%s'에 접근할 수 없습니다: 그런 파일이나 디렉터리가 없습니다\n", dirName);
        return -1;
    }

    return 0;
}
//chown
void ChangeOwnerAll(TreeNode* NodeDir, char* userName)
{
    UserNode* tmpUser = NULL;

    tmpUser = UserExistion(UsersList, userName);
    /*
        993 line
        error: no member named 'RightSibling' in 'struct TreeNodetag'
        if(NodeDir->RightSibling != NULL){
    */

    if(NodeDir->RightChild != NULL){
        /*
            999 line
            error: no member named 'RightSibling' in 'struct TreeNodetag'
            ChangeOwnerAll(NodeDir->RightSibling, userName);
        */
        ChangeOwnerAll(NodeDir->RightChild, userName);
    }
    if(NodeDir->LeftChild != NULL){
        ChangeOwnerAll(NodeDir->LeftChild, userName);
    }
    NodeDir->UID = tmpUser->UID;
    NodeDir->GID = tmpUser->GID;
}
int chown_(DirectoryTree* dirTree, char* cmd)
{
    TreeNode* tmpNode = NULL;
    UserNode* tmpUser = NULL;
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
            tmpUser = UserExistion(UsersList, str);
            if(tmpUser != NULL){
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
            tmpNode = DirExistion(dirTree, str, 'd');
            if(tmpNode != NULL){
                if(tmpNode->LeftChild == NULL)
                    ChangeOwner(dirTree, tmp, str);
                else{
                    ChangeOwner(dirTree, tmp, str);
                    ChangeOwnerAll(tmpNode->LeftChild, tmp);
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
    Dir = fopen("./resource/Directory.txt", "r");

    while (fgets(tmp, MAX_LENGTH, Dir) != NULL) {
        DirRead(TreeDir, tmp, NameDir, o);
    }
    fclose(Dir);
}