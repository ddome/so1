#ifndef _PQADT_
#define _PQADT_

/*
 *          CONTRATO DEL TAD COLA CON MANEJO DE PRIORIDADES
 * Una cola de prioridades es aquella estructura de datos que en la que los
 * elementos son encolados en un orden especifico determinado por una
 * prioridad asociada a cada uno de ellos. Si varios elementos tienen la misma
 * prioridad, se encolaran de modo convencional según la posición que ocupen.
*/

/* Este es el tipo abstracto usado para representar una instancia particular de
 * la cola de prioridades.
*/
typedef struct pqCDT *pqADT;

/* Este es el tipo de dato que maneja la cola.
*/
typedef void * pqElementT;

/*---------------------------------------------------------------------------*/
/* Prototipo de la funcion callback necesaria para hacer una copia del dato a
 * encolar (En caso de querer hacerse deep copy). La funcion misma es
 * responsable de alocar todos los recursos necesario. Es importante que los
 * alocamientos de memoria se hagan en base al tipo de dato del dato a encolar
 * De esta forma se evita tener que estar copiando el elemento a devolver al
 * desencolar.
*/
typedef pqElementT (*fCopia)(pqElementT);

/* Prototipo de la funcion callback necesaria para poder liberar correctamente
 * los recursos. Si se desea poder liberar completamente la cola y esta ultima
 * no esta vacia, esta funcion es indispensable. La funcion es responsable de
 *  liberar todos los recursos alocados por la funcion de copiado.
*/
typedef pqElementT (*fLibera)(pqElementT);

/* IMPORTANTE:      Las funciones de liberado como de copiado pueden estar como
 *                  no definidas. Si la funcion de copiado no esta definida
 *                  se encolara el elemento a travez de una simple asignacion.
 *                  Todos los cambios realizados fuera del TAD afectaran a la
 *                  informacion de dato encolado.
 *                  Es importante notar que si la funcion de copiado esta
 *                  definida y la de liberar no lo esta, los elementos encolados
 *                  no seran liberados adecuandamente, causando asi leaks de
 *                  memoria. Es importante que si se define la funcion de
 *                  copiado tambien se defina la de liberar.
*/



/*
 * Funciones provistas por el TAD para el manejo de la cola
*/


/* Funcion:         NewPQ
 * Uso:             pqADT pq;
 *                  pq=NewPQ(FuncionCopia,FuncionLibera);
 *-----------------------------------------------------------------------------
 * Descripcion:     Crea una nueva cola de prioridades. Devuelvo NULL en caso
 *                  de error.
 *-----------------------------------------------------------------------------
 * Precondicion:    -
 * Postcondicion:   Cola creada.
*/
pqADT NewPQ(fCopia copia,fLibera libera);


/* Funcion:         PQIsEmpty
 * Uso:             PQIsEmpty(pq);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion devuele 1 si hay al menos un elemento en la
 *                  cola. 0 en caso contrario.
 *-----------------------------------------------------------------------------
 * Precondicion:    Cola creada correctamente.
 * Postcondicion:   Valor entero (0 o 1). Cola invariante.
*/
int PQIsEmpty(pqADT pq);


/* Funcion:         QueueDepth
 * Uso:             QueueDepth(pq);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion devuelve la cantidad de elementos presentes en
 *                  la cola en el momento de la invocacion.
 *-----------------------------------------------------------------------------
 * Precondicion:    Cola creada correctamente.
 * Postcondicion:   Valor entero positivo. Cola invariante.
*/
int QueueDepth(pqADT pq);


/* Funcion:         Enqueue
 * Uso:             Enqueue(pq,elemento,2);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion encola un elemento segun la prioridad del
 *                  elemento. Devuelve 1 si se pudo encolar satisfactoriamente,
 *                  -1 en caso contrario.
 *-----------------------------------------------------------------------------
 * Precondicion:    Cola creada correctamente.
 * Postcondicion:   Cola con el nuevo elemento encolado segun su prioridad
*/
int Enqueue(pqADT pq,pqElementT elem, unsigned long prio);


/* Funcion:         Dequeue
 * Uso:             Dequeue(pq);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion devuelve el elemento a desencolar. El usuario
 *		    es responsable de liberar el espacio ocupado por el
 *		    elemento devuelto.
 *-----------------------------------------------------------------------------
 * Precondicion:    Cola creada correctamente.
 * Postcondicion:   Cola sin el elemente que fue desencolado.
*/
pqElementT Dequeue( pqADT pq);


/* Funcion:         FreePQ
 * Uso:             FreePQ(&pq);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion libera todos los recursos reservados. 
 *-----------------------------------------------------------------------------
 * Precondicion:    Cola creada correctamente.
 * Postcondicion:   -
*/
void FreePQ(pqADT * pq);

#endif
