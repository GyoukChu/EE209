/* str.c
  Assignment #2, Part 1
  Edited by Gyouk Chu on 29/09/2021
  Last Updated on 05/10/2021
*/

#include <assert.h> /* to use assert() */
#include <stdio.h>
// #include <string.h>
#include <limits.h> /* to use LONG_MIN, LONG_MAX */
#include <ctype.h>
#include <stdlib.h> /* for strtol() */
#include "str.h"

/* for StrToLong function */
#define OVERFLOW 1
#define UNDERFLOW - 1

/* StrGetLength: Count the length of string, excluding the terminating null 
    pcSrc: a pointer of string

    returns: the number of characters in *pcSrc
*/

size_t StrGetLength(const char * pcSrc) {
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */

  const char * pcEnd = pcSrc;

  while ( * pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/* StrCopy: Copy the string 
    pcDest: a pointer of string which will copy *pcSrc
    pcSrc: a pointer of string which will be copied

    returns: the pointer pcDest
*/

char * StrCopy(char * pcDest, const char * pcSrc) {
  assert(pcDest);
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */

  char * pcTemp = pcDest;
  /* Until *pcSrc == NULL, copy *pcSrc to *pcDest */
  while ( * pcSrc) {
    /* null character and FALSE are identical. */
    * pcDest = * pcSrc;
    pcDest++;
    pcSrc++;
  }
  * pcDest = * pcSrc; // Copy the terminating null character.

  return pcTemp;
}

/* StrCompare: Compare the two strings 
    pcS1: a pointer of one string
    pcS2: a pointer of the other string

    returns: the integer less than, equal to, or greater than 0 if
    s1 is found, to be less than, to match, or to be grater than s2,
    respectively.
*/

int StrCompare(const char * pcS1, const char * pcS2) {
  assert(pcS1);
  assert(pcS2); /* NULL address, 0, and FALSE are identical. */

  while (1) {
    /* take each character of string *pcS1, *pcS2 */
    if ( * pcS1 < * pcS2) return -1;
    else if ( * pcS1 > * pcS2) return 1;

    if ( * pcS1) {
      /* null character and FALSE are identical. */
      pcS1++;
      pcS2++;
    } 
    else return 0;
  }
}

/* StrFindChr: Find the first occurrence of specific character in string
    pcHaystack: a pointer of string to look up the character c
    c: a character to look up

    returns: the pointer to the first occurrence of c in *pcHaystack
    if not found, then return NULL.
    if c == NULL, then return a pointer of terminator of *pcHaystack
*/

char * StrFindChr(const char * pcHaystack, int c) {
  assert(pcHaystack); /* NULL address, 0, and FALSE are identical. */

  for (;* pcHaystack != (char) c; pcHaystack++) {
    /* Move until the terminating null of *pcHaystack */
    if (! * pcHaystack) {
      /* null character and FALSE are identical. */
      /* If c == '\0', then return a pointer of terminator of *pcHaystack */
      if (!c) return (char * ) pcHaystack; /* null and FALSE are identical. */
      else return NULL; /* Otherwise, c is not found. return NULL. */
    }
  }

  return (char * ) pcHaystack;
}

/* StrFindStr: Find the first occurrence of specific substring in string
    pcHaystack: a pointer of string to look up the substring *pcNeedle
    pcNeedle: a pointer of substring to look up

    returns: the pointer to the beginning of the first occurrence of 
    *pcNeedle in *pcHaystack. (NOT compare terminating NULL)
    if not found, then return NULL.
*/

char * StrFindStr(const char * pcHaystack, const char * pcNeedle) {
  assert(pcHaystack);
  assert(pcNeedle); /* NULL address, 0, and FALSE are identical. */

  const char * pcTemp = pcNeedle; // to save the address of original Needle
  const char * pcTgt = pcHaystack; // to save the address of candidate

  while ( * pcHaystack) {
    /* null character and FALSE are identical. */
    
    /* Different from the first character of Needle */
    if ( * pcHaystack != * pcNeedle) pcHaystack++;
    
    /* Same as the first character of Needle */
    else {
      pcTgt = pcHaystack; // save the address of candidate
      
      while ( * pcNeedle) {
        /* null character and FALSE are identical. */
        /* Different from the character of Needle */
        if ( * pcHaystack != * pcNeedle) {
          pcNeedle = pcTemp; //reset the pcNeedle
          pcHaystack = pcTgt; //reset the pcHaystack
          pcHaystack++;
          break;
        }
        /* Same as the character of Needle */
        else {
          pcHaystack++;
          pcNeedle++;
        }
      }

      if (! * pcNeedle) return (char * ) pcTgt;
    }
  }

  /* If *pcHaystack == '\0', then two cases.
     If *pcNeedle == '\0', then correctly find. return pcTgt;
     Otherwise, cannot find string of Neddle. return NULL */
  if (! * pcNeedle) return (char * ) pcTgt; 
  else return NULL;
}

/* StrConcat: Append one string to the other string
    pcDest: a pointer of string which will append *pcSrc from terminating NULL
    pcSrc: a pointer of string which will be appended

    returns: the pointer pcDest
*/

char * StrConcat(char * pcDest, const char * pcSrc) {
  assert(pcDest);
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */

  char * pcTemp = pcDest;

  while ( * pcDest) pcDest++; /* null character and FALSE are identical. */
  /* Here, *pcDest == '\0'. Use StrCopy to copy pcSrc from there. */
  pcDest = StrCopy(pcDest, pcSrc);

  return pcTemp;
}

/* IsNotValid: Check whether overflow or underflow occur when
	converting string into long int value in StrToLong
	  num: converted number right before converting exactly temp
    temp: one number (0~9) s.t. exactly converting into num. 
    
    returns: 1 if overflow occur. -1 if underflow occur. 0 if safe.
*/

int IsNotValid(long int num, long int temp) {
  /* First : Check whether 10 * num occur over/underflow */
  long int temp2 = 10 * num;
  if ((num > 0) && (num != temp2 / 10)) return OVERFLOW;
  if ((num < 0) && (num != temp2 / 10)) return UNDERFLOW;

  /* Second : Check whether 10 * num + temp occur over/underflow */
  if ((num > 0) && (10 * num > LONG_MAX - temp)) return OVERFLOW;
  if ((num < 0) && (10 * num < LONG_MIN + temp)) return UNDERFLOW;

  /* Here, we are safe :) */
  return 0;
}

/* SetEndptr: Set an endptr (not NULL) of StrToLong function
    nptr: a pointer of string containing number with terminating NULL
    
    returns: the pointer pointing the first character which is not valid
*/

char * SetEndptr(char * nptr) {
  char * endptr = nptr;

  while ( * nptr && (isspace( * nptr))) nptr++;
  if ( * nptr == '-') nptr++;
  else if ( * nptr == '+') nptr++;

  while ( * nptr && (isdigit( * nptr))) endptr = ++nptr;

  return endptr;
}

/* StrToLong: Convert a string to a long integer
    nptr: a pointer of string containing number with terminating NULL
    endptr: a pointer to the string where conversion was stopped
    base: only 10. if not, then return 0
    
    returns: the result of conversion unless overflow or underflow occur.
    if overflow, return LONG_MAX. if underflow, return LONG_MIN.
*/

long int StrToLong(const char * nptr, char ** endptr, int base) {
  assert(nptr); /* NULL address, 0, and FALSE are identical. */
  if (base != 10) return 0; /* base can be only 10 by assumption */

  char * pcTemp = (char * ) nptr;
  if (endptr) * endptr = SetEndptr(pcTemp);

  /* skip spaces */
  while ( * nptr && (isspace( * nptr))) nptr++;

  /* optional '+' or '-' sign. Note that if not exist such things,
  	 then automatically + so that need to consider only '-' case. */
  char sign = '+'; /* To store the sign */
  if ( * nptr == '-') {
    sign = '-';
    nptr++;
  } 
  else if ( * nptr == '+') nptr++;

  /* change str into long int */
  long int num = 0; /* the result number */
  long int temp = 0; /* number for one temporaily number *nptr */

  while ( * nptr && (isdigit( * nptr))) {
    temp = * nptr - '0';
    /*OVERFLOW */
    if (IsNotValid(num, temp) == OVERFLOW) {
      num = LONG_MAX;
      break;
    }
    /* UNDERFLOW */
    else if (IsNotValid(num, temp) == UNDERFLOW) {
      num = LONG_MIN;
      break;
    }
    /* Otherwise, safe :) */
    else {
      if (sign == '+') num = 10 * num + temp;
      else num = 10 * num - temp;
    }
    nptr++;
  }

  return num;
}