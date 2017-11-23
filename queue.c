#include<stdlib.h>
#include"queue.h"
#include<stdio.h>

struct node{
	void* obj;
	Node *next;
};

struct queue{
    Node *first;
};

Queue * queue_create(){
	Queue *f;
	f = (Queue *) malloc(sizeof(Queue));
	if(f == NULL){
		printf("Lack of memory.\n");
		exit(1);
	}
    q->first = NULL;
	return f;
}

void queue_push(Queue *q, void* obj){
	Node *p, *new;
	if(q->first == NULL){
		new = (Node *) malloc(sizeof(Node));
		new->obj = obj;
		new->next = NULL;
		q->first = new;
	}
	else{
		p = q->first;
		while(p->next != NULL)
			p = p->next;
		new = (Node *) malloc(sizeof(Node));
		new->obj = obj;
		new->next = NULL;
		p->next = new;
	}
}

void * queue_pop(Queue *q){
	void* saida;
	Node *aux;
	aux = q->first;
	saida = aux->obj;
	q->first = aux->next;
	free(aux);
	return saida;
}

int queue_isempty(Queue *q){
	if(q->first == NULL)
		return 1;
	else return 0;
}

void queue_free(Queue *q){
	Node *p, *aux;
	p = q->first;
	while(p != NULL){
		aux = p;
		p = aux->next;
		free(aux);
	}
	free(f);
}
