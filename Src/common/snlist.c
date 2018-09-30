#include "snlist.h"

void SListInitiate( SNode **head ){
	*head = (SNode *)malloc( sizeof( SNode ));
	(*head)->next = NULL;
}

int ListLength( SNode *head ){
	SNode *p = head;
	int size = 0;
	while( p-> next != NULL ){
		p = p->next;
		size ++;
	}
	return size;
}

bool SListInsert( SNode *head, int i, DataChar x ){
	SNode *p, *q;
	int j;
	p = head;
	j = -1;
	while( p->next != NULL && j < i - 1 ){
		p = p->next;
		j++;
	}
	if( j !=  i - 1 ){
		return false;
	}
	q = ( SNode *)malloc( sizeof( SNode ));
	q->data = x;
	q->next = p->next;
	p->next = q;
	return true;
}

bool SListDelete( SNode *head, int i, DataChar *x ){
	SNode *p,*s;
	int j;
	p = head;
	j = -1;
    
    if( p->next == NULL ){
        *x = 0xff;
		return false;
    }
    
	while( p->next != NULL && p->next->next != NULL && j < i - 1 ){
		p = p->next;
		j++;
	}
    
	if( j != (i - 1) ){
        *x = 0xff;
		return false;
	}
    
	s = p->next;
	*x = s->data;
	p->next = p->next->next;
	free( s );
    
	return true;
}

void SListPop( SNode *head, DataChar x ){
    
    SNode *p = NULL, *q = NULL;
    p = head;
    
    while( p->next != NULL ){
        q = p;
        p = p->next;
        if( p->data == x ){
            q->next = p->next;
            free( p );
            break;
        }
    }
}



bool SListGet( SNode *head, int i, DataChar *x ){
	SNode *p;
	int j;
	p =head;
	j = -1;
    
    if( p->next == NULL ){
        *x = 0xff;
		printf( "SListGet--No data\r\n" );
		return false;
    }
    
	while( p->next != NULL && j < i ){
		p = p->next;
		j++;
	}
	if( j != i ){
		printf("Error taking element position\r\n");
		return false;
	}
	*x = p->data;
	return true;
}

bool SListExist( SNode *head, DataChar x ){
    
	SNode *p;
	p = head;
    
    if( p->next == NULL ){
		return false;
    }
    
	while( p->next != NULL ){
		p = p->next;
        if( p->data == x ){
            return true;
        }
	}

	return false;
}

void SDestroy( SNode **head ){
	SNode *p ,*p1;
	p = *head;
	while( p != NULL ){
		p1 = p;
		p = p->next;
		free( p1 );
	}
	*head = NULL;
}

