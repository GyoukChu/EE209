/* valid.h
  Assignment #5
  Created by Gyouk Chu on 02/12/2021
  Last Updated on 06/12/2021
*/

#include "token.h"
#include "dynarray.h"
#ifndef VALID_INCLUDED
#define VALID_INCLUDED

/* Free oTokens */
void freeoTokens(DynArray_T oTokens);

/* Free new_argv */
void freenew_argv(char **new_argv, int n);

/* Check whether lexical analysis is valid or not. */
int isvalid_lex(const char * arg, int issucessful,  DynArray_T oTokens);

/* Check whether syntatic analysis is valid or not. */
int isvalid_syn(const char * arg, int issucessful,  DynArray_T oTokens);

/* Check whether built-in command is valid or not. */
int isvalid_builtin(const char * arg, int issucessful, DynArray_T oTokens);

/* Check whether there exists '<' */
int is_RedirectIn(DynArray_T oTokens);

/* Check whether there exists '>' */
int is_RedirectOut(DynArray_T oTokens);

#endif