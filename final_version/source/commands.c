#include "../include/osproject.h"

//pasingCommand command
int Mkdir(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* tmpNode = NULL;
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

    tmpNode = TreeDir->current;
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
    TreeNode* tmpNode = NULL;
    TreeNode* tmpNode2 = NULL;
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
                tmpNode = DirExistion(TreeDir, str, 'f');
                tmpNode = (DirExistion(TreeDir, str, 'd') == NULL) ? tmpNode : DirExistion(TreeDir, str, 'd');
                if (tmpNode == NULL) {
                    printf("rm: Can not remove '%s': No such file or directory.\n", str);
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
                tmpNode = DirExistion(TreeDir, tmp3, 'f');
                tmpNode = (DirExistion(TreeDir, tmp3, 'd') == NULL) ? tmpNode : DirExistion(TreeDir, tmp3, 'd');
                if (tmpNode == NULL) {
                    printf("rm: Can not remove '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
                tmpNode = DirExistion(TreeDir, str, 'f');
                if (tmpNode == NULL) {
                    printf("error: %s is not a file, %s is a directory\n", str, str);
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
                tmpNode = DirExistion(TreeDir, tmp3, 'f');
                if (tmpNode == NULL) {
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
                tmpNode = DirExistion(TreeDir, str, 'f');
                tmpNode = (DirExistion(TreeDir, str, 'd') == NULL) ? tmpNode : DirExistion(TreeDir, str, 'd');
                if (tmpNode == NULL) {
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
                tmpNode = DirExistion(TreeDir, tmp3, 'f');
                tmpNode = (DirExistion(TreeDir, tmp3, 'd') == NULL) ? tmpNode : DirExistion(TreeDir, tmp3, 'd');
                if (tmpNode == NULL) {
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
            tmpNode = DirExistion(TreeDir, cmd, 'f');
            tmpNode2 = DirExistion(TreeDir, cmd, 'd');

            if (tmpNode2 != NULL) {
                printf("rm: Can not remove '%s': Is a directory\n", cmd);
                return -1;
            }
            if (tmpNode == NULL) {
                printf("rm: Can not remove '%s': No such file or directory.\n", cmd);
                return -1;
            }
            else {
                if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
            tmpNode = DirExistion(TreeDir, tmp3, 'f');
            tmpNode2 = DirExistion(TreeDir, tmp3, 'd');

            if (tmpNode2 != NULL) {
                printf("rm: Can not remove '%s': Is a directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            if (tmpNode == NULL) {
                printf("rm: Can not remove '%s' No such file or directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                if (OwnPermission(TreeDir->current, 'w') != 0 || OwnPermission(tmpNode, 'w') != 0) {
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
    TreeNode* tmpNode = NULL;
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
        tmpNode = DirExistion(TreeDir, cmd, 'd');
        if (tmpNode != NULL) {
            if (OwnPermission(tmpNode, 'r') != 0) {
                printf("-bash: cd: '%s': Permission denied\n", cmd);
                return -1;
            }
        }
        tmpNode = DirExistion(TreeDir, cmd, 'f');
        if (tmpNode != NULL) {
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
    UserNode* tmpNode = NULL;

    tmpNode = UsersList->head;
    while(tmpNode != NULL){
        if(tmpNode->UID == dirNode->UID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

char* GetGID(TreeNode* dirNode)
{
    UserNode* tmpNode = NULL;

    tmpNode = UsersList->head;
    while(tmpNode != NULL){
        if(tmpNode->GID == dirNode->GID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

void ls(DirectoryTree* TreeDir) {
    int count = 0;
    TreeNode* tmpNode = TreeDir->current;
    if (tmpNode->LeftChild == NULL)
        printf("directory empty...\n");
    else {
        tmpNode = tmpNode->LeftChild;
        while (tmpNode->RightChild != NULL) {

            if(tmpNode->name[0] != '.'){
                if (strlen(tmpNode->name) < 8)
                    printf("%s\t\t", tmpNode->name);
                else
                    printf("%s\t", tmpNode->name);
                count++;
            }
            tmpNode = tmpNode->RightChild;

            if (count % 5 == 0)
                printf("\n");

        }
        printf("%s\t\n", tmpNode->name);
    }
}

void ls_a(DirectoryTree* TreeDir) {
    int count = 1;
    TreeNode* tmpNode = TreeDir->current;
    if (tmpNode->LeftChild == NULL) {
        printf(".\t\t..\n");
    }
    else {
        printf(".\t\t..\t\t");
        count = count + 2;
        tmpNode = tmpNode->LeftChild;
        while (tmpNode->RightChild != NULL) {
            if (strlen(tmpNode->name) < 8)
                printf("%s\t\t", tmpNode->name);
            else
                printf("%s\t", tmpNode->name);
            tmpNode = tmpNode->RightChild;
            if (count % 5 == 0)
                printf("\n");
            count++;
        }
        printf("%s\t\n", tmpNode->name);
    }
}
void ls_l(DirectoryTree* TreeDir) {
    time_t timer = time(NULL);
    TreeNode* tmpNode = TreeDir->current;
    TreeNode* tmpNode2 = tmpNode->LeftChild;
    int cnt=0;

    if(tmpNode2 == NULL) cnt = 2;
    else{
        if(tmpNode2->type == 'd') cnt = 3;
        else cnt = 2;

        while(tmpNode2->RightChild != NULL){
            tmpNode2 = tmpNode2->RightChild;
            if(tmpNode2->type == 'd') cnt = cnt + 1;
        }
    }
    if (tmpNode->LeftChild == NULL)
        printf("directory empty\n");
    else {
        tmpNode = tmpNode->LeftChild;
        while (tmpNode->RightChild != NULL) {
            if(tmpNode->name[0] != '.') {
                printf("%c", tmpNode->type);
                PermissionPrint(tmpNode);
                printf("%3d   ", cnt);
                printf(" %-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
                printf(" %5d", tmpNode->SIZE);
                printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
                printf("%s\n", tmpNode->name);
            }
            tmpNode = tmpNode->RightChild;
        }
        printf("%c", tmpNode->type);
        PermissionPrint(tmpNode);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
        printf(" %5d", tmpNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
        printf("%s\n", tmpNode->name);
    }
}
void ls_al(DirectoryTree* TreeDir) {
    time_t timer = time(NULL);
    TreeNode* tmpNode = TreeDir->current;
    TreeNode* tmpNode2 = tmpNode->LeftChild;
    int cnt=0;

    if(tmpNode2 == NULL) cnt = 2;
    else{
        if(tmpNode2->type == 'd') cnt = 3;
        else cnt = 2;

        while(tmpNode2->RightChild != NULL){
            tmpNode2 = tmpNode2->RightChild;
            if(tmpNode2->type == 'd') cnt = cnt + 1;
        }
    }
    if (tmpNode->LeftChild == NULL) {
        //.
        printf("%c", TreeDir->current->type);
        PermissionPrint(TreeDir->current);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
        printf(" %5d", tmpNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
        printf(".\n");
        //..
        if (strcmp(TreeDir->current->name, "/") == 0) {
            printf("%c", TreeDir->current->type);
            PermissionPrint(TreeDir->current);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
            printf(" %5d", tmpNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
            printf("..\n");
        }
        else {
            printf("%c", tmpNode->Parent->type);
            PermissionPrint(tmpNode->Parent);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tmpNode->Parent), GetGID(tmpNode->Parent));
            printf(" %5d", tmpNode->Parent->SIZE);
            printf(" %d월 %2d %02d:%02d ", tmpNode->Parent->month, tmpNode->Parent->day, tmpNode->Parent->hour, tmpNode->Parent->min);
            printf("..\n");
        }
    }
    else {
        //.
        printf("%c", TreeDir->current->type);
        PermissionPrint(TreeDir->current);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
        printf(" %5d", tmpNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
        printf(".\n");
        //..
        if (strcmp(TreeDir->current->name, "/") == 0) {
            printf("%c", TreeDir->current->type);
            PermissionPrint(TreeDir->current);
            printf("%3d ", cnt);
            printf(" %-5s%-5s %5d", GetUID(tmpNode), GetGID(tmpNode), tmpNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
            printf("..\n");
        }
        else {
            printf("%c", tmpNode->Parent->type);
            PermissionPrint(tmpNode->Parent);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s", GetUID(tmpNode->Parent), GetGID(tmpNode->Parent));
            printf(" %5d", tmpNode->Parent->SIZE);
            printf(" %d월 %2d %02d:%02d ", tmpNode->Parent->month, tmpNode->Parent->day, tmpNode->Parent->hour, tmpNode->Parent->min);
            printf("..\n");
        }


        tmpNode = tmpNode->LeftChild;
        while (tmpNode->RightChild != NULL) {
            printf("%c", tmpNode->type);
            PermissionPrint(tmpNode);
            printf("%3d   ", cnt);
            printf(" %-5s%-5s %5d", GetUID(tmpNode), GetGID(tmpNode), tmpNode->SIZE);
            printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
            printf("%s\n", tmpNode->name);
            tmpNode = tmpNode->RightChild;
        }
        printf("%c", tmpNode->type);
        PermissionPrint(tmpNode);
        printf("%3d   ", cnt);
        printf(" %-5s%-5s %5d", GetUID(tmpNode), GetGID(tmpNode), tmpNode->SIZE);
        printf(" %d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->min);
        printf("%s\n", tmpNode->name);
    }
}
int cat(DirectoryTree* TreeDir, char* cmd)
{
    TreeNode* NodeCurrent = NULL;
    TreeNode* tmpNode = NULL;
    TreeNode* tmpNode2 = NULL;
    char* str;
    char tmp[MAX_DIR];
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
            tmpNode = DirExistion(TreeDir, str, 'd'); // if write permission-> TsExist excute
            if (tmpNode != NULL) {
                printf("cat: '%s':Is a directory.\n", str);
                return -1;
            }
            else {
                Concatenate(TreeDir, str, 0);
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
            tmpNode = DirExistion(TreeDir, tmp3, 'd');
            if (tmpNode != NULL) {
                printf("cat: '%s': Is a directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                Concatenate(TreeDir, tmp3, 0);
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
                tmpNode = DirExistion(TreeDir, str, 'd');
                tmpNode2 = DirExistion(TreeDir, str, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", str);
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", str);
                    return -1;
                }
                else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                    return -1;
                }
                else {
                    Concatenate(TreeDir, str, 2);
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
                tmpNode = DirExistion(TreeDir, tmp3, 'd');
                tmpNode2 = DirExistion(TreeDir, tmp3, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    Concatenate(TreeDir, tmp3, 2);
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
                tmpNode = DirExistion(TreeDir, str, 'd');
                tmpNode2 = DirExistion(TreeDir, str, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", str);
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': Is a directory.\n", str);
                    return -1;
                }
                else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                    return -1;
                }
                else {
                    Concatenate(TreeDir, str, 3);
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
                tmpNode = DirExistion(TreeDir, tmp3, 'd');
                tmpNode2 = DirExistion(TreeDir, tmp3, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': No such file or directory.\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': Is a direcotry\n", tmp3);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                    printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                    TreeDir->current = NodeCurrent;
                    return -1;
                }
                else {
                    Concatenate(TreeDir, tmp3, 3);
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
            Concatenate(TreeDir, cmd, 4);
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
            tmpNode = DirExistion(TreeDir, cmd, 'd');
            tmpNode2 = DirExistion(TreeDir, cmd, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", cmd);
                return -1;
            }
            else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", cmd);
                return -1;
            }
            else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                return -1;
            }
            else {
                //printf("%s\n", cmd); 파일 이름 잘 넘어감
                Concatenate(TreeDir, cmd, 1);
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
            tmpNode = DirExistion(TreeDir, tmp3, 'd');
            tmpNode2 = DirExistion(TreeDir, tmp3, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", tmp3);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else if (tmpNode2 != NULL && OwnPermission(tmpNode2, 'r') != 0) {
                printf("cat:  Can not open file '%s': Permission denied\n", tmpNode2->name);
                TreeDir->current = NodeCurrent;
                return -1;
            }
            else {
                Concatenate(TreeDir, tmp3, 1);
            }
            TreeDir->current = NodeCurrent;
        }
    }
    return 1;
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

void grep_v(char* findWord, char *findFile){
    //printf("%s\n", findFile);
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
    //printf("%s\n", findFile);
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
    TreeNode* tmpNode = NULL;
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
                tmpNode = DirExistion(TreeDir, str, 'd');
                if (tmpNode != NULL) {
                    if (tmpNode->LeftChild == NULL)
                        ModeConvers(TreeDir, tmp, str);
                    else {
                        ModeConvers(TreeDir, tmp, str);
                        ModeConversAll(tmpNode->LeftChild, tmp);
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
    if (strcmp(cmd, "") == 0 || cmd[0] == ' ') {
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
        if (str == NULL)
            ls(TreeDir);
        else if (strcmp(str, "-a") == 0)
            ls_a(TreeDir);
        else if (strcmp(str, "-l") == 0)
            ls_l(TreeDir);
        else
            ls_al(TreeDir);
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
        else if (strcmp(str, "-i") == 0) grep_i(str, str1);
        else if (strcmp(str, "-v") == 0) grep_v(str, str1);
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
    TreeNode* tmpNode = NULL;
    char tmp[MAX_DIR] = "";
    char tmp2[MAX_DIR] = "";
    char usr;

    if (UsersList->current == UsersList->head)
        usr = '#';
    else
        usr = '$';

    printf("%s@os-Virtualbox", UsersList->current->name);
    printf(":");
    tmpNode = TreeDir->current;

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

    strncpy(tmp2, tmp, strlen(UsersList->current->dir));

    if (UsersList->current == UsersList->head) {
        printf("%s", tmp);
    }
    else if (strcmp(UsersList->current->dir, tmp2) != 0) {
        printf("%s", tmp);
    }
    else {
        tmpNode = TreeDir->current;
        while (tmpNode->Parent != NULL) {
            Push(StackDir, tmpNode->name);
            tmpNode = tmpNode->Parent;
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