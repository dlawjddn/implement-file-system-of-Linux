#include "../include/osproject.h"

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
    User = fopen("./resource/User.txt", "w");
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
    User = fopen("./resource/User.txt", "r");

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
    UserNode* tmpUser = NULL;
    char NameUser[MAX_NAME];
    char tmp[MAX_DIR];

    tmpUser = ListUser->head;

    printf("Users: ");
    while (tmpUser != NULL) {
        printf("%s ", tmpUser->name);
        tmpUser = tmpUser->LinkNode;
    }
    printf("\n");

    while (1) {
        printf("Login as: ");
        fgets(NameUser, sizeof(NameUser), stdin);
        NameUser[strlen(NameUser) - 1] = '\0';
        if (strcmp(NameUser, "exit") == 0) {
            break;
        }
        tmpUser = UserExistion(ListUser, NameUser);
        if (tmpUser != NULL) {
            ListUser->current = tmpUser;
            break;
        }
        printf("'%s' Invalid user\n", NameUser);
    }
    strcpy(tmp, ListUser->current->dir);
    PathMove(TreeDir, tmp);
}