#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

#include "builtins.h"
#include "io_helpers.h"

//checks if the string is comprised of only ".", if it is, it returns 0 else it returns -1;
int check_for_dots(char* s){
    int string_length = strlen(s);
    int i = 0;
    int indicator = 0;
    while (i< string_length){
        if(s[i]!='.'){
            indicator = -1;
            return indicator;
        }
        i+=1;
    }
    return indicator;
}
// returns the number of dots in s
int return_number_of_dots(char* s){
    int string_length = strlen(s);
    int i = 0;
    int number_of_dots = 0;
    while (i< string_length){
        if(s[i]=='.'){
            number_of_dots += 1;
        }
        i+=1;
    }
    return number_of_dots;
}


//checks if variable exists and returns the value of the ariable if it does but returns null if it doesn't
char* check_if_variable_exists(Node* front,char* tempv){
    Node* curr = front;
    while(curr != NULL){
        if(strcmp(curr->name, tempv) == 0){
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

// ls's helper
void ls_helper(int integer_value_of_d,char* value_of_f,char* value_of_rec){
    DIR* current_directory;
    current_directory = opendir(value_of_rec);
    if(current_directory == NULL){
        return;
    }
    else if(integer_value_of_d == 1){
        if(current_directory != NULL){
            struct dirent* struct_dirent_ptr;
            struct_dirent_ptr = readdir(current_directory);
            while(struct_dirent_ptr != NULL){
                if (value_of_f[0]!= -1){
                    char *p;
                    p = strstr(struct_dirent_ptr->d_name, value_of_f);
                    if(p!= NULL){
                        display_message(struct_dirent_ptr->d_name);
                        display_message("\n");
                    }
                }
                else{
                    display_message(struct_dirent_ptr->d_name);
                    display_message("\n");
                }
                
                struct_dirent_ptr = readdir(current_directory);
            }
            closedir(current_directory);
        }
    }
    else{
        struct dirent* struct_dirent_ptr;
        struct_dirent_ptr = readdir(current_directory);
        while(struct_dirent_ptr != NULL){
            if (value_of_f[0]!= -1){
                char *p;
                p = strstr(struct_dirent_ptr->d_name, value_of_f);
                if(p!= NULL){
                    display_message(struct_dirent_ptr->d_name);
                    display_message("\n");
                }
                if(strcmp(struct_dirent_ptr->d_name,".")!=0 &&strcmp(struct_dirent_ptr->d_name,"..")!=0){
                    char new_directory[256];
                    strcpy(new_directory,value_of_rec);
                    strcat(new_directory,"/");
                    strcat(new_directory,struct_dirent_ptr->d_name);
                    ls_helper(integer_value_of_d-1,value_of_f,new_directory);
                }
                
            }
            else{
                display_message(struct_dirent_ptr->d_name);
                display_message("\n");
                if(strcmp(struct_dirent_ptr->d_name,".")!=0 &&strcmp(struct_dirent_ptr->d_name,"..")!=0){
                    char new_directory[256];
                    strcpy(new_directory,value_of_rec);
                    strcat(new_directory,"/");
                    strcat(new_directory,struct_dirent_ptr->d_name);
                    ls_helper(integer_value_of_d-1,value_of_f,new_directory);
                }
            }
            
            struct_dirent_ptr = readdir(current_directory);
        }
        closedir(current_directory);
    }
}


// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}



// ===== Builtins =====

// this is the wc bultin
ssize_t bn_wc(char **tokens,Node* front){
    if(tokens[1] == NULL){
        FILE *fptr = NULL;
        struct pollfd fds;
        fds.fd = 0;     
        fds.events = POLLIN; 
        int ret = poll(&fds, 1, 10); // in your project, set this to 10, not 3000.
        if (ret == 0) {
            display_error("ERROR: No input source provided","");
            return -1;
        } else {
            int last_character_was_a_word = 0;
            int number_of_spaces = 0;
            int number_of_words = 0;
            int number_of_characters = 0;
            int number_of_newlines = 0;
            // char c;
            char return_string1[64];
            char return_string2[64];
            char return_string3[64];
            // char directory[256];
            // strcpy(directory,tokens[1]);
            // if(tokens[1][0] == '$'){
            //     char temp[256];
            //     strcpy(temp,&tokens[1][1]);
            //     char* temp_str_var2= check_if_variable_exists(front,temp);
            //         if(temp_str_var2 != NULL){
            //         strcpy(directory,temp_str_var2);
            //     }
            // }
            fptr = stdin;
            char c = fgetc(fptr);
            while (c != EOF) {
                if(c == ' ' ||c == '\t' || c == '\r'){
                // number_of_words+=1;
                number_of_spaces +=1;
                if(last_character_was_a_word == 1){
                    number_of_words+=1;
                }
                last_character_was_a_word = 0;
                }
                else if(c == '\n'){
                    number_of_newlines +=1;
                    if(last_character_was_a_word == 1){
                        number_of_words+=1;
                    }
                    last_character_was_a_word = 0;
                }
                else{
                    last_character_was_a_word = 1;
                }
                
                number_of_characters +=1;
                c = fgetc(fptr);
            }
            sprintf(return_string1,"word count %d\n",number_of_words);
            sprintf(return_string2,"character count %d\n",number_of_characters);
            sprintf(return_string3,"newline count %d\n",number_of_newlines);
            display_message(return_string1);
            display_message(return_string2);
            display_message(return_string3);
            return EXIT_SUCCESS;
        }
        
    }else{
        int last_character_was_a_word = 0;
        int number_of_spaces = 0;
        int number_of_words = 0;
        int number_of_characters = 0;
        int number_of_newlines = 0;
        char c;
        char return_string1[64];
        char return_string2[64];
        char return_string3[64];
        char directory[256];
        strcpy(directory,tokens[1]);
        if(tokens[1][0] == '$'){
            char temp[256];
            strcpy(temp,&tokens[1][1]);
            char* temp_str_var2= check_if_variable_exists(front,temp);
                if(temp_str_var2 != NULL){
                strcpy(directory,temp_str_var2);
            }
        }
        FILE* file;
        file = fopen(directory,"r");
        if(file == NULL){
            display_error("ERROR: Cannot open file","");
            return -1;
        }
        while(1){
            c = (char)fgetc(file) ; 
            if (c == EOF){
                break ;
            }
            else if(c == ' ' ||c == '\t' || c == '\r'){
                // number_of_words+=1;
                number_of_spaces +=1;
                if(last_character_was_a_word == 1){
                    number_of_words+=1;
                }
                last_character_was_a_word = 0;
            }
            else if(c == '\n'){
                number_of_newlines +=1;
                if(last_character_was_a_word == 1){
                    number_of_words+=1;
                }
                last_character_was_a_word = 0;
            }
            else{
                last_character_was_a_word = 1;
            }
            
            number_of_characters +=1;
        }
        // if(number_of_characters >0 &&number_of_spaces >0){
        //     number_of_words+=1;
        // }
        sprintf(return_string1,"word count %d\n",number_of_words);
        sprintf(return_string2,"character count %d\n",number_of_characters);
        sprintf(return_string3,"newline count %d\n",number_of_newlines);
        display_message(return_string1);
        display_message(return_string2);
        display_message(return_string3);
        return 0;
    }
    
}








// this is the cat builtin
ssize_t bn_cat(char **tokens,Node* front){
    // char str[100] ="hello my name is lekan\n how are you titi\n is lolade ok?\n";
    // char str2[100] ="what about chukwudi and amaka?";
    // write(STDIN_FILENO, str, strnlen(str, MAX_STR_LEN));
    // write(STDIN_FILENO, str2, strnlen(str2, MAX_STR_LEN));
    if(tokens[1] == NULL){
        char* line = NULL;
        size_t n;
        FILE *fptr = NULL;
        struct pollfd fds;
        fds.fd = 0;     
        fds.events = POLLIN; 
        int ret = poll(&fds, 1, 10); // in your project, set this to 10, not 3000.
        if (ret == 0) {
            display_error("ERROR: No input source provided","");
            return -1;
        } 
        else {
            fptr = stdin;
            while(getline(&line,&n,fptr) != -1){
                display_message(line);
            }
            free(line);
            return 0;
        }
    }
    else{
        char* line = NULL;
        size_t n;
        char directory[256];
        strcpy(directory,tokens[1]);
        if(tokens[1][0] == '$'){
            char temp[256];
            strcpy(temp,&tokens[1][1]);
            char* temp_str_var2= check_if_variable_exists(front,temp);
                if(temp_str_var2 != NULL){
                strcpy(directory,temp_str_var2);
            }
        }
        FILE* file;
        file = fopen(directory,"r");
        if(file == NULL){
            display_error("ERROR: Cannot open file","");
            return -1;
        }
        while(getline(&line,&n,file) != -1){
            display_message(line);
        }
        free(line);
        return 0;
    }
    
}



// this is the cd builtin
ssize_t bn_cd(char **tokens,Node* front){
    if(tokens[1] == NULL){
        chdir("/");
        return 0;
    }
    else if(tokens[1]!= NULL && tokens[2] == NULL){
        char directory[256];
        strcpy(directory,tokens[1]);
        if(tokens[1][0] == '$'){
            char temp[256];
            strcpy(temp,&tokens[1][1]);
            char* temp_str_var2= check_if_variable_exists(front,temp);
                if(temp_str_var2 != NULL){
                strcpy(directory,temp_str_var2);
            }
        }
        char directory1[256];
        strcpy(directory1,directory);
        int checking_for_dots = check_for_dots(directory1);
        if(checking_for_dots == 0){
            int number_of_dotss = return_number_of_dots(directory1);
            char new_directory[256]="";
            int i = 1;
            while(i<number_of_dotss){
                strcat(new_directory,"../");
                i+=1;
            }
            strcpy(directory1,new_directory);
        }
        int return_value_of_chdir = chdir(directory1);
        if(return_value_of_chdir == -1){
            display_error("ERROR: Invalid path","");
            return -1;
        }
        return 0;
    }
    else{
        display_error("ERROR: TOO MANY ARGUMENTS GIVEN TO CD","");
        return -1;
    }
}



//this is the ls builtin
// don't forget to return 0 on success and -1 on failure
// we need to implement the feature where you can say ls --f $substring1 --rec $path --d $depth
// i did what i said above but i didn't fix it for the cases with onlu --f as a flag, i did it for when it's --f and --rec and --d
ssize_t bn_ls(char **tokens,Node* front){
    int index_of_f = -1;
    int index_of_d = -1;
    int index_of_rec = -1;
    char value_of_f[64] = {-1};
    char value_of_d[64] = {-1};
    char value_of_rec[255] = {-1};
    int i =1;
    while(tokens[i] != NULL){
        if(strcmp(tokens[i],"--f")==0){
            index_of_f = i;
        }
        else if(strcmp(tokens[i],"--d")==0){
            index_of_d = i;
        }
        else if(strcmp(tokens[i],"--rec")==0){
            index_of_rec = i;
        }
        else{
            if(strcmp(tokens[i-1],"--rec") == 0){
                strcpy(value_of_rec,tokens[i]);
            }
            else if(strcmp(tokens[i-1],"--d") == 0){
                strcpy(value_of_d,tokens[i]);
            }
            else if(strcmp(tokens[i-1],"--f")){
                strcpy(value_of_f,tokens[i]);
            }
            else if(strcmp(tokens[i-1],"--rec") != 0 &&strcmp(tokens[i-1],"--d") != 0 && tokens[i+1]!= NULL&& strcmp(tokens[i+1],"--f")){
                strcpy(value_of_f,tokens[i]);
            }
        }
        i+=1;
    }
    if(index_of_rec != -1 && value_of_rec[0] == -1){
        strcpy(value_of_rec,"./");
    }
    if(value_of_rec[0]!= -1){
        DIR* directory;
        directory = opendir(value_of_rec);
        if(directory == NULL){
            display_error("ERROR: something went wrong somewhere","");
            return -1;
        }
        closedir(directory);

        if(value_of_rec[0]== '$'){
            char temp123[256];
            strcpy(temp123,&value_of_rec[1]);
            char* temp_str_var556= check_if_variable_exists(front,temp123);
            if(temp_str_var556 != NULL){
                strcpy(value_of_rec,temp_str_var556);
            }
        }
    }
    if(value_of_d[0]!= -1){
        if(value_of_d[0]== '$'){
            char temp1234[256];
            strcpy(temp1234,&value_of_d[1]);
            char* temp_str_var5564= check_if_variable_exists(front,temp1234);
            if(temp_str_var5564 != NULL){
                strcpy(value_of_d,temp_str_var5564);
            }
        }
    }
    if(value_of_f[0]!= -1){
        if(value_of_f[0]== '$'){
            char temp12345[256];
            strcpy(temp12345,&value_of_f[1]);
            char* temp_str_var55645= check_if_variable_exists(front,temp12345);
            if(temp_str_var55645 != NULL){
                strcpy(value_of_d,temp_str_var55645);
            }
        }
    }

    // ls with no arguments
    if(tokens[1] == NULL){
        DIR* current_directory;
        current_directory = opendir("./");
        struct dirent* struct_dirent_ptr;
        struct_dirent_ptr = readdir(current_directory);
        while(struct_dirent_ptr != NULL){
            display_message(struct_dirent_ptr->d_name);
            display_message("\n");
            struct_dirent_ptr = readdir(current_directory);
        }
        closedir(current_directory);
        return 0;
    }
    // ls with one argument that is a path
    else if(strcmp(tokens[1],"--f")!=0 && strcmp(tokens[1],"--rec")!=0 && strcmp(tokens[1],"--d")!=0 && tokens[2]==NULL){
        char directory[256];
        strcpy(directory,tokens[1]);
        if(tokens[1][0] == '$'){
            char temp[256];
            strcpy(temp,&tokens[1][1]);
            char* temp_str_var2= check_if_variable_exists(front,temp);
             if(temp_str_var2 != NULL){
                strcpy(directory,temp_str_var2);
            }
        }
        DIR* current_directory;
        current_directory = opendir(directory);
        if(current_directory != NULL){
            struct dirent* struct_dirent_ptr;
            struct_dirent_ptr = readdir(current_directory);
            while(struct_dirent_ptr != NULL){
                display_message(struct_dirent_ptr->d_name);
                display_message("\n");
                struct_dirent_ptr = readdir(current_directory);
            }
            closedir(current_directory);
            return 0;
        }
        else{
            display_error("ERROR: Invalid path","");
            return -1;
        }
    }
    // ls with just --f and no paths
    else if(strcmp(tokens[1],"--f")==0 && strcmp(tokens[2],"--rec")!=0 && strcmp(tokens[2],"--d")!=0 && strcmp(tokens[2],"--f")!=0 && tokens[3] == NULL){
        DIR* current_directory;
        current_directory = opendir("./");
        if(current_directory != NULL){
            struct dirent* struct_dirent_ptr;
            struct_dirent_ptr = readdir(current_directory);
            while(struct_dirent_ptr != NULL){
                char *p;
                p = strstr(struct_dirent_ptr->d_name, tokens[2]);
                if(p!= NULL){
                    display_message(struct_dirent_ptr->d_name);
                    display_message("\n");
                }
                
                struct_dirent_ptr = readdir(current_directory);
            }
            closedir(current_directory);
            return 0;
        }
        else{
            display_error("ERROR: Invalid path","");
            return -1;
        }
    }
    // ls with a path before --f and no other arguments
    else if(strcmp(tokens[1],"--f")!=0 && strcmp(tokens[1],"--rec")!=0 && strcmp(tokens[1],"--d")!=0 && strcmp(tokens[2],"--f") == 0 && tokens[3]!= NULL &&strcmp(tokens[3],"--f")!=0 && strcmp(tokens[3],"--rec")!=0 && strcmp(tokens[3],"--d")!=0){
        DIR* current_directory;
        current_directory = opendir(tokens[1]);
        if(current_directory != NULL){
            struct dirent* struct_dirent_ptr;
            struct_dirent_ptr = readdir(current_directory);
            while(struct_dirent_ptr != NULL){
                char *p;
                p = strstr(struct_dirent_ptr->d_name, tokens[3]);
                if(p!= NULL){
                    display_message(struct_dirent_ptr->d_name);
                    display_message("\n");
                }
                
                struct_dirent_ptr = readdir(current_directory);
            }
            closedir(current_directory);
            return 0;
        }
        else{
            display_error("ERROR: Invalid path","");
            return -1;
        }
    }
    // ls with a path after --f and no other arguments
    else if(strcmp(tokens[1],"--f")==0 && strcmp(tokens[2],"--rec")!=0 && strcmp(tokens[2],"--d")!=0 && strcmp(tokens[2],"--f") != 0 && tokens[3]!= NULL &&strcmp(tokens[3],"--f")!=0 && strcmp(tokens[3],"--rec")!=0 && strcmp(tokens[3],"--d")!=0){
        DIR* current_directory;
        current_directory = opendir(tokens[3]);
        if(current_directory != NULL){
            struct dirent* struct_dirent_ptr;
            struct_dirent_ptr = readdir(current_directory);
            while(struct_dirent_ptr != NULL){
                char *p;
                p = strstr(struct_dirent_ptr->d_name, tokens[2]);
                if(p!= NULL){
                    display_message(struct_dirent_ptr->d_name);
                    display_message("\n");
                }
                
                struct_dirent_ptr = readdir(current_directory);
            }
            closedir(current_directory);
            return 0;
        }
        else{
            display_error("ERROR: Invalid path","");
            return -1;
        }
    }
    else if( ((index_of_rec == -1 && index_of_d ==-1)||((index_of_rec != -1 && index_of_d !=-1)&&(value_of_d[0] != -1 && value_of_rec[0] != -1))) && ((index_of_f ==-1)||(index_of_f !=-1 && value_of_f[0]!=-1))){
        int integer_value_of_d = strtol(value_of_d,NULL,10);
        if ((strcmp(value_of_d,"0") && integer_value_of_d == 0)|| integer_value_of_d>0){
            ls_helper(integer_value_of_d,value_of_f,value_of_rec);
            return 0;
        }
       else{
           display_error("ERROR: there is a problem with the depth value","");
           return -1;
       } 
    }
    else{
        display_error("ERROR: something went wrong somewhere","");
        return -1;
    }
    // might need to change this later  && strcmp(tokens[2],"--f")!=0 && strcmp(tokens[2],"--rec")!=0 && strcmp(tokens[2],"--d")!=0
}








/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens,Node* front) {
    ssize_t index = 1;
    if (tokens[index] != NULL) {
        if(tokens[index][0] == '$'){
            char temp_str[strlen(tokens[index])];
            int i= 0;
            while(i < (sizeof(temp_str) -1)){
                temp_str[i] = tokens[index][i+1];
                i+=1;
            }
            temp_str[i] = '\0';
            char* value = check_if_variable_exists(front,temp_str);
            if(value == NULL){
                display_message(tokens[index]);
                index += 1;
            }
            else{
                display_message(value);
                index += 1;
            }
        }
    
        else{
        // TODO:
        // Implement the echo command
        display_message(tokens[1]);
        index += 1;
        }
    }
    
    while (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        if (tokens[index] != NULL) {
            if(tokens[index][0] == '$'){
                char temp_str[strlen(tokens[index])];
                int i= 0;
                while(i < (sizeof(temp_str) -1)){
                    temp_str[i] = tokens[index][i+1];
                    i+=1;
                }
                temp_str[i] = '\0';
                char* value = check_if_variable_exists(front,temp_str);
                if(value == NULL){
                    display_message(" ");
                    display_message(tokens[index]);
                    index += 1;
                }
                else{
                    display_message(" ");
                    display_message(value);
                    index += 1;
                }
            }
    
            else{
            display_message(" ");
            display_message(tokens[index]);
            index += 1;
            }
        }
        
    }
    display_message("\n");

    return 0;
}
