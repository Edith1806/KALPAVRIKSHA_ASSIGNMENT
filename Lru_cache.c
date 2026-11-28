#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include <errno.h>
#include <limits.h>

#define STRING_LENGTH 51

typedef struct Node
{
    int key;
    char data[STRING_LENGTH];
    struct Node *prev;
    struct Node *next;
}Node;

typedef struct Queue
{
    Node *front;
    Node *rear;
}Queue;

typedef struct HashMap
{
    int key;
    Node *q_node;
    struct HashMap *next;
}hashMap;

typedef struct LRUcache
{
    int capacity;
    int size;
    Queue queue;
    hashMap **table;
    int table_size; 
}LRUcache;

LRUcache * createCache(int capacity)
{
   if(capacity <= 0)
    return NULL;
   LRUcache *cache = malloc(sizeof(*cache));
   if(cache == NULL)
        return NULL;
   cache->capacity = capacity;
   cache->queue.front = NULL;
   cache->queue.rear = NULL;
   cache->size = 0;
   cache->table_size = capacity * 2;
   cache->table = calloc(cache->table_size, sizeof(hashMap *));
   if(cache->table == NULL)
   {
    free(cache);
    return NULL;
   }
   return cache;
}

Node *createNode(int key, const char *data)
{
    Node *newNode = malloc(sizeof(*newNode));
    if(newNode == NULL)
        return NULL;

    if(data == NULL || strlen(data) >= sizeof(newNode->data))
    {
        free(newNode);
        return NULL;
    }
    strcpy(newNode->data, data);
    newNode->key = key;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void addToHead(Queue *queue, Node *newNode)
{
   if(queue == NULL || newNode == NULL) return;
   if(queue->front == NULL)
   {
    queue->front = newNode;
    queue->rear = newNode;
    return;
   }

   newNode->next = queue->front;
   queue->front->prev = newNode;
   queue->front = newNode;
}

void removeNode(Queue *queue, Node *node)
{
    if(queue == NULL || node == NULL)
        return;
    
    if(node->next == NULL && node->prev == NULL)
    {
        queue->front = NULL;
        queue->rear = NULL;
    }

    else if(node->next == NULL)
    {
        node->prev->next = NULL;
        queue->rear = node->prev;
    }

    else if(node->prev == NULL)
    {
        queue->front = node->next;
        queue->front->prev = NULL;
    }

    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
}

void moveNode(Queue *queue, Node *node)
{
    if(node == NULL || queue == NULL)
        return;
    
    if(node == queue->front)
        return;
    
    removeNode(queue, node);
    addToHead(queue, node);   
}

int hashFunction(LRUcache *cache, int key)
{
    int index = key % cache->table_size;
    if(index < 0)
        index += cache->table_size;

    return index;
}

hashMap *createHashEntry(int key, Node *q_node)
{
    hashMap *entry = malloc(sizeof(*entry));

    if(entry == NULL)
        return NULL;

    entry->key = key;
    entry->q_node = q_node;
    entry->next = NULL;

    return entry;
}

void insertHashMap(LRUcache *cache, hashMap *entry)
{
    int index = hashFunction(cache, entry->key);

    entry->next = cache->table[index];
    cache->table[index] = entry;
}

Node *searchHashMap(LRUcache *cache, int key)
{
    if(cache == NULL || cache->table == NULL)
        return NULL;
    int bucket_index = hashFunction(cache, key);
    hashMap *current = cache->table[bucket_index];
    while(current != NULL)
    {
        if(current->key == key)
            return current->q_node;
        current = current->next;
    }
    return NULL;
}

bool RemoveHashMap(LRUcache *cache, int key)
{
    if(cache == NULL) return false;
    int bucket_index = hashFunction(cache, key);
    hashMap *current = cache->table[bucket_index];
    hashMap *prev = NULL;
    while(current != NULL)
    {
        if(current->key == key)
        {
            if(prev == NULL)
            {
                cache->table[bucket_index] = current->next;
                free(current);
            }
            else
            {
                prev->next = current->next;
                free(current);
            }
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

char *get(LRUcache *cache, int key)
{
    if(cache == NULL) return NULL;
    Node *node = searchHashMap(cache, key);
    if(node == NULL)
        return NULL;
   moveNode(&cache->queue, node);
   return node->data;
}

void put(LRUcache *cache, int key, const char *data)
{
    if(cache == NULL || data == NULL || strlen(data) >= STRING_LENGTH)
        return;
    Node *existing_node = searchHashMap(cache, key);
    if(existing_node != NULL)
    {
        strcpy(existing_node->data, data);
        moveNode(&cache->queue, existing_node);
        return;
    }
    Node *newNode = createNode(key, data);
    if(newNode == NULL)
        return;
    hashMap *newEntry = createHashEntry(key, newNode);

    if(newEntry == NULL)
    {
        free(newNode);
        return;
    }
    if(cache->size == cache->capacity)
    {
        Node *last_node = cache->queue.rear;

        RemoveHashMap(cache, last_node->key);
        removeNode(&cache->queue, last_node);

        free(last_node);
        cache->size--;
    }
    insertHashMap(cache, newEntry);
    addToHead(&cache->queue, newNode);

    cache->size++;
}

void freeCache(LRUcache *cache)
{
    if(cache == NULL)
        return;
    Node *current = cache->queue.front;
    while(current != NULL)
    {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    cache->queue.rear = NULL;
    cache->queue.front = NULL;
    for(int curr = 0; curr < cache->table_size; curr++)
    {
        hashMap *data = cache->table[curr];
        while(data != NULL)
        {
            hashMap *temp = data;
            data = data->next;
            free(temp);
        }
        cache->table[curr] = NULL;
    }

    free(cache->table);
    free(cache);
}

bool parse_int(const char *str, int *out)
{
    if (str == NULL || *str == '\0')
        return false;
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0' || errno == ERANGE || val < INT_MIN || val > INT_MAX)
        return false;

    *out = (int)val;
    return true;
}

bool process_input(char *input, LRUcache **cache)
{
    if (input == NULL)
        return false;

    char *command = strtok(input, " \t\n");
    if (command == NULL)
        return true;

    char *arg1 = strtok(NULL, " \t\n");
    char *arg2 = strtok(NULL, " \t\n");

    if (strcmp(command, "createCache") == 0)
    {
        if(*cache != NULL)
        {
            printf("cache already exist!\n");
            return true;
        }
        int capacity;
        if (!parse_int(arg1, &capacity) || capacity < 1 || capacity > 1000)
        {
            printf("Invalid Capacity! Must be a number between 1 and 1000.\n");
            return true;
        }
        *cache = createCache(capacity);
        if(*cache == NULL)
        {
            printf("Cahce creation unsuccessful!\n");
            return false;
        }
    }
    else if (strcmp(command, "put") == 0)
    {
        int key;
        if (*cache == NULL || !parse_int(arg1, &key) || arg2 == NULL)
        {
            printf("Invalid put command! Usage: put <int_key> <value>\n");
            return true;
        }
        put(*cache, key, arg2);
    }
    else if (strcmp(command, "get") == 0)
    {
        int key;
        if (*cache == NULL || !parse_int(arg1, &key))
        {
            printf("Invalid get command! Usage: get <int_key>\n");
            return true;
        }
        char *result = get(*cache, key);
        printf("%s\n", result == NULL ? "NULL" : result);
    }
    else if (strcmp(command, "exit") == 0)
    {
        return false;
    }
    else
    {
        printf("Invalid command!\n");
    }

    return true;
}


int main()
{
    LRUcache *cache = NULL;
    char input[256];
    while(true)
    {
        if(fgets(input, sizeof(input), stdin) == NULL)
            break;
        if(!process_input(input, &cache))
            break;
    }
    freeCache(cache);
    return 0;
}