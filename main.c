#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT_LENGTH 16384

#define ADD_STATION_CMD 0
#define ADD_CAR_CMD 1
#define SCRAP_CAR_CMD 2
#define PLAN_PATH 3
#define SCRAP_STATION 4

typedef struct Car {
    long long range;
    struct Car* parent;
    struct Car* left;
    struct Car* right;
} Car;

typedef struct CarTree {
    struct Car* root;
    long long maxRange;
} CarTree;

typedef struct Station {
    long long distance;
    long long backwardsReachedBy;
    short int pathStep;
    struct CarTree cars;
    struct Station* prevInPath;
    struct Station* nextInQueue;
    struct Station* next;
    struct Station* prev;
    struct Station* parent;
    struct Station* left;
    struct Station* right;
} Station;

Station* stationTreeRoot = NULL;
Station* stationListRoot = NULL;
Station* queueHead = NULL;
Station* queueTail = NULL;
Station* lastStationFetched = NULL;

void printStationTree(Station* s) {
    if (s == NULL) return;
    printStationTree(s->left);
    printf("(%lld, %lld) ", s->distance, s->cars.maxRange);
    // printf("%p(%lld, %lld) ", s, s->distance, s->cars.maxRange);
    printStationTree(s->right);
}

bool isInQueue(Station* station) {
    if (station == NULL) return false;
    if (station == queueTail || station->nextInQueue != NULL) return true;
    return false;
}

void enqueue(Station* station) {
    if (queueHead == NULL) {
        queueHead = station;
        queueTail = station;
        return;
    }
    queueTail->nextInQueue = station;
    queueTail = station;
}

Station* dequeue() {
    if (queueHead == NULL) return NULL;
    Station* head = queueHead;
    queueHead = queueHead->nextInQueue;
    head->nextInQueue = NULL;
    return head;
}

void dequeueAll() {
    while(queueHead != NULL) dequeue();
}

// s is the first character of a string formatted like this: "342 23 54 123 43"
// the function returns the first number in the string s as an integer
// it also sets the nextInt pointer the the beginning of the next int
long long getNextInt(char* s, char** nextInt) {
    long long res = 0;
    char cur = *s;
    while(cur != ' ' && cur != '\n' && cur != EOF && cur != '\0') {
        cur = cur - 48;
        res = res * 10 + (long long) cur;
        s++;
        cur = *s;
    }
    if (cur == ' ') {
        *nextInt = s + 1;
    } else *nextInt = NULL;
    return res;
}

Station* stationMalloc(long long distance) {
    Station* station = (Station*)malloc(sizeof(Station));
    station->distance = distance;
    station->backwardsReachedBy = 0;
    station->pathStep = 0;
    station->cars.root = NULL;
    station->cars.maxRange = -1;
    station->next = NULL;
    station->prev = NULL;
    station->nextInQueue = NULL;
    station->prevInPath = NULL;
    station->parent = NULL;
    station->left = NULL;
    station->right = NULL;
    return station;
}

Car* carMalloc(long long range) {
    Car* car = (Car*)malloc(sizeof(Car));
    car->range = range;
    car->parent = NULL;
    car->left = NULL;
    car->right = NULL;
    return car;
}

void carTreeInsert(CarTree* tree, Car* car) {
    if (tree->maxRange < car->range) tree->maxRange = car->range;
    Car* y = NULL;
    Car* x = tree->root;
    while (x != NULL) {
        y = x;
        if (car->range < x->range) x = x->left;
        else x = x->right;
    }
    car->parent = y;
    if (y == NULL) tree->root = car;
    else if (car->range < y->range)
        y->left = car;
    else y->right = car;
}

Car* carTreeMinimum(Car* x) {
    while (x->left != NULL) x = x->left;
    return x;
}

Car* carTreeMaximum(Car* x) {
    while (x->right != NULL) x = x->right;
    return x;
}

Car* getCar(CarTree* tree, long long range) {
    Car* x = tree->root;
    while (x != NULL && x->range != range) {
        if (range < x->range) x = x->left;
        else x = x->right;
    }
    return x;
}

