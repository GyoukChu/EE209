/* analyzer.c
  Assignment #5
  Created by Gyouk Chu on 02/12/2021
  Last Updated on 07/12/2021
*/

#include "dynarray.h"
#include "token.h"
#include "analyzer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum {MAX_LINE_SIZE = 1024};
enum {FALSE, TRUE};

/*--------------------------------------------------------------------*/

int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) if memory allocation fails, -1 if unmatched quote.
   In the latter case, oTokens may contain tokens 
   that were discovered before the error.
   Note that there is no NULL at the end of oTokens.
   The caller owns the tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */
{

  enum LexState {STATE_START, STATE_IN_WORD, STATE_IN_2QUOTE,
              STATE_IN_1QUOTE};

  enum LexState eState = STATE_START;

  int iLineIndex = 0;
  int iValueIndex = 0;
  char c;
  char acValue[MAX_LINE_SIZE];
  Token_T psToken;

  assert(pcLine != NULL);
  assert(oTokens != NULL);

  for (;;)
  {
    /* "Read" the next character from pcLine. */
    c = pcLine[iLineIndex++];

    switch (eState)
    {
      case STATE_START:
        if ((c == '\n') || (c == '\0'))
          return TRUE;
        else if (isspace(c))
          eState = STATE_START;
        else if (c == '\'')
          eState = STATE_IN_1QUOTE;
        else if (c == '\"')
          eState = STATE_IN_2QUOTE;
        /* ------------------------------------------- */
        /* Create Token for |, <, >, & cases */
        else if (c == '|')
        {
          acValue[iValueIndex++] = '|';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_PIPE, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '<')
        {
          acValue[iValueIndex++] = '<';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_REDIRECTION_IN, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '>')
        {
          acValue[iValueIndex++] = '>';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_REDIRECTION_OUT, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '&')
        {
          acValue[iValueIndex++] = '&';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_BACKGROUND, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        /* ------------------------------------------- */
        else
        {
          acValue[iValueIndex++] = c;
          eState = STATE_IN_WORD;
        }
        break;

      case STATE_IN_1QUOTE:
        if ((c == '\n') || (c == '\0'))
          return -1; // Unmated quote
        else if (c == '\'')
          eState = STATE_IN_WORD;
        else
        {
          acValue[iValueIndex++] = c;
          eState = STATE_IN_1QUOTE;
        }
        break;

      case STATE_IN_2QUOTE:
        if ((c == '\n') || (c == '\0'))
          return -1; // Unmated quote
        else if (c == '\"')
          eState = STATE_IN_WORD;
        else
        {
          acValue[iValueIndex++] = c;
          eState = STATE_IN_2QUOTE;
        }
        break;

      case STATE_IN_WORD:
        if ((c == '\n') || (c == '\0'))
        {
          /* Create a token. */
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          return TRUE;
        }
        else if (c == '\'')
          eState = STATE_IN_1QUOTE;
        else if (c == '\"')
          eState = STATE_IN_2QUOTE;
        else if (isspace(c))
        {
          /* Create a token. */
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        /* ------------------------------------------- */
        /* Create twp tokens for |, <, >, & cases */
        else if (c == '|')
        {
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          acValue[iValueIndex++] = '|';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_PIPE, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '<')
        {
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          acValue[iValueIndex++] = '<';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_REDIRECTION_IN, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '>')
        {
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          acValue[iValueIndex++] = '>';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_REDIRECTION_OUT, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        else if (c == '&')
        {
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_WORD, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          acValue[iValueIndex++] = '&';
          acValue[iValueIndex] = '\0';
          psToken = makeToken(TOKEN_REDIRECTION_OUT, acValue);
          if (psToken == NULL)
            return FALSE; //Cannot allocate memory
          if (! DynArray_add(oTokens, psToken))
            return FALSE; //Cannot allocate memory
          iValueIndex = 0;
          eState = STATE_START;
        }
        /* ------------------------------------------- */
        else
        {
          acValue[iValueIndex++] = c;
          eState = STATE_IN_WORD;
        }
        break; 
    }
  }
}

/*--------------------------------------------------------------------*/

int synLine(DynArray_T oTokens)

/* Syntatically analyze oTokens. Return 1 (TRUE) if is valid,
  0 (FALSE) if missing command name,
  -1/-2 if standard input/output redirection without file name,
  2/3 if multiple redirection of standard input/output respectively,
  or 4 if invalid use of background
  Except for 1 (TRUE), all the others are invalid cases.
  For such cases, caller should free remaining tokens and oTokens. */
{
  int n = DynArray_getLength(oTokens);
  if (n == 0) return TRUE;
  int count_in = 0, count_out = 0;
  int ispipe = FALSE, isout = FALSE;

  if (getTokenType(DynArray_get(oTokens,0)) != TOKEN_WORD)
    return 0;

  for(int i = 1; i < n; i++) {
    switch(getTokenType(DynArray_get(oTokens,i)))
    {
      case TOKEN_WORD:
        break; // do nothing

      case TOKEN_REDIRECTION_IN:
        count_in++;
        if (i == (n-1))
          return -1;
        else if(getTokenType(DynArray_get(oTokens,i+1)) != TOKEN_WORD)
          return -1;
        else if(count_in == 2)
          return 2;
        else if(ispipe)
          return 2;
        break;

      case TOKEN_REDIRECTION_OUT:
        count_out++; isout = TRUE;
        if (i == (n-1))
          return -2;
        else if((getTokenType(DynArray_get(oTokens,i+1)) != TOKEN_WORD))
          return -2;
        else if(count_out == 2)
          return 3;
        break;

      case TOKEN_PIPE:
        ispipe = TRUE;
        if (i == (n-1))
          return 0;
        else if(getTokenType(DynArray_get(oTokens,i+1)) != TOKEN_WORD)
          return 0;
        else if(isout)
          return 3;
        break;

      case TOKEN_BACKGROUND:
        if (i != (n-1))
          return 4;
        break;
    }
  }
  return 1;
}

/*--------------------------------------------------------------------*/
