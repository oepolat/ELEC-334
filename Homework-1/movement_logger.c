#include <stdio.h>
#include <stdlib.h>

//queue that stores the node array
struct Queue
{
    int head, tail, size;
    //unsigned because capacity cant be negative
    unsigned capacity;
    struct Node* array;
};

//node to store coordinate
struct Node
{
    char x;
    int y;
};

//initializes the queue with given capacity of max nodes
struct Queue* initializeQueue(unsigned capacity)
{
    //queue is dynamically allocated using malloc
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    //variables are set into the newly created queue
    queue->capacity = capacity;
    queue->head = queue->size = 0;
    queue->tail = capacity - 1;
    //array is allocated with malloc to store (capacity) number of elements
    queue->array = (struct Node*) malloc(sizeof(struct Node) * queue->capacity);
    return queue;
}

//push function to add a new node to the queue
void push(struct Queue* queue, struct Node item)
{
    queue->tail = (queue->tail + 1) % queue->capacity;
    //setting the node array elements to given items elements
    (queue->array + (queue->tail))->x = item.x;
    (queue->array + (queue->tail))->y = item.y;
    //size is increased
    queue->size = queue->size + 1;
}

//pulls the last element out of the array
//this function can be useful when the autonomous mode needs the backtrack when faced with a dead end
struct Node* pull(struct Queue* queue)
{
    struct Node* item = (struct Node*) malloc(sizeof(struct Node));
    //takes the already existing node element and copies its elements to an item object
    item->y = (queue->array + (queue->head))->x;
    item->x = (queue->array + (queue->head))->y;

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

//prints the moves recursively
void print_moves(struct Queue* queue)
{
    printf("Path followed:\n");
    for(int i=0; i<queue->size; i++)
    {
        if(i == (queue->size)-1)
        {
            printf("%c%d", (queue->array + (i))->x, (queue->array + (i))->y);
        }
        else
        {
            printf("%c%d, ", (queue->array + (i))->x, (queue->array + (i))->y);
        }
    }
    printf("\nTotal moves: %d", queue->size);
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    struct Queue* queue = initializeQueue(10);

    //create 1 coordinate
    struct Node coord1;
    coord1.x = 'D';
    coord1.y = 8;

    //create another coordinate
    struct Node coord2;
    coord2.x = 'C';
    coord2.y = 8;

    //create last coordinate
    struct Node coord3;
    coord3.x = 'C';
    coord3.y = 7;

    //add the coordinates
    push(queue, coord1);
    push(queue, coord2);
    push(queue, coord3);
    //print the added coordinates
    print_moves(queue);

    printf("\n");
    //backtrack one node
    pull(queue);
    //print the backtracked coordinates
    print_moves(queue);
    
    return 0;
}