#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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

int check_if_pipe_exists(char **tokens){
    int i =0;
    while (tokens[i] != NULL){
        char* pipe_pointer = strchr(tokens[i],'|');
        if (pipe_pointer != NULL)
        {
            return 1;
        }
        i +=1;
    }
    return 0;
}

void split_commands_by_pipes(char** array_of_tokens_separated_by_pipes,char *in_ptr){
    char *curr_ptr = strtok (in_ptr, "|");
    size_t token_count = 0;

    while (curr_ptr != NULL) {  // TODO: Fix this
        // TODO: Fix this
        array_of_tokens_separated_by_pipes[token_count] = curr_ptr;
        curr_ptr = strtok(NULL, "|");

        token_count += 1;
    }
    array_of_tokens_separated_by_pipes[token_count] = NULL;
}

int get_number_of_pipes(char **token_arr){
    int i = 0;
    int number_of_pipes = 0;
    while (token_arr[i] != NULL){
        char* str_pointer = strchr(token_arr[i],'|');
        if(str_pointer != NULL){
            number_of_pipes +=1;
        }
        i+=1;
    }
    return number_of_pipes;
}
// return 1 if there is a pipe syntax error
int is_there_a_pipe_syntax_error(char **token_arr,int number_of_tokens){
    int i =0;
    char* str_pointer = strchr(token_arr[i],'|');
    if(str_pointer != NULL && strlen(token_arr[i])==1){
        return 1;
    }
    while (i< number_of_tokens-1){
        char* str_pointer3 = strchr(token_arr[i],'|');
        char* str_pointer4 = strchr(token_arr[i+1],'|');
        char* str_pointer5 = strstr(token_arr[i],"||");
        if(str_pointer5 != NULL ){
            return 1;
        }
        if(str_pointer3 != NULL && strlen(token_arr[i])==1 && str_pointer4 != NULL && strlen(token_arr[i+1])==1){
            return 1;
        }
        i +=1;
    }
    char* str_pointer2 = strchr(token_arr[i],'|');
    if(str_pointer2 != NULL && strlen(token_arr[i])==1){
        return 1;
    }
    return 0;
}


