#include<stdio.h>
#include<stdlib.h>

struct node{
    struct node *next, *prev;
    char text[1000];
}node;

struct node *head = NULL;
struct node *end = NULL;
int length = 0;

void push_string(){
    char str;
    struct node* new_node = (struct node*)malloc(sizeof(node));

    new_node -> next = NULL;
    new_node -> prev = NULL;

    printf("\nInsert a String: ");
    scanf("%c", &str);
    scanf("%s", new_node -> text);

    if(!end){
        head = new_node;
        end = new_node;
    }
    else{
        end -> next = new_node;
        struct node *temp = end;
        end = new_node;
        end -> prev = temp;
    }
    length++;
}


void print_list(){


    if(length == 0 ){
        printf("\nList is empty.\n");
        return;
    }
    else{
    printf("\n\nItems: \n");
    struct node *current = head;
    int counter = 1; 
    while(counter <= length){
        printf("%d: %s\n", counter, current -> text);
        current = current -> next;
        counter++;
    }
    printf("\n");
    counter = 1;
    }
}


void delete_item(){

    int delete = 0;

    struct node *current = head;

    if(length == 0 ){
        printf("\nList is empty.\n");
        return;
    }

    if(length == 1){
        printf("\nDeleted the only item.\n");
        free(head);
        head = NULL;
        end = NULL;
        length = 0;
        return;
    }

    while( delete < 1 || delete > length ){
        printf("Please enter an integer between 1 and %d: ", length);
        while(scanf("%d", &delete) != 1)
        {
            printf("Please enter an integer: ");
            while (getchar() != '\n');            
        }
    }

    for(int i = 1; i < delete; i++){
        if(current -> next ){
            current = current -> next;
        }
        else{
            return;
        }
    }

    if(current -> prev){
        if(current -> next){
            current -> prev -> next = current -> next;
            current -> next -> prev = current -> prev;
        }
        else{
            current -> prev -> next == NULL;
            end = current -> prev;
        }
    }
    else{
        head = current -> next;
        current -> next -> prev = NULL;
    }

    printf("\nDeleted item %d", delete);
    free(current);
    length--;
}


int main(){

    int selection = 0;

    while (selection != 4)
    {
        printf("\nSelect: \n1 Push String\n2 Print List\n3 Delete Item\n4 End Program\nMake a Selection: ");

        while(scanf("%d", &selection) != 1){
            printf("Please enter an integer: ");
            while (getchar() != '\n');            
        }

        if( selection < 1 || selection > 4 ){
            printf("\nInvalid Selection\n\n");
        }
        else if( selection == 1){
            push_string();
        }
        else if( selection == 2){
            print_list();
        }
        else if( selection == 3){
            delete_item();
        }
    }

    while (head)
    {
        free(head);
        head = head -> next;
    }
    printf("\n------- Ending Program-------\n");

    return 0;
}