#include <stdio.h>
#include <stdlib.h>

#define STARTNODE 0             //nodo di partenza di ogni grafo
#define INFINITY 2147483647     //infinito per dijkstra, sarebbe int_max
#define THRESHOLD 512           //limite per il cambio struttura dati

#define BLACK 0
#define RED 1

struct node{            //definisco la struttura dei nodi del rbt
	int weight;
    int num;
	int color;
	struct node *parent;
	struct node *left;
	struct node *right;
};

typedef struct nodo_{       //definisco la struttura della lista
    int weight;
    int num;
    struct nodo_ *next;
} nodo_t;

//dichiarazione delle funzioni
void parameters(int*, int*);                //funzione per acquisire i parametri d e k
int read();                                 //funzione per la lettura di stdin
void AggiungiGrafo(int*, int*, nodo_t**, int*);   //funzione per leggere un nuovo grafo
void insmatrix(int*, int*);                 //funzione per leggere e salvare la matrice

int dijkstra(int*, int*);                  //funzione per implementare l'algoritmo di dijkstra

void insertNode(int);                //funzione per inserire un nodo nel rbt
void leftRotation(struct node*);     //funzione per la rotazione sx del rbt
void rightRotation(struct node*);    //funzione per la rotazione dx del rbt
void fixTree(struct node*);          //funzione per fixare la struttura del rbt
void TopKrbt(struct node*, int*);    //funzione per stampare i k nodi del rbt

nodo_t* tree2list(struct node*);        //funzione per convertire il rbt in una lista
void TopKlist(nodo_t*, int*);           //funzione per stampare la lista
nodo_t* sortedInsertion(nodo_t*, int);  //funzione per l'inserimento in ordine nella lista
nodo_t* headInsertion(nodo_t*, int);    //funzione per l'inserimento in testa della lista
nodo_t* destroyList(nodo_t*);           //funzione per liberare la memoria allocata per la lista

int limitList(nodo_t*, int*);           //funzione per trovare il k-esimo elemento della lista

int number = 0;                 //variabile globale per il numero di grafi
int u;                          //variabile globale per tener contro del k di TopK
int max;                        //variabile globale per il k-esimo elemento della lista
struct node *ROOT = NULL;       //variabile globale per la root del rbt che è presente in tutto il programma


//MAIN
int main(){
    int d, k;
    int *adjm;

    nodo_t* head = NULL;
    
    parameters(&d, &k);            //funzione per acquisire i parametri d e k

    int flag = read();             //funzione per leggere la prossima istruzione

    adjm = malloc((d*d)*sizeof(int));          //alloco la memoria per la adj matrix d*d

    while(flag != 0){
        if(flag == 1){
            AggiungiGrafo(adjm, &d, &head, &d);        //funzione per aggiungere un nuovi grafi

        } else{
            u = 0;
            if(number < THRESHOLD){
                TopKrbt(ROOT, &k);              //funzione per stampare la TopK dal rbt finora            
            } else{
                TopKlist(head, &k);             //funzione per stampare la TopK dalla lista finora 
            }
            printf("\n");
        }

        flag = read();
    }
    
    free(adjm);            //libera la memoria allocata per la adj matrix

    if(number >= THRESHOLD){        //libera la memoria allocata per la lista
        head = destroyList(head);
        free(head);
    }

    return 0;
}

     
//        sviluppo delle FUNZIONI

void parameters(int *nvertices, int *nrank){            //funzione per acquisire i parametri d e k
    char c;
    *nvertices = 0;
    *nrank = 0;    

    c = getchar_unlocked();
    while(c != ' '){
        *nvertices = ((*nvertices)*10) + (c - '0');
        c = getchar_unlocked();        
    }

    c = getchar_unlocked();
    while(c != '\n'){
        (*nrank) = ((*nrank)*10) + (c - '0');
        c = getchar_unlocked();
    }
}

int read(){
    char checkAG = getchar_unlocked();
    
    if(checkAG == EOF){
        return 0;
    }

    if(checkAG == 'A'){              //controllo se è un AggiungiGrafo
        while(checkAG != '\n'){
            checkAG = getchar_unlocked();
        }
        return 1;        
    } else{                         //(checkAG == 'T')    controllo che sia la chiamata di TopK
        while(checkAG != '\n'){
            checkAG = getchar_unlocked();
        }
        return -1;
    }
}

