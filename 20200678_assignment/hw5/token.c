/* token.c
  Assignment #5
  Created by Gyouk Chu on 02/12/2021
  Last Updated on 06/12/2021

  Lexical/Syntatic analysis using a deterministic finite state
  automaton (DFA)
*/

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* A Token is either a number or a word, expressed as a string. */

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/

/* Free token pvItem.  pvExtra is unused. */

void freeToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/

/* Get eType of oToken. */

enum TokenType getTokenType(Token_T oToken)
{
  return (oToken->eType);
}

/*--------------------------------------------------------------------*/

/* Get pcValue of oToken. */

void *getTokenValue(Token_T oToken)
{
  return (void*)(oToken->pcValue);
}

/*--------------------------------------------------------------------*/

void printToken(void *pvItem, void *pvExtra)

/* Print token pvItem to stdout with its eType.  pvExtra is
   unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
    if (psToken->eType == TOKEN_WORD)
      fprintf(stdout, "TOKEN_WORD(\"%s\")\n", psToken->pcValue);
    if (psToken->eType == TOKEN_PIPE)
      fprintf(stdout, "TOKEN_PIPE(\"%s\")\n", psToken->pcValue);
    if (psToken->eType == TOKEN_REDIRECTION_IN)
      fprintf(stdout, "TOKEN_REDIRECTION_IN(\"%s\")\n", psToken->pcValue);
    if (psToken->eType == TOKEN_REDIRECTION_OUT)
      fprintf(stdout, "TOKEN_REDIRECTION_OUT(\"%s\")\n", psToken->pcValue);
    if (psToken->eType == TOKEN_BACKGROUND)
      fprintf(stdout, "TOKEN_BACKGROUND(\"%s\")\n", psToken->pcValue);
}

/*--------------------------------------------------------------------*/


Token_T makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
  struct Token *psToken;

  psToken = (struct Token*)malloc(sizeof(struct Token));
  if (psToken == NULL) // malloc fails
  return NULL;

  psToken->eType = eTokenType;
  
  psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
  if (psToken->pcValue == NULL) // malloc fails
  {
    free(psToken);
    return NULL;
  }

  strcpy(psToken->pcValue, pcValue);
  return psToken;
}