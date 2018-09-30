#ifndef __SNLIST__H__
#define __SNLIST__H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t DataChar;

typedef struct Node{
	DataChar data;
	struct Node *next;
}SNode;

void SListInitiate( SNode **head );
int ListLength( SNode *head );
bool SListInsert( SNode *head, int i, DataChar x );
bool SListDelete( SNode *head, int i, DataChar *x );
bool SListGet( SNode *head, int i, DataChar *x );
void SDestroy( SNode **head );
bool SListExist( SNode *head, DataChar x );
void SListPop( SNode *head, DataChar x );
#endif
