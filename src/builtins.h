#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>
#include <sys/types.h>
typedef struct node{
    char* name;
    char* value;
    struct node* next;
} Node;

/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **,Node*);
ssize_t bn_echo(char **tokens,Node* front);
ssize_t bn_ls(char **tokens,Node* front);
ssize_t bn_cd(char **tokens,Node* front);
ssize_t bn_cat(char **tokens,Node* front);
ssize_t bn_wc(char **tokens,Node* front);


/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo","ls","cd","cat","wc"};
static const bn_ptr BUILTINS_FN[] = {bn_echo,bn_ls,bn_cd,bn_cat,bn_wc, NULL};    // Extra null element for 'non-builtin'
static const size_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);

#endif
