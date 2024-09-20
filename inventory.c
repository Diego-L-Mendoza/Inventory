#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Author: Diego Mendoza
//Date: 4/16/2024
//Assignment: ProgrammingAssignment_5

#define MAXLEN 19
#define TABLESIZE 300007

int complexity = 0;

typedef struct item 
{
    char name[MAXLEN+1];
    int quantity;
    int saleprice;
} item;

typedef struct node 
{
    item* iPtr;
    struct node* next;
} node;

typedef struct hashtable 
{
    node** lists;
    int size;
} hashtable;

int hashfunc(char* word, int size) {
    int len = strlen(word);
    int res = 0;
    for (int i=0; i<len; i++)
    res = (1151*res + (word[i]-'a'))%size;
    return res;
}

// Search function in the hashtable
item* search(hashtable* ht, char* name, int* complexity) {
    // Calculate the index of the item
    int index = hashfunc(name, ht->size);
    // Get the list at the index
    node* list = ht->lists[index];

    if(list == NULL) 
    {
        // If the list is empty, return NULL and add 1 to complexity
        (*complexity)++;
        return NULL;
    }
    
    
    while(list != NULL) 
    {
        // Iterate through the list and add to complexity
        (*complexity)++;
        if(strcmp(list->iPtr->name, name) == 0) 
        {
            return list->iPtr;
        }
        list = list->next;
        
    }
    //add complexity after all the iterations
    (*complexity)++;
    return NULL; // If the item is not found
}
int buy(hashtable* hTable, char* name, int quantity, int price, int totalcash, FILE* out)
{
    //search for the item
    item* i = search(hTable, name, &complexity);
    //if the item is not found, create a new item and add it to the hashtable
    if(i == NULL)
    {
        item* newItem = malloc(sizeof(item));
        strcpy(newItem->name, name);
        newItem->quantity = quantity;
        newItem->saleprice = 0;
        int index = hashfunc(name, hTable->size);
        node* newNode = malloc(sizeof(node));
        newNode->iPtr = newItem;
        newNode->next = hTable->lists[index];
        hTable->lists[index] = newNode;

        i = newItem;
        totalcash -= price;
    }
    //if the item is found, add the quantity to the existing quantity
    else
    {
        i->quantity += quantity;
        totalcash -= price;
    }
    //print
    fprintf(out, "%s %d %d\n", name, i->quantity, totalcash);
    return totalcash;
}

int sell(hashtable* hTable, char* name, int quantity, int totalcash, FILE* out)
{   
    //search for the item
    item* i = search(hTable, name, &complexity);
    //if the item is not found, print "Item not found"
    if(i == NULL)
    {   
        printf("Item not found\n");
        return 1;
    }
    else
    {
        //if the quantity of the item is less than the quantity to be sold, sell all the quantity of the item
        if(i->quantity < quantity) 
        {
            totalcash = totalcash + (i->quantity * i->saleprice);
            i->quantity = 0;
        }
        //if the quantity of the item is greater than or equal to the quantity to be sold, sell the quantity to be sold
        else 
        {
            totalcash = totalcash + (quantity * i->saleprice);
            i->quantity -= quantity;
        }
    }
    //print
    
    fprintf(out, "%s %d %d\n", name, i->quantity, totalcash);
    return totalcash;
}

//change the price of the item
int change_price(hashtable* hTable, char* name, int newprice)
{
    //search for the item
    item* i = search(hTable, name, &complexity);
    //if the item is not found, print "Item not found"
    if(i == NULL) 
    {
        printf("Item not found\n");
        return 1; // Item not found
    }
    //if the item is found, change the price of the item
    else 
    {
        i->saleprice = newprice;
    }
    return 0;
}

//main function
int main(int argc, char* argv[])
{   
    if(argc < 3)
    {
        printf("Usage: %s <input file> <output_file>\n", argv[0]);
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if(in == NULL || out == NULL)
    {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    //initiaize the hashtable
    hashtable* hTable = malloc(sizeof(hashtable));
    hTable->size = TABLESIZE;
    hTable->lists = malloc(sizeof(node*) * TABLESIZE);

    for(int i = 0; i < TABLESIZE; i++) {
        hTable->lists[i] = NULL;
    }
    
    //initialize the totalcash
    int totalcash = 100000;

    int n;
    char cmd[MAXLEN+1];
    fscanf(in, "%d", &n, cmd);
    
    for(int i=0; i<n; i++)
    {
        fscanf(in, "%s", cmd);
        if(strcmp(cmd, "buy") == 0)
        {
            char name[MAXLEN+1];
            int quantity, price;
            fscanf(in, "%s %d %d", name, &quantity, &price);
            totalcash = buy(hTable, name, quantity, price, totalcash, out);
        }
        else if(strcmp(cmd, "sell") == 0)
        {
            char name[MAXLEN+1];
            int quantity; 
            fscanf(in, "%s %d", name, &quantity);
            totalcash = sell(hTable, name, quantity, totalcash, out);
        }
        else if(strcmp(cmd, "change_price") == 0)
        {
            char name[MAXLEN+1];
            int newprice;
            fscanf(in, "%s %d", name, &newprice);
            change_price(hTable, name, newprice);
        }
    }

    //close the file
    fprintf(out, "%d\n", totalcash);
    fprintf(out, "%d\n", complexity);

    //free memory
    for (int i = 0; i < hTable->size; i++)
    {
        node* list = hTable->lists[i];
        while (list != NULL) {
            node* temp = list;
            list = list->next;
            free(temp->iPtr);
            free(temp);
        }
    }
    free(hTable->lists);
    free(hTable);

    //close the files
    fclose(in);
    fclose(out);
    
    return 0;
}