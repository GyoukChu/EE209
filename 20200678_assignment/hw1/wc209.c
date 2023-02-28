//
// wc209.c
// Assignment #1
// Created by Gyouk Chu on 06/09/2021.
// Last Updated on 11/09/2021.
//

#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h> // for isspace
#include <assert.h> // for defensive programming

enum DFASTATE { Is_Normal_WordOut, Is_Normal_WordIn, 
	Is_FacingSlash, Is_In_MultiLineComment, 
	Is_In_SingleLineComment, Is_FacingStar }; 

/* Main function : 
 * Print the number of newlines, words, 
 * and characters in standard input stream.
 */

int main() {

	int CurrentLine = 1;
	int temp = 0; // number for checking a line of comment error
	int NLCOUNT = 0; // number of newlines
	int WCOUNT = 0; // number of words
	int CCOUNT = 0; // number of characters
	int c = 0; // for each character of stdin
	enum DFASTATE beforeslash = Is_Normal_WordOut; // for checking state before slash
	enum DFASTATE state = Is_Normal_WordOut; 

	while ((c = getchar()) != EOF) {
	  
		switch (state) {
		  
			case Is_Normal_WordOut:
				if (c == '\n') { // check the newline char
			  		NLCOUNT++;
			  		CurrentLine++;
				}
			    if (isspace(c)) {CCOUNT++; break;} //same state
                else if (c != '/') { // inside word
			  	  state = Is_Normal_WordIn;
			  	  CCOUNT++; WCOUNT++; // out->in == one word
			  	  break;
			    }
            	else { // before comment
			  		state = Is_FacingSlash; 
					beforeslash = Is_Normal_WordOut; //save previous state
					break;
				} 
		  
		  	case Is_Normal_WordIn:
			  	if (c == '\n') { // check the newline char
			  		NLCOUNT++;
			  		CurrentLine++;
			  	}
			  	if (isspace(c)) { // outside word
			  	  	state = Is_Normal_WordOut;
			  	  	CCOUNT++;
			  	  	break;
			    }	
			    else if (c != '/') {CCOUNT++; break;} //same state
			    else { // before comment
					state = Is_FacingSlash; 
					beforeslash = Is_Normal_WordIn; //save previous state
					break;
				} 
		  
		  	case Is_FacingSlash:
			  	if (c == '\n') { // check the newline char
			  		NLCOUNT++;
			  		CurrentLine++;
			  	}
			  	if (c == '*') { // multiline comment start
					state = Is_In_MultiLineComment;
					break;
				}
			  	else if (c == '/') { // singleline comment start
				  	state = Is_In_SingleLineComment;
					break;
				} 
			  	else if (isspace(c)) { // '/' was just one of char
			  	  	state = Is_Normal_WordOut;
			  	  	if (beforeslash == Is_Normal_WordIn) {
						CCOUNT+=2; // because of c and existing '/' , add 2.
						break;
					} 
				 	else {
					 	CCOUNT+=2; 
					 	WCOUNT++; // since '/' becomes one word.
					 	break;
					} 
			  	} 
			  	else {
			  	  	state = Is_Normal_WordIn; 
			  	  	if (beforeslash == Is_Normal_WordIn) 
						{CCOUNT+=2; break;} //same reason
				  	else {CCOUNT+=2; WCOUNT++; break;} //same reason
			  	}  
			  
		  	case Is_In_MultiLineComment:
			  	if (c == '*') {state = Is_FacingStar; break;} // before comment end
			  	else if (c == '\n') { //check the newline char
			  		CCOUNT++; NLCOUNT++; 
			  		temp++; //for counting errorline, temp++
			  		break;
				}
			  	else {break;}
		  
		  	case Is_In_SingleLineComment:
			  	if (c == '\n') { // comment end
			  	  	state = Is_Normal_WordOut; 
			  	  	CCOUNT++;
			  	  	NLCOUNT++;
			  	  	CurrentLine++; 
			  	  	break;
			  	} 
			  	else break;
		  
		  	case Is_FacingStar:
			  	if (c == '/') { //comment end.
				  	state = Is_Normal_WordOut; 
					CCOUNT++; 
					CurrentLine+=temp; temp=0; // no error found. update currentline
					break;
				}
			  	else if (c == '*') {break;} // not end. still facing star.
			  	else if (c == '\n') {
				  	NLCOUNT++; CCOUNT++; temp++; //for errorline counting, temp++
				  	state = Is_In_MultiLineComment; break;
				} //check the newline char
			  	else {state = Is_In_MultiLineComment; break;}
		  
		  	default:
			  	assert(0);  // error
			  	break;
		}
	}
    // If comment is not terminated, then print standard error stream.
   	if (state == Is_In_MultiLineComment || state == Is_FacingStar) { 
	   	fprintf(stderr, "Error: line %d: unterminated comment\n", CurrentLine); 
	   	return EXIT_FAILURE;
   	} 
    // In valid case, print the result normally.
   	else {
   		printf("%d %d %d\n", NLCOUNT, WCOUNT, CCOUNT);
   		return EXIT_SUCCESS;
   	}
   	
}
 