int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    char input_buf2[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    Node* front = NULL;
    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:
        // Display the prompt via the display_message function.

        display_message(prompt);
        int ret = get_input(input_buf);
        strcpy(input_buf2,input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);
        // char* str ="hello my name is lekan\nhow are you titi\nis lolade ok?\n";
        // char* str2 ="what about chukwudi and amaka?\n";
        // int sw =write(0, str, strnlen(str, MAX_STR_LEN));
        // int sw2 =write(STDIN_FILENO, str2, strnlen(str2, MAX_STR_LEN));
        // sw2+=1;
        // sw+=1;
        
        // Clean exit
        if (ret != -1 && (token_count == 0 || (strcmp("exit", token_arr[0]) == 0))) {
            break;
        }

        // Command execution
        if (token_count >= 1) {
            if(is_there_a_pipe_syntax_error(token_arr,token_count) == 1){
                display_error("ERROR: pipe syntax error", "");
            }
            else{
                int number_of_pipes = get_number_of_pipes(token_arr);
                if(number_of_pipes >= 1){
                    char* array_of_command_arrays[number_of_pipes+1][256];
                    char* array_of_tokens_separated_by_pipes[256] = {NULL};
                    split_commands_by_pipes(array_of_tokens_separated_by_pipes,input_buf2);
                    int p =0;
                    while (array_of_tokens_separated_by_pipes[p]!= NULL){
                        tokenize_input(array_of_tokens_separated_by_pipes[p],array_of_command_arrays[p]);
                        p +=1;
                    }
                    int pipe_array[number_of_pipes][2];
                    int i =0;
                    while(i<number_of_pipes +1){
                        if(i == 0){
                            if(pipe(pipe_array[0]) == -1){
                                display_error("ERROR: There was an opening pipe error", "");
                            }

                            int r = fork();
                            if(r<0){
                                display_error("ERROR: forking error", "");
                                break;
                            }
                            else if(r > 0){
                                // if (close(pipe_array[i][0])/*TODO*/ == -1) {
                                //     display_error("ERROR: There was a closing pipe error", "");
                                // }
                                if (close(pipe_array[i][1])/*TODO*/ == -1) {
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                wait(NULL);
                            }
                            else if(r == 0){
                                if(dup2(pipe_array[0][1],fileno(stdout)) == -1){
                                    display_error("ERROR: There was a changing pipe error", "");
                                    break;
                                }
                                
                                // execute the command
                                bn_ptr builtin_fn1 = check_builtin(array_of_command_arrays[0][0]);
                                if (builtin_fn1 != NULL){ //|| (check_if_pipe_exists(token_arr) == 1 && check_if_input_is_a_variable(token_arr[0]) == 1)) {
                                    ssize_t err = builtin_fn1(array_of_command_arrays[0],front);
                                    if (err == - 1) {
                                        display_error("ERROR: Builtin failed: ", array_of_command_arrays[0][0]);
                                        // break;
                                    }
                                }
                                else if(check_if_input_is_a_variable(array_of_command_arrays[0][0]) == 1){
                                    // do nothing
                                    int lekan =0;
                                    lekan+=1;
                                }
                                else {
                                    display_error("ERROR: Unrecognized command: ", array_of_command_arrays[0][0]);
                                }
                                if (close(pipe_array[0][1]) == -1) { 
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                if ((close(pipe_array[0][0])) == -1) {
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                exit(0);            
                            }
                            
                        }
                        else if(i== number_of_pipes){
                            // if(pipe(pipe_array[i]) == -1){
                            //     display_error("ERROR: There was an opening pipe error", "");
                            // }

                            int r = fork();
                            if(r<0){
                                display_error("ERROR: forking error", "");
                                break;
                            }
                            else if(r > 0){
                                // if (close(pipe_array[i][0])/*TODO*/ == -1) {
                                //     display_error("ERROR: There was a closing pipe error", "");
                                // }
                                // if (close(pipe_array[i][1])/*TODO*/ == -1) {
                                //     display_error("ERROR: There was a closing pipe error", "");
                                //     break;
                                // }
                                // do nothing
                                int lekan2 =0;
                                lekan2+=1;
                                wait(NULL);
                            }
                            else if(r == 0){
                                if(dup2(pipe_array[i-1][0],fileno(stdin)) == -1){
                                    display_error("ERROR: There was a changing pipe error", "");
                                    break;
                                }
                                // if(dup2(pipe_array[i][1],fileno(stdout)) == -1){
                                //     display_error("ERROR: There was a changing pipe error", "");
                                //     break;
                                // }
                                // if ((close(pipe_array[i][1]) == -1) { 
                                //     display_error("ERROR: There was a closing pipe error", "");
                                //     break;
                                // }
                                // if ((close(pipe_array[i][0])) == -1) {
                                //     display_error("ERROR: There was a closing pipe error", "");
                                //     break;
                                // }
                                int j = 0;
                                while(j<i){
                                    if ((close(pipe_array[j][0])) == -1) {
                                        display_error("ERROR: There was a closing pipe error", "");
                                        break;
                                    }
                                    j+=1;
                                }
                                // execute the command
                                bn_ptr builtin_fn1 = check_builtin(array_of_command_arrays[i][0]);
                                if (builtin_fn1 != NULL){ //|| (check_if_pipe_exists(token_arr) == 1 && check_if_input_is_a_variable(token_arr[0]) == 1)) {
                                    ssize_t err = builtin_fn1(array_of_command_arrays[i],front);
                                    if (err == - 1) {
                                        display_error("ERROR: Builtin failed: ", array_of_command_arrays[i][0]);
                                        // break;
                                    }
                                }
                                else if(check_if_input_is_a_variable(array_of_command_arrays[i][0]) == 1){
                                    // do nothing
                                    int lekan =0;
                                    lekan+=1;
                                }
                                else {
                                    display_error("ERROR: Unrecognized command: ", array_of_command_arrays[i][0]);
                                }
                                exit(0);            
                            }
                            
                        }
                        else{
                            if(pipe(pipe_array[i]) == -1){
                                display_error("ERROR: There was an opening pipe error", "");
                            }

                            int r = fork();
                            if(r<0){
                                display_error("ERROR: forking error", "");
                                break;
                            }
                            else if(r > 0){
                                // if (close(pipe_array[i][0])/*TODO*/ == -1) {
                                //     display_error("ERROR: There was a closing pipe error", "");
                                // }
                                if (close(pipe_array[i][1])/*TODO*/ == -1) {
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                wait(NULL);
                            }
                            else if(r == 0){
                                if(dup2(pipe_array[i-1][0],fileno(stdin)) == -1){
                                    display_error("ERROR: There was a changing pipe error", "");
                                    break;
                                }
                                if(dup2(pipe_array[i][1],fileno(stdout)) == -1){
                                    display_error("ERROR: There was a changing pipe error", "");
                                    break;
                                }
                                
                                // execute the command
                                bn_ptr builtin_fn1 = check_builtin(array_of_command_arrays[i][0]);
                                if (builtin_fn1 != NULL){ //|| (check_if_pipe_exists(token_arr) == 1 && check_if_input_is_a_variable(token_arr[0]) == 1)) {
                                    ssize_t err = builtin_fn1(array_of_command_arrays[i],front);
                                    if (err == - 1) {
                                        display_error("ERROR: Builtin failed: ", array_of_command_arrays[i][0]);
                                        break;
                                    }
                                }
                                else if(check_if_input_is_a_variable(array_of_command_arrays[i][0]) == 1){
                                    // do nothing
                                    int lekan =0;
                                    lekan+=1;
                                }
                                else {
                                    display_error("ERROR: Unrecognized command: ", array_of_command_arrays[i][0]);
                                }
                                if (close(pipe_array[i][1]) == -1) { 
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                if ((close(pipe_array[i][0])) == -1) {
                                    display_error("ERROR: There was a closing pipe error", "");
                                    break;
                                }
                                int j = 0;
                                while(j<i){
                                    if ((close(pipe_array[j][0])) == -1) {
                                        display_error("ERROR: There was a closing pipe error", "");
                                        break;
                                    }
                                    j+=1;
                                }
                                exit(0);            
                            }
                            
                        }
                        i+=1;
                    }
                    
                }

                else{
                    bn_ptr builtin_fn = check_builtin(token_arr[0]);
                
                    if (builtin_fn != NULL || (check_if_pipe_exists(token_arr) == 1 && check_if_input_is_a_variable(token_arr[0]) == 1)) {
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
        }
            
        
    }
    free_all_elements(front);

    return 0;
}
