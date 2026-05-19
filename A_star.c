/* ΜΙΧΑΗΛ ΣΥΜΕΩΝΙΔΗΣ ΑΜ:2022202200202*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 20  // Estw max megethos 20 gia grammes/sthles
#define COLS 20

typedef struct Node { // Domh Node 
    int x, y;        // Syntetagmenes komboy
    int g, h, f;     // g = Kostos gia na ftasoyme ston kombo apto start, h = ektimomenh apostash apo Manhattan, and total f = g+h (synoliko kostos)
    struct Node* parent; // Deikths ston kombo parent p xrhsimopoieitai gia anakataskevh
} Node;

typedef struct PriorityQueue { // Oura proteraiothtas
    Node** nodes; // Pinakas deiktwn se domh Node
    int size; // Arithmos kombwn sthn Oura
    int capacity; // Megisth xwritikothta Ouras prin thn anadiorthwsh
} PriorityQueue;

void swap(Node** a, Node** b);
void heapify(PriorityQueue* prq, int index);
void heapInsert(PriorityQueue* prq, Node* node);
Node* heapExtract(PriorityQueue* prq);
int manhattan(int x1, int y1, int x2, int y2);
void generateGrid(int grid[ROWS][COLS], int rows, int cols, int obstaclePercentage);
void a_star(int grid[ROWS][COLS], int startX, int startY, int goalX, int goalY, int rows, int cols);
void reconstruct_path(Node* goal);

int manhattan(int x1, int y1, int x2, int y2) { // Xrhsimopoieitai ston A* gia na broume ektimomenh apostash
    return abs(x1 - x2) + abs(y1 - y2); // Apolyth timh twn prajewn
}

void swap(Node** a, Node** b) { // Antimetathetei times 2 kombwn
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(PriorityQueue* prq, int index) { // Ejasfalizoyme oti h oura proteraiothtas isxyei 
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < prq->size && prq->nodes[left]->f < prq->nodes[smallest]->f) { // An h timh f enos komboy einai mikroterh apo ekeinh ton paidiwn toy, tote ta antallazei
        smallest = left;
    }
    if (right < prq->size && prq->nodes[right]->f < prq->nodes[smallest]->f) {
        smallest = right;
    }
    if (smallest != index) {
        swap(&prq->nodes[index], &prq->nodes[smallest]);
        heapify(prq, smallest); // Diathroyme domh swrou
    }
}

void heapInsert(PriorityQueue* prq, Node* node) { // Synarthsh eisagwghs komboy sthn Oura
    if (prq->size == prq->capacity) { // Ama yperboume to capacity t
        prq->capacity *= 2; // Tote allazoyme to megethos ths Ouras Proteraiothtas
        prq->nodes = (Node**)realloc(prq->nodes, prq->capacity * sizeof(Node*));
    }

    prq->nodes[prq->size] = node; // Eisagoyme neo kombo sto telos
    int i = prq->size;
    prq->size++;

    while (i > 0 && prq->nodes[(i - 1) / 2]->f > prq->nodes[i]->f) { // Checkarei ama to kostos toy parent node[(i-1)/2] einai megalytero apto current node[i]
        swap(&prq->nodes[(i - 1) / 2], &prq->nodes[i]); // An nai allazei thesh toys komboys
        i = (i - 1) / 2;
    }
}

Node* heapExtract(PriorityQueue* prq) { // Ejagoyme ton kombo me to mikrotero synoliko kostos f apthn oura
    if (prq->size == 0) {
        return NULL; // Adeia oura
    }

    Node* root = prq->nodes[0]; // Ejagoyme riza (mikrotero synoliko kostos f)

    prq->nodes[0] = prq->nodes[prq->size - 1]; // Antikathistoyme th riza me ton teleftaio kombo
    prq->size--;

    heapify(prq, 0); // Diathroyme th domh ths swrou

    return root;
}

void generateGrid(int grid[ROWS][COLS], int rows, int cols, int obstaclePercentage) { // Paragoyme tyxaia orthogwnia
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((rand() % 100) < obstaclePercentage) {
                grid[i][j] = 1; // 1 gia kathe ebodio
            } else {
                grid[i][j] = 0; // 0 gia keno
            }
        }
    }
}

void a_star(int grid[ROWS][COLS], int startX, int startY, int goalX, int goalY, int rows, int cols) { // Xrhsimopoioume alg A* gia na broume to syntomotero monopati
    Node start = {startX, startY, 0, manhattan(startX, startY, goalX, goalY), 0, NULL}; // Arxikopoioyme ton prwto kombo
    start.f = start.g + start.h;
    
    PriorityQueue prq; // Dhmiourgoyme oura proteraiothtas
    prq.nodes = (Node**)malloc(ROWS * COLS * sizeof(Node*)); // Dynamikh diaxeirhsh mnhmhs
    prq.size = 0;
    prq.capacity = ROWS * COLS;

    heapInsert(&prq, &start);
    
    while (prq.size > 0) {
        Node* current = heapExtract(&prq); // Afairei epanalambanomena komboys me to mikrotero kosts f apthn oura.

        if (current->x == goalX && current->y == goalY) { //An o kombos einai o stoxos
            printf("Brethike monopati !\n");
            reconstruct_path(current); // tote anakataskevazei th diadromh
            return;
        }

        int dx[] = {0, 0, -1, 1}; // Diaforetika koitame kai toys geitonikoys komboys (panw, katw, aristera, dejia)
        int dy[] = {-1, 1, 0, 0};
        
        for (int i = 0; i < 4; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];
            
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] == 0) {
                Node* neighbor = (Node*)malloc(sizeof(Node));
                neighbor->x = nx;
                neighbor->y = ny;
                neighbor->g = current->g + 1;
                neighbor->h = manhattan(nx, ny, goalX, goalY);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = current;
                heapInsert(&prq, neighbor); // Kai toys eisagoyme sthn oura proteraiothtas
            }
        }
    }
    printf("De brethike monopati\n");
}

void reconstruct_path(Node* goal) { // Sth synarthsh afth anakataskevazei to monopati 
    Node* current = goal;
    while (current != NULL) {
        printf("(%d, %d) <- ", current->x, current->y); // Kai ektypwnei th diadromh
        current = current->parent;
    }
    printf("Arxh\n");
}

int main() {
    int grid[ROWS][COLS];
    srand(time(NULL)); // Arxizoyme to random number generator

    int rows = 10 + rand() % 11; // Tyxaies grammes apo 10 ews 20 (de katalaba an ekei poy lete p.x. 10x10 kai 20x20 thelate toso h apla htan paradeigma oi diastaseis)
    int cols = 10 + rand() % 11; // Tyxaies sthles apo 10 ews 20 (an den ithele apo 10x10 ews 20x20 tote apla kanw rows/cols = 1 + rand()% ROWS/COLS)

    int obstaclePercentage = 10 + rand() % 81; // Bazoyme to pososto ebodiwn na einai tyxaio apo 10% ews 90%

    printf("Megethos plegmatos: %d x %d\n", rows, cols);
    printf("Pososto ebodiwn: %d%%\n", obstaclePercentage);

    generateGrid(grid, rows, cols, obstaclePercentage);

    int startX = 0, startY = 0; // Arxikopoioyme arxikes theseis kai orizoyme to telos
    int goalX = rows - 1, goalY = cols - 1;

    a_star(grid, startX, startY, goalX, goalY, rows, cols);
    return 0;
}