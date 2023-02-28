/* valid.c
  Assignment #5
  Created by Gyouk Chu on 02/12/2021
  Last Updated on 06/12/2021
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include "token.h"
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

enum {FALSE, TRUE};

/*--------------------------------------------------------------------*/

void freeoTokens(DynArray_T oTokens)
{
    /* free the oTokens */
    assert(oTokens);
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
    return;
}

/*--------------------------------------------------------------------*/

void freenew_argv(char **new_argv, int n)
{
    /* free the new_argv */
    assert(new_argv);
    for (int i = 0; i < n; i++) {
        free(new_argv[i]);
    }
    free(new_argv);
    return;
}

/*--------------------------------------------------------------------*/

int isvalid_lex(const char * arg, int isuccessful,  DynArray_T oTokens)
{
    /* Check whether lexical analysis is valid or not. After lexical
    analysis, oTokens is generated and isscuessful shows the return 
    value. Return 1 (TRUE) if successful, 0 (FALSE) otherwise. */

    if (isuccessful == 0)  // memory short
    {
        fprintf(stderr, "%s: %s\n", arg, strerror(errno));
        return FALSE;
    }
    else if (isuccessful == -1)  // Unmatched quote
      {
        fprintf(stderr, "%s: Unmatched quote\n", arg);
        return FALSE;
    }
    else if (isuccessful == 1) return TRUE;
    else    assert(0);
}

/*--------------------------------------------------------------------*/


int isvalid_syn(const char * arg, int isuccessful,  DynArray_T oTokens)
{
    /* Check whether syntatic analysis is valid or not. After syntatic
    analysis, isscuessful shows the return value. 
    Return 1 (TRUE) if successful, 0 (FALSE) otherwise. */
    if (isuccessful != TRUE)
    {
        switch(isuccessful)
        {
          case -2:
            fprintf(stderr, "%s: Standard output redirection without file name\n", arg);
          break;
          case -1:
            fprintf(stderr, "%s: Standard input redirection without file name\n", arg);
          break;
          case 0:
            fprintf(stderr, "%s: Missing command name\n", arg);
          break;
          case 2:
            fprintf(stderr, "%s: Multiple redirection of standard input\n", arg);
          break;
          case 3:
            fprintf(stderr, "%s: Multiple redirection of standard out\n", arg);
          break;
          case 4:
            fprintf(stderr, "%s: Invalid use of background\n", arg);
          break;
        }
        return FALSE;
    }
    else return TRUE;
}

/*--------------------------------------------------------------------*/

int isvalid_builtin(const char * arg, int isuccessful, DynArray_T oTokens)
{
    /* Check whether built-in command is valid or not.  
    Return 1 (TRUE) if successful, 0 (FALSE) otherwise. */
    char * new_argv_0 = (char *)getTokenValue(DynArray_get(oTokens,0));
    int n = DynArray_getLength(oTokens);
    if (strcmp(new_argv_0,"setenv")==0) 
    {
        isuccessful = TRUE;
        if ((n < 2) || (n > 3)) 
          isuccessful = FALSE;
        for(int i = 1; i < n; i++) {
            if( (strcmp(getTokenValue(DynArray_get(oTokens,i)),"|")==0) || 
                (strcmp(getTokenValue(DynArray_get(oTokens,i)),"<")==0) ||
                (strcmp(getTokenValue(DynArray_get(oTokens,i)),">")==0) || 
                (strcmp(getTokenValue(DynArray_get(oTokens,i)),"&")==0) )
            {
                isuccessful = FALSE; break;
            }
        }
        /* Invalid cases */
        if (isuccessful == FALSE) 
        {
            fprintf(stderr, "%s: setenv takes one or two parameters\n", arg);
            return FALSE;
        }
        else
        {
            if (n == 3)
                isuccessful = setenv(getTokenValue(DynArray_get(oTokens,1)), 
                            getTokenValue(DynArray_get(oTokens,2)), 1);
            else // n==2
                isuccessful = setenv(getTokenValue(DynArray_get(oTokens,1)), 
                            "", 1);

            if (isuccessful != 0) //setenv fail
            {
                fprintf(stderr, "%s: %s\n", arg, strerror(errno));
                return FALSE;
            }
        }
    }
    else if (strcmp(new_argv_0,"unsetenv")==0)
    {
        if (n!=2) 
        {
            fprintf(stderr, "%s: unsetenv takes one parameter\n", arg);
            return FALSE;
        }
        else
        {
            isuccessful = unsetenv(getTokenValue(DynArray_get(oTokens,1)));
            if (isuccessful != 0) //unsetenv fail
            {
                fprintf(stderr, "%s: %s\n", arg, strerror(errno));
                return FALSE;
            }
        }
    }
    else if (strcmp(new_argv_0,"cd")==0)
    {
        if (! ((n==1) || (n==2))) 
        {
            fprintf(stderr, "%s: cd takes one parameter\n", arg);
            return FALSE;
        }
        else if (n==2)
        {
            isuccessful = chdir(getTokenValue(DynArray_get(oTokens,1)));
            if (isuccessful != 0) //chdir fail
            {
                fprintf(stderr, "%s: %s\n", arg, strerror(errno));
                return FALSE;
            }
        }
        else
        {
            void *psuccessful = getenv("HOME");
            assert(psuccessful); // getenv fail
            isuccessful = chdir(psuccessful);
            assert(! isuccessful); // chdir fail
            return TRUE;
        }
    }
    else if (strcmp(new_argv_0,"exit")==0)
    {  
        if (n!=1) 
        {
            fprintf(stderr, "%s: exit does not take any parameters\n", arg);
            return FALSE;
        }
        else
        {
            fprintf(stdout ,"\n");
            exit(EXIT_SUCCESS); //exit(0);
        }
    }
    return TRUE;
}

/*--------------------------------------------------------------------*/

int is_RedirectIn(DynArray_T oTokens)
{
    /* Check whether there exists TOKEN_REDIRECTION_IN .
    Return the index of such token if exists, 0 (FALSE) otherwise. */
    assert(oTokens);
    int n = DynArray_getLength(oTokens);
    for (int i = 0; i < n; i++) {
        if (strcmp((char *)getTokenValue(DynArray_get(oTokens,i)),
                    "<") == 0) return i;
    }
    return FALSE;
}

/*--------------------------------------------------------------------*/

int is_RedirectOut(DynArray_T oTokens)
{
    /* Check whether there exists TOKEN_REDIRECTION_OUT .
    Return the index of such token if exists, 0 (FALSE) otherwise. */
    assert(oTokens);
    int n = DynArray_getLength(oTokens);
    for (int i = 0; i < n; i++) {
        if (strcmp((char *)getTokenValue(DynArray_get(oTokens,i)),
                    ">") == 0) return i;
    }
    return FALSE;
}
