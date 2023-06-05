#include "../include/osproject.h"

void* threadmkdir(void* arg) {
    ThreadArg* pArgThreading = ((ThreadArg*)arg);
    DirectoryTree* pdirectoryTree = pArgThreading->MultiDirTree;
    DirectoryTree* ppreTree;
    char* command = pArgThreading->command;
    char* filename = pArgThreading->AddValues;
    char* PathCopy = pArgThreading->PathCopy;

    TreeNode* tmpNode = pdirectoryTree->current;
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
        pdirectoryTree->current = tmpNode;
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
            pdirectoryTree->current = tmpNode;
        }
    }
    pthread_exit(NULL);
}

int MakeMultiDir(DirectoryTree* TreeDir, char* directoryName, char type, char* AddValues) {
    TreeNode* NewNode = (TreeNode*)malloc(sizeof(TreeNode));
    TreeNode* tmpNode = NULL;

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
    tmpNode = DirExistion(TreeDir, directoryName, type);
    if (tmpNode != NULL) {
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
        tmpNode = TreeDir->current->LeftChild;

        while (tmpNode->RightChild != NULL) {
            tmpNode = tmpNode->RightChild;
        }
        tmpNode->RightChild = NewNode;
    }

    return 0;

}