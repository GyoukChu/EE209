/* token.h
  Assignment #5
  Created by Gyouk Chu on 02/12/2021
  Last Updated on 06/12/2021

  Lexical/Syntatic analysis using a deterministic finite state
  automaton (DFA)
*/

#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED

/*--------------------------------------------------------------------*/

enum TokenType {TOKEN_WORD, TOKEN_REDIRECTION_IN, 
              TOKEN_REDIRECTION_OUT, TOKEN_PIPE, TOKEN_BACKGROUND};

/*--------------------------------------------------------------------*/

/* A Token is a sequence of non-white-space characters */
typedef struct Token * Token_T;

/* Free a Token */
void freeToken(void *pvItem, void *pvExtra);

/* Get a Type of Token */
enum TokenType getTokenType(Token_T oToken);

/* Get a Value of Token */
void *getTokenValue(Token_T oToken);

/* Only for testing functionality */
void printToken(void *pvItem, void *pvExtra);

/* Make new Token */
Token_T makeToken(enum TokenType eTokenType,
   char *pcValue);

#endif