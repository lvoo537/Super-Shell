#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"




Node *create_node(char* namee,char* valuee, Node *next) {
    Node *new_node = malloc(sizeof(Node));
    new_node->name = malloc(strlen(namee) + 1);
    strcpy(new_node->name,namee);
    new_node->value = malloc(strlen(valuee) + 1);
    strcpy(new_node->value,valuee);
    new_node->next = next;
    return new_node;
}



int check_if_input_is_a_variable(char* input){
    char* equals_to_index_pointer = strchr(input,'=');
    if (equals_to_index_pointer == NULL){
        return 0;
    }
    // else{
    //     int equals_to_index = equals_to_index_pointer - input;
    // }
    return 1;
}

void print_elements(Node* front){
    Node *curr = front;
    while (curr != NULL) {
        printf("name: %s value: %s\n", curr->name,curr->value);
        curr = curr->next;
    }
}

void free_all_elements(Node* front){
    Node *curr = front;
    Node* next = front;
    while (next != NULL) {
        next = curr->next;
        //printf("name: %s value: %s\n", curr->name,curr->value);
        free(curr->name);
        free(curr->value);
        // free(curr->next);
        free(curr);
        curr = next;
    }
}

Node* check_if_variable_existss(Node* front,char* temp){
    Node* curr = front;
    while(curr != NULL){
        if(strcmp(curr->name, temp) == 0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    Node* front = NULL;
    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:
        // Display the prompt via the display_message function.

        display_message(prompt);
        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);
        
        // Clean exit
        if (ret != -1 && (token_count == 0 || (strcmp("exit", token_arr[0]) == 0))) {
            break;
        }

        // Command execution
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr,front);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }            
            }
            else if(check_if_input_is_a_variable(token_arr[0]) == 1){
                char* equals_to_index_pointer = strchr(token_arr[0],'=');
                int equals_to_index = equals_to_index_pointer - token_arr[0];
                int i = 0;
                // char temp_name[equals_to_index + 1];
                char temp_name[62];
                // char temp_value[strlen(token_arr[0]) - equals_to_index];
                char temp_value[62];
                while(i< equals_to_index){
                    temp_name[i] = token_arr[0][i];
                    i+=1;
                }
                // temp_name[sizeof(temp_name) - 1] = '\0';
                temp_name[equals_to_index] = '\0';
                i+=1;
                int j = 0;
                while(i < strlen(token_arr[0])){
                    temp_value[j] = token_arr[0][i];
                    i+=1;
                    j+=1;
                }
                // temp_value[sizeof(temp_value) - 1] = '\0';
                temp_value[j] = '\0';
                Node* check = check_if_variable_existss(front,temp_name);
                if(check == NULL){
                    front = create_node(temp_name,temp_value,front);
                }else{
                    strcpy(check->value,temp_value);
                }
                
            }
            else {
                display_error("ERROR: Unrecognized command: ", token_arr[0]);
            }
        }
    }
    free_all_elements(front);

    return 0;
}