void AggiungiGrafo(int *adjm, int *nvertices, nodo_t** l, int *topnow){           //funzione per aggiungere un nuovo grafo
    int weight;

    insmatrix(nvertices, adjm);             //dato che è un AggiungiGrafo, chiamo l'inserimeto dei valori in matrice            
    weight = dijkstra(nvertices, adjm);     //una volta acquisita la matrice, applico l'alg di dijkstra

    if(number < THRESHOLD){                 //se sono sotto la soglia, procedi con il rbt
        insertNode(weight);
    } else{
        max = limitList(*l, topnow);
        if(weight < max){
            *l = sortedInsertion(*l, weight);   //se supero la soglia, allora procedi con le liste
        }
    }

    number++;             //ho finito di leggere un grafo, incremento number

    if(number == THRESHOLD){        //solo per il passaggio, converti il rbt in lista     
        *l = tree2list(ROOT);
    }
}

void insmatrix(int *nvertices, int *adjm){           //funzione per leggere e salvare la matrice
    int i, j;
    char sep = ',';
    char c;

    for(i=0; i<(*nvertices); i++){          //inizializzo la matrice tutta a 0
        for(j=0; j<(*nvertices); j++){
            adjm[i*(*nvertices)+j] = 0;
        }
    }

    for(i=0; i<(*nvertices); i++){          //scorro le righe con la i
        j=0;

        while(j<(*nvertices)){              //scorro le colonne con la j
            
            c = getchar_unlocked();

            if(c != sep && c != '\n'){          //se non è il separatore o newline, leggi i caratteri
                (adjm[i*(*nvertices)+j]) = (adjm[i*(*nvertices)+j])*10 + (c - '0');

            } else if(c == sep){                //se è il separatore, aggiorna la j perché si passa alla prox colonna
                j++;

            } else if(c == '\n'){               //caso particolare, newline. aggiorna j
                j++;
            }
        }   
    }
}

int dijkstra(int *nvertices, int *adjm){
    int dim = *nvertices;
    int dist[dim], visited[dim];
    int nodcount, mindist, nextnode;
    int i, j;
    int sumSP = 0;

    for(i=0; i<dim; i++){               //creo la matrice dei costi
        for(j=0; j<dim; j++){
            if(adjm[i*dim+j] == 0){
                adjm[i*dim+j] =  INFINITY;
            }
        }
    }

    for(i=0; i<dim; i++){                //inizializzo le distanze viste da 0, i vertici precedenti allo 0, nessun nodo visitato
        dist[i] = adjm[STARTNODE*dim+i];
        visited[i] = 0;
    }

    dist[STARTNODE] = 0;        //distanza della partenza da se stessa è 0
    visited[STARTNODE] = 1;     //la partenza è già visitata
    nodcount = 1;               //aggiungo la partenza al conto dei nodi visitati
    nextnode = STARTNODE;       

    while(nodcount < dim-1){
        mindist =  INFINITY;     //minima distanza a infinito

        for(i=0; i<dim; i++){

            if( (dist[i] < mindist) && !visited[i]){     //controllo che la dist sia la min possibile
                mindist = dist[i];                       //e che il nodo non sia già stato visitato
                nextnode = i;              
            }
        }

        visited[nextnode] = 1;          //segno come visitato il nodo raggiunto

        for(i=0; i<dim; i++){           //analizzo tutti i nodi non visitati e controllo se esiste un percorso migliore da nextnode
            if( !visited[i] ){
                if(mindist < (dist[i] - adjm[nextnode*dim+i])){
                    dist[i] = mindist + adjm[nextnode*dim+i];
                }
            }
        }
        nodcount++;                     //incremento il numero di nodi analizzati
    }

    for(i=0; i<dim; i++){         //ciclo per la somma di TUTTI i cammnini minimi del grafo

        if(dist[i] == INFINITY){
            dist[i] = 0;
        }

        sumSP = sumSP + dist[i];
    }

    return sumSP;
}

