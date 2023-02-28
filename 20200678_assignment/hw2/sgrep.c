/* sgrep.c
  Assignment #2, Part 2
  Edited by Gyouk Chu on 30/09/2021
  Last Updated on 05/10/2021
*/

#include <stdio.h>
#include <stdlib.h>
// #include <string.h> 
#include <unistd.h> /* for getopt */
#include "str.h"

/* for SearchPattern function */
#define MAX_STR_LEN 1023
#define FALSE 0
#define TRUE  1

/* IsMatch() : check whether given pattern is in string or not
    buf: a pointer of string
    pattern: a pointer of pattern

    returns: True if given pattern is in string. Otherwise, False
*/

int IsMatch (char *buf, const char* pattern) {
	/* Escape condition for recursion*/
	if (StrGetLength(pattern) == 0) return TRUE;
	
  const long int len = StrGetLength(pattern);
	char temp[len + 1]; //+1 for null character
	char * pcTemp = temp;
	
	/* Make a substring between *'s to use StrFindStr */
	while ((*pattern != '*') && (*pattern != '\0')) {
		*pcTemp = *pattern;
		pcTemp++; pattern++;
	}
	*pcTemp = '\0';
  /* if pattern string is not ended, then need +1 to skip '*' */
	if (*pattern != '\0') pattern++;
	
	/* Find substring */
	char * newbuf = StrFindStr((const char *)buf, (const char *)temp);
	
	if (! newbuf) return FALSE; //substring is not founded
  /* If substring is founded, then recursively return newbuf and revised
    pattern. Here, newbuf should point after such substring */
	else {
		for (int i = 0; i < StrGetLength((const char *)temp); i++) newbuf++;
		return IsMatch(newbuf, pattern);
	}
}

/* PrintUsage(): print out the usage of the Simple Grep Program    
    argv0: pointer of argument 0                
*/
void PrintUsage(const char* argv0) {
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}

/* SearchPattern(): Read each line from stdin, check and print out the line
  contains a given pattern. Argument validation is also performed.

    pattern: a pointer of pattern

    returns: If there is any problem, return FALSE; if not, return TRUE 
*/
int SearchPattern(const char *pattern) {
  char buf[MAX_STR_LEN + 2];

  /* Check if pattern is too long */
  if (StrGetLength(pattern) > MAX_STR_LEN ) {
    fprintf(stderr, "Error: pattern is too long\n");
    return FALSE;
  }
  
  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) {
	
    /* Check the length of an input line */
    if (StrGetLength((const char *)buf) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    
	  if (IsMatch(buf, pattern)) printf("%s",buf);
	  else continue;
  }
  return TRUE;
}

/* main function */
int main(const int argc, const char *argv[]) {
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}
