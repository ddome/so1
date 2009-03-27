#include "pqADT.h"
#include <stdlib.h>
#include <string.h>


/*Header de la cola de prioridades*/
struct pqCDT{
    struct listT * list;
    fCopia copia;
    fLibera libera;
    int cant;
};

/*Celda de la lista*/
typedef struct listT{
    struct nodeT * head;
    struct nodeT * tail;
    struct listT * next;
    unsigned long prio;
}listT;

/*Nodo de las colas correspondientes a cada celda de la lista.*/
typedef struct nodeT{
    pqElementT elem;
    struct nodeT * next;
}nodeT;



pqADT
NewPQ(fCopia copia,fLibera libera)
{
    pqADT aux;
    if( (aux=calloc(1,sizeof(struct pqCDT)))==NULL )
        return NULL;

    aux->copia=copia;
    aux->libera=libera;
    
    return aux;
}



int
PQIsEmpty(pqADT pq)
{
    return pq->cant==0;
}



int
QueueDepth(pqADT pq)
{
    return pq->cant;
}



int
Enqueue(pqADT pq,pqElementT elem, unsigned long prio)
{
    listT * cur,*old;
    nodeT * newNode;
    cur=old=pq->list;
    
    /*Itero hasta encontrar la posicion deseada para encolar
     *el nuevo elemento.*/
    while( cur!=NULL && cur->prio<prio)
    {
        old=cur;
        cur=cur->next;
    }
    
    /*Si no hay elementos con la misma prioridad se crea un
     *nueva celda en la lista*/
    if( cur==NULL || cur->prio!=prio )
    {
        listT * new=calloc(1,sizeof(struct listT));
        if( new==NULL )
            return -1;
 
        new->next=cur;
        new->prio=prio;
        if( cur==old )
            pq->list=new;/*Es la primer celda de la lista*/
        else
            old->next=new;
        
        cur=new;
    }

    /*Una vez encontrada (o creada) la posicion para encolar, se crea
     *un nuevo nodo para almacenar los datos necesarios.*/
    newNode=calloc(1,sizeof(struct nodeT));
    if( newNode==NULL )
        return -1;
    
    if( cur->head==NULL)
        cur->head=newNode;
    else
        cur->tail->next=newNode;
    
    cur->tail = newNode;
    
    /*Se copian los datos enviados por el usuario.*/
    if( pq->copia!=NULL )
        newNode->elem=(pq->copia)(elem);
    else
        newNode->elem=elem;
    
    (pq->cant)++;
    
    return 1;
}



pqElementT
Dequeue( pqADT pq)
{
    pqElementT resp;
    listT * aux;
    nodeT * auxNode;
    /*La respuesta es una asignacion para evitar copiar de vuelta el dato.*/
    resp=pq->list->head->elem;
    
    auxNode=pq->list->head;
    pq->list->head=pq->list->head->next;
    
    /*Se libera el nodo de la cola.*/
    free(auxNode);
    
    /*Si no hay mas elementos se libera la celda de la lista.*/
    if(pq->list->head == NULL )
    {
        aux=pq->list;
        pq->list=aux->next;
        free(aux);
    }

    (pq->cant)--;

    return resp;

}



void
FreePQ(pqADT * pq)
{
    listT * auxList;
    nodeT * auxNodo;

    while( (*pq)->list != NULL )
    {
        
        auxList=(*pq)->list;
        while( auxList->head != NULL )
        {
            /*Se liberan los nodos de la cola.*/
            auxNodo=auxList->head;
            auxList->head=auxNodo->next;
            if( (*pq)->libera!=NULL )
                ((*pq)->libera)(auxNodo->elem);
            
            free(auxNodo);
            
        }
        /*Se liberan las celdas de la lista.*/
        (*pq)->list=(*pq)->list->next;
        free(auxList);
        
    }
    /*Se libera el header*/
    free(*pq);
    *pq=NULL;
}