void insertNode(int value){          //funzione per inserire un nodo nel rbt
    struct node *z;
    
    z = (struct node*) malloc(sizeof(struct node));     //alloco la memoria per un nuovo nodo

    z->weight = value;
    z->num = number;
    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;

    if(ROOT == NULL){          //se la radice è NULL, z diventa la radice
        z->color = BLACK;
        ROOT = z;
        
        return ;
    }

    struct node *x, *y;

    x = ROOT;
    y = NULL;
    
    while(x != NULL){               //inserimento standard del nodo in un classico albero binario
        y = x;

        if ((z->weight) < (x->weight)){
            x = x->left;
        } else{
            x = x->right;
        }
    }

    z->parent = y;

    if((z->weight) < (y->weight)){
        y->left = z;
    } else{
        y->right = z;
    }
    
    z->color = RED;

    fixTree(z);       //chiamo la funzione per fixare l'albero se necessario
}

void leftRotation(struct node *x){      //funzione per la rotazione sx del rbt
    
    if((!x) || (!x->right)){
        return ;
    }

    struct node *y;
    
    y = x->right;           //assegno ad y il child dx di x
    x->right = y->left;     //assegno al child dx di x, il subtree sotto il child sx di y

    if(x->right != NULL){       //aggiorno il parent del child dx di x
        x->right->parent = x;
    }

    if(y != NULL){
        y->parent = x->parent;      //aggiorno il parent di y
    }

    if(x->parent == NULL){             //se il parent di x è NULL, rendo y la radice
        ROOT = y;
    } else if(x == (x->parent->left)){   //altrimenti salvo y al posto della x
        x->parent->left = y;     
    } else{
        x->parent->right = y;
    }

    y->left = x;        //rendo x il child sx di y
    if(x != NULL){
        x->parent = y;      //aggiorno il parent di x
    }
}

void rightRotation(struct node *y){             //funzione per la rotazione dx del rbt

    if(!y || !y->left){
        return ;
    }

    struct node *x;

    x = y->left;           //assegno ad x il child sx di y
    y->left = x->right;    //assegno al child sx di y, il subtree sotto il child dx di x

    if(x->right != NULL){      //aggiorno il parent del child dx di x
        x->right->parent = y;
    }

    if(x != NULL){
        x->parent = y->parent;      //aggiorno il parent di x
    }

    if(x->parent == NULL){             //se il parent di x è NULL, rendo x la radice
        ROOT = x;
    } else if(y == y->parent->right){   //altrimenti salvo x al posto della y
        y->parent->right = x;
    } else{
        y->parent->left = x;
    }

    x->right = y;        //rendo y il child dx di x
    if(y != NULL){
        y->parent = x;       //aggiorno il parent di y
    }
}

void fixTree(struct node *z){

    while((z != ROOT) && (z != ROOT->left) && (z != ROOT->right) && (z->parent->color == RED)){
        struct node *y;         //itero finche z non è la radice e il parent di z è RED

        if((z->parent) && (z->parent->parent) && ((z->parent) == (z->parent->parent->left))){       //trovo l'uncle e lo salvo in y
            y = z->parent->parent->right;
        } else{
            y = z->parent->parent->left;
        }

        if(!y){                                    //se y non estste
            z = z->parent->parent;                      //z diventa il grandparent di z

        } else if(y->color == RED) {               //se uncle è RED:
            y->color = BLACK;                           //1. cambio il colore del parent e dell'uncle in BLACK
            z->parent->color = BLACK;
            z->parent->parent->color = RED;             //2. cambio il colode del grandparent in RED
            z = z->parent->parent;                      //3. z diventa il grandparent di z

        } else{                                     //se uncle è BLACK, ci sono 4 casi (LL, LR, RL, RR)

            if((z->parent == z->parent->parent->left) && (z == z->parent->left)){          //caso Left-Left (LL)                                                       
                z->parent->color = BLACK;               //1. scambia colore del parent e del grandparent
                z->parent->parent->color = RED;
                rightRotation(z->parent->parent);       //2.rotazione a dx del grandparent
            }

            if((z->parent) && (z->parent->parent) && (z->parent == z->parent->parent->left) && (z == z->parent->right)){  //caso Left-Right (LR)
                z = z->parent;                          
                leftRotation(z);                        //1. effettuo una rotazione sx del parent
                z->parent->color = BLACK;               //2. scambia i colori del nodo corrente e del grandparent
                z->parent->parent->color = RED;
                rightRotation(z->parent->parent);       //3. effettuo una rotazione dx del grandparent
            }

            if((z->parent) && (z->parent->parent) && (z->parent == z->parent->parent->right) && (z == z->parent->right)){  //caso Right-Right (RR)
                z->parent->color = BLACK;                   //1. scambio i colori del parent e del grandparent
                z->parent->parent->color = RED;
                leftRotation(z->parent->parent);            //2. effettuo una rotazione sx del grandparent
            }

            if((z->parent) && (z->parent->parent) && (z->parent == z->parent->parent->right) && (z == z->parent->left)){    //caso Right-Left (RL)
                z = z->parent;                              
                rightRotation(z);                               //1. effettuo una rotazione dx del parent
                z->parent->color = BLACK;                       //2. scambio il colore del nodo corrente e del grandparent
                z->parent->parent->color = RED;
                leftRotation(z->parent->parent);                //3. effettuo una rotazione sx del grandparent
            }
        }
    }

    ROOT->color = BLACK;         //la radice deve essere sempre BLACK
}