Car* carTreeSuccessor(Car* x) {
    if (x->right != NULL) return carTreeMinimum(x->right);
    
    Car* y = x->parent;
    while (y != NULL && x == x->parent->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

bool deleteCar(CarTree* tree, long long range) {

    Car* z = getCar(tree, range);

    if (z == NULL) return false;

    Car* y = NULL;
    Car* x = NULL;

    if (z->left == NULL || z->right == NULL)
        y = z;
    else 
        y = carTreeSuccessor(z);

    if (y->left != NULL) x = y->left;
    else x = y->right;

    if (x != NULL)
        x->parent = y->parent;

    if (y->parent == NULL)
        tree->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;

    if (y != z) z->range = y->range;
    
    free(y);

    if (range == tree->maxRange) {
        if (tree->root == NULL) tree->maxRange = -1;
        else {
            tree->maxRange = (carTreeMaximum(tree->root))->range;
        }
    }

    return true;
}

void freeCarTree(Car* car) {
    if (car == NULL) return;
    freeCarTree(car->left);
    freeCarTree(car->right);
    free(car);
    return;
}

void stationTreeInsert(Station* station) {
    Station* y = NULL;
    Station* x = stationTreeRoot;
    while (x != NULL) {
        y = x;
        if (station->distance < x->distance) x = x->left;
        else x = x->right;
    }
    station->parent = y;
    if (y == NULL) stationTreeRoot = station;
    else if (station->distance < y->distance)
        y->left = station;
    else y->right = station;
}

Station* stationTreeMinimum(Station* x) {
    while (x->left != NULL) x = x->left;
    return x;
}

Station* stationTreeMaximum(Station* x) {
    while (x->right != NULL) x = x->right;
    return x;
}

Station* getStation(long long distance) {
    if (lastStationFetched && distance == lastStationFetched->distance) return lastStationFetched;
    Station* x = stationTreeRoot;
    while (x != NULL && x->distance != distance) {
        if (distance < x->distance) x = x->left;
        else x = x->right;
    }
    if (x != NULL) lastStationFetched = x;
    return x;
}

Station* stationTreeSuccessor(Station* x) {
    if (x->right != NULL) return stationTreeMinimum(x->right);
    
    Station* y = x->parent;
    while (y != NULL && x == x->parent->right) {
        x = y;
        y = y->parent;
    }

    return y;
}

Station* stationTreePredecessor(Station* x) {
    if (x->left != NULL) return stationTreeMaximum(x->left);
    
    Station* y = x->parent;
    while (y != NULL && x == x->parent->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

bool deleteStation(long long distance) {

    Station* z = getStation(distance);

    if (z == NULL) return false;

    if (z == lastStationFetched) lastStationFetched = NULL;

    Station* y = NULL;
    Station* x = NULL;

    if (z->left == NULL || z->right == NULL)
        y = z;
    else 
        y = stationTreeSuccessor(z);

    if (y->left != NULL) x = y->left;
    else x = y->right;

    if (x != NULL)
        x->parent = y->parent;

    if (y->parent == NULL)
        stationTreeRoot = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;

    if (y != z) {
        freeCarTree(z->cars.root);
        z->distance = y->distance;
        z->backwardsReachedBy = y->backwardsReachedBy;
        z->cars = y->cars;
    }

    free(y);

    return true;
}

void addStationParser(char* parameters) {

    long long distance = getNextInt(parameters, &parameters);
    
    // ignore number of cars 
    getNextInt(parameters, &parameters);
    if (getStation(distance) != NULL) {
        printf("non aggiunta\n");
        return;
    }

    Station* newStation = stationMalloc(distance);

    while (parameters != NULL) {
        long long range = getNextInt(parameters, &parameters);
        Car* car = carMalloc(range);
        carTreeInsert(&newStation->cars, car);
    }
    stationTreeInsert(newStation);
    printf("aggiunta\n");

}

bool addCarToStation(long long stationDistance, long long carRange) {
    Station* station = getStation(stationDistance);
    if (station == NULL) return false;
    Car* car = carMalloc(carRange);
    carTreeInsert(&station->cars, car);
    return true;
}

void addCarParser(char* parameters) {
    long long distance = getNextInt(parameters, &parameters);
    long long range = getNextInt(parameters, &parameters);
    bool added = addCarToStation(distance, range);
    if (added) printf("aggiunta\n");
    else printf("non aggiunta\n");
}

void scrapStationParser(char* parameters) {
    unsigned long distance = getNextInt(parameters, &parameters);
    bool deleted = deleteStation(distance);

    if (deleted)  {
        printf("demolita\n");
    } else 
        printf("non demolita\n");
}

void scrapCarParser(char* parameters) {
    unsigned long distance = getNextInt(parameters, &parameters);
    long long range = getNextInt(parameters, &parameters);
    bool deleted = false;

    Station* station = getStation(distance);
    if (station != NULL)
        deleted = deleteCar(&station->cars, range);

    if (deleted)
        printf("rottamata\n");
    else
        printf("non rottamata\n");
}

unsigned long* planPathForward(Station* start, Station* goal, int* steps) {
    enqueue(start);
    // Station* next = start->next;
    Station* next = stationTreeSuccessor(start);
    start->pathStep = 0; // PRIMA NON CERA
    unsigned long maxDistVisited = start->distance;
    while (queueHead != NULL) {
        Station* cur = dequeue();

        if (cur == goal) {
            *steps = cur->pathStep;
            unsigned long* path = (unsigned long*)malloc(sizeof(unsigned long) * ((*steps) + 1));
            for (int i = *steps; i >= 0; i--) {
                path[i] = cur->distance;
                cur = cur->prevInPath;
            }
            dequeueAll();
            return path;
        }
        
        long long curRange = cur->cars.maxRange;
        // Station* next = cur->next;
        
            
        while (next != NULL && abs(next->distance - cur->distance) <= curRange) {
            // if (!isInQueue(next)) {
            if (maxDistVisited < next->distance) {
                maxDistVisited = next->distance;
                enqueue(next);

                next->prevInPath = cur;
                next->pathStep = cur->pathStep + 1;
            }
            next = stationTreeSuccessor(next);
        }
    }
    dequeueAll();
    return NULL;
}

void updatebackwardsReachedBy(Station* rightest, Station* leftest) {
    if (rightest == NULL || leftest == NULL || leftest->distance >= rightest->distance) return;
    // Station* curLeft = rightest->prev;
    Station* curLeft = stationTreePredecessor(rightest);
    Station* curRight = rightest;
    while (curLeft != NULL) {
        unsigned long distance = curRight->distance - curLeft->distance;
        if (curRight->cars.maxRange >= distance) {
            curLeft->backwardsReachedBy = curRight->distance;
            // curLeft = curLeft->prev;
            curLeft = stationTreePredecessor(curLeft);
        } else
            curRight = stationTreePredecessor(curRight);
            // curRight = curRight->prev;

        if (curRight == curLeft) curLeft = stationTreePredecessor(curLeft);
        // if (curRight == curLeft) curLeft = curLeft->prev;
    }
}

// approach of swapping start and goal and treating kinda like a forwarad path plan + backwardsReachedBy
unsigned long* planPathBackward(Station* start, Station* goal, int* steps) {

    updatebackwardsReachedBy(start, goal);

    Station *tmp = start;
    start = goal;
    goal = tmp;
    enqueue(start);
    // Station* next = start->next;
    Station* next = stationTreeSuccessor(start);
    start->pathStep = 0; // PRIMA NON CERA
    unsigned long maxDistVisited = start->distance;
    while (queueHead != NULL) {
        Station* cur = dequeue();

        if (cur == goal) {
            *steps = cur->pathStep;
            unsigned long* path = (unsigned long*)malloc(sizeof(unsigned long) * ((*steps) + 1));
            for (int i = 0; i <= *steps; i++) {
                path[i] = cur->distance;
                cur = cur->prevInPath;
            }
            dequeueAll();
            return path;
        }
        
        // Station* next = NULL;
        // next = cur->next;
            
        while (next != NULL && next->distance <= cur->backwardsReachedBy) {
            if (abs(next->distance - cur->distance) <= next->cars.maxRange && maxDistVisited < next->distance) {
                maxDistVisited = next->distance;
                enqueue(next);

                next->prevInPath = cur;
                next->pathStep = cur->pathStep + 1;
            }
            // next = next->next;
            next = stationTreeSuccessor(next);
        }
       
    }
    dequeueAll();
    return NULL;
}

void planPathParser(char* parameters) {

    unsigned long startingStationDist = getNextInt(parameters, &parameters);
    unsigned long goalStationDist = getNextInt(parameters, &parameters);

    if (startingStationDist == goalStationDist) {
        printf("%ld\n", startingStationDist);
        return;
    }

    Station* startingStation = getStation(startingStationDist);
    Station* goalStation = getStation(goalStationDist);

    // return;
    int steps = 0;
    unsigned long* path = NULL;
    if (startingStationDist < goalStationDist)
        path = planPathForward(startingStation, goalStation, &steps);
    else
        path = planPathBackward(startingStation, goalStation, &steps);

    if (path == NULL) printf("nessun percorso\n");
    else {
        printf("%ld", path[0]);
        for(int i = 1; i <= steps; i++)
            printf(" %ld", path[i]);
        printf("\n");
    }

    free(path);
}

unsigned char getCMD(char* command) {
    if (command[0] == 'a') {
        if (command[9] == 's') return ADD_STATION_CMD;
        return ADD_CAR_CMD;
    }
    if (command[0] == 'r') return SCRAP_CAR_CMD;
    if (command[0] == 'p') return PLAN_PATH;
    return SCRAP_STATION;
}

void commandParser(char* command) {
    unsigned char cmd = getCMD(command);
    // if (cmd == ADD_STATION_CMD) printf("AGGIUNGI STAZIONE\n");
    if (cmd == ADD_STATION_CMD) addStationParser(&command[18]);
    // if (cmd == ADD_CAR_CMD) printf("AGGIUNGI AUTO\n");
    if (cmd == ADD_CAR_CMD) addCarParser(&command[14]);
    // if (cmd == SCRAP_CAR_CMD) printf("ROTTAMA AUTO\n");
    if (cmd == SCRAP_CAR_CMD) scrapCarParser(&command[13]);
    // if (cmd == PLAN_PATH) printf("PIANIFICA PERCORSO\n");
    if (cmd == PLAN_PATH) planPathParser(&command[19]);
    // if (cmd == SCRAP_STATION) printf("DEMOLISCI STAZIONE\n");
    if (cmd == SCRAP_STATION) scrapStationParser(&command[19]);
}
 
int main(void) {

    char command[MAX_INPUT_LENGTH];

    while (fgets(command, MAX_INPUT_LENGTH, stdin) != NULL) {
        commandParser(command);
    }

    return 0;

}