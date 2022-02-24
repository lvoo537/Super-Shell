// #include <stdio.h>
// #include <string.h>

// int main() {
//     char s1[30] = "University of C Programming";
//     char *p;

//     // find the index of the first 'v'
//     p = strchr(s1, 'e');
//     if (p == NULL) {
//         printf("Character not found\n");
//     } else {
//         printf("Character found at index %ld\n", p - s1);
//     }

//     // find the first token (up to the first space)
//     p = strchr(s1, ' ');
//     if (p != NULL) {
//         *p = '\0';
//     }
//     printf("%s\n", s1);

//     return 0;
// }


// typedef struct node {
//     int value;
//     struct node *next;
// } Node;

// /* Create and return a new Node with the supplied values. */
// Node *create_node(int num, Node *next) {
//     Node *new_node = malloc(sizeof(Node));
//     new_node->value = num;
//     new_node->next = next;
//     return new_node;
// }

// /* Insert a new node with the value num into this position of list front. */
// void insert(int num, Node *front, int position) {
//     Node *curr = front;
//     for (int i = 0; i < position - 1; i++) {
//         curr = curr->next;
//     }
//     printf("Currently at: %d\n", curr->value);

//     Node *new_node = create_node(num, curr->next);
//     curr->next = new_node;
// }

// int main() {
//     Node *front = NULL;
//     front = create_node(4, front);
//     front = create_node(3, front);
//     front = create_node(1, front);

//     insert(2, front, 1);

//     Node *curr = front;
//     while (curr != NULL) {
//         printf("%d\n", curr->value);
//         curr = curr->next;
//     }
//     return 0;


// char name[20]="lekan is the best \n";
// char* s = "lekan";
// printf("%s",name);
// printf("%s\n",s);
// s = "titi";
// printf("%s\n",s);
// s[0] = 5;
// printf("%s\n",s);
// return 0;
// }
#include <stdio.h>

int main() {
    FILE *scores_file;
    int error, total;
    char name[81];
  
    scores_file = fopen("top10.txt", "r");
    if (scores_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
  
    // Like scanf, fscanf returns the number of items successfully
    // read.
    // Here we compare the return value of fscanf to 2, since we
    // expect it to find two things on each call: a string
    // and an integer.
    while (fscanf(scores_file, "%s %d", name, &total) == 2) {
        printf("Name: %s. Score: %d.\n", name, total);
    }
  
    error = fclose(scores_file);
    if (error != 0) {
        fprintf(stderr, "fclose failed\n");
        return 1;
    }

    return 0;
}