void TopKrbt(struct node *z, int *topnow){             //funzione per stampare la TopK tra i nodi del rbt

    if(z == NULL){                  //se il z è NULL non fare nulla
        return ;
    }

    TopKrbt(z->left, topnow);      //arrivo nell'estremo sx dell'albero

    if(u == (*topnow)){
        return ;                //se u è arrivato a k, ferma la ricorsione
    } else{
        u++;                    //altrimenti continua ad incrementare u
    }

    if(u != 1){
        printf(" ");
    }
    printf("%d", z->num);       //stampa del numero del grafo

    TopKrbt(z->right, topnow);     //esploro anche il ramo dx di ogni nodo
}

void TopKlist(nodo_t* l, int *topnow){           //funzione per stampare la lista
    int i=0;

    while((l != NULL) && (i<(*topnow))){      //stampa finché arrivi alla fine della lista o raggiungi k
        if(u == 0){
            printf("%d", l->num);
            u++;
        } else{
            printf(" %d", l->num);
        }
        l = l->next;
        i++;
    }
}

nodo_t* sortedInsertion(nodo_t* head, int data){        //funzione per l'inserimento in ordine nella lista
    if ((head == NULL) || ((head->weight) > data)){
        return headInsertion(head, data);
    }

    head->next = sortedInsertion(head->next, data);
    return head;
}

nodo_t* headInsertion(nodo_t* list, int data){          //funzione per l'inserimento in testa della lista
    nodo_t *tmp;
    tmp = malloc(sizeof(nodo_t));

    tmp->num = number;
    tmp->weight = data;
    tmp->next = list;
    list = tmp;

    return list;
}

nodo_t* tree2list(struct node *root){        //funzione per convertire il rbt in una lista
    nodo_t *list1, *list2;
    
    if(root->left != NULL){         //scorro tutto il ramo sx del rbt
        list1 = tree2list(root->left);
    } else{
        list1 = NULL;
    }

    if(root->right != NULL){        //scorro tutto il ramo dx del rbt, analizzando tutti i sinistri
        list2 = tree2list(root->right);
    } else{
        list2 = NULL;
    }

    nodo_t *middle = malloc(sizeof(nodo_t));        //alloco un nuovo nodo della lista
    middle->weight = root->weight;
    middle->num = root->num;

    middle->next = list2;       //middle non può essere NULL, aggancia la list2, va bene anche se list2 è NULL
    if (!list1){
        return middle;      //nulla da agganciare prima
    }

    nodo_t *last = list1;
    while (last->next){
        last=last->next;        //arriva alla fine della list1
    }
    last->next = middle;        //aggancia middle+list2 alla fine della list1

    return list1;
}

int limitList(nodo_t* l, int *topnow){           //funzione per trovare il k-esimo elemento della lista
    int i = 0;

    while(l != NULL && i<*topnow){
        l = l->next;
        i++;
    }

    return l->weight;
}

nodo_t* destroyList(nodo_t* head){         //funzione per liberare la memoria allocata per lista
    nodo_t* tmp;

    while(head!= NULL){
        tmp = head;
        head = head->next;
        free(tmp);
    }

  return NULL;
}