#include "../include/osproject.h"

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