/* ish.c
  Assignment #5
  Created by Gyouk Chu on 20/11/2021
  Last Updated on 08/12/2021
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include "token.h"
#include "dynarray.h"
#include "analyzer.h"
#include "valid.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define INTR_SECONDS 5 // for Sighandler

enum {MAX_LINE_SIZE = 1024};
enum {FALSE, TRUE};

static int interrupted = 0; // for Sighandler

static void SIGQUITHandler (int iSig)
{
  interrupted++;
  assert((interrupted==1)||(interrupted==2));
  if (interrupted==1)
  {
    fprintf(stdout, "\nType Ctrl-\\ again within 5 seconds to exit.\n");
    alarm(INTR_SECONDS);
  }
  else
    exit(EXIT_SUCCESS);
}

static void SIGALRMHandler (int iSig)
{
  interrupted = 0;
}

char **tokentoargv(DynArray_T oTokens, char **new_argv)
{
  assert(oTokens); assert(new_argv);
  int n = DynArray_getLength(oTokens);
  for (int i = 0; i < n; i++) {
    new_argv[i] = strdup((char *)getTokenValue(DynArray_get(oTokens,i)));
  }
  new_argv[n] = NULL;
  return new_argv;
}

void execute_ishrc(char * acLine, const char * arg) 
{
  int isuccessful = FALSE; void *psuccessful;
  psuccessful = getenv("HOME");
	assert(psuccessful); // getenv fail
	isuccessful = chdir(psuccessful);
	assert(! isuccessful); // chdir fail
  DynArray_T oTokens;
  FILE* pFile = fopen(".ishrc", "r");
  /* fopen fail if .ishrc does not exist or not readable */
  if (pFile != NULL)  
  {
    while (fgets(acLine, MAX_LINE_SIZE, pFile) != NULL) {
      fprintf(stdout, "%% ");
      fprintf(stdout, "%s",acLine);
      /* ------------------------------------------*/
      /* Lexical Analysis */
      oTokens = DynArray_new(0);
      if (oTokens == NULL) // memory short
      {
        fprintf(stderr, "%s: %s\n", arg, strerror(errno));
        continue; // go to next command since failed
      }
      isuccessful = lexLine(acLine, oTokens);
      /* Failure cases - go to next command */
      if (! isvalid_lex(arg, isuccessful, oTokens)) 
      { freeoTokens(oTokens); continue; }
      /* Lexical Analysis Success */
      /* ------------------------------------------*/
      /* Syntatic Analysis */
      isuccessful = synLine(oTokens);
      if (! isvalid_syn(arg, isuccessful, oTokens)) 
      { freeoTokens(oTokens); continue; }
      // go to next command since failed
      /* Syntatic Analysis Success */
      int n = DynArray_getLength(oTokens);
      if (n==0) 
      {
        freeoTokens(oTokens);
        continue; // go to next command since nothing
      }
      /* ------------------------------------------*/
      /* Execute Commands */
      /* Built-in commands Case */
      char * new_argv_0 = (char *)getTokenValue(DynArray_get(oTokens,0));
      if ((strcmp(new_argv_0,"setenv")==0) || (strcmp(new_argv_0,"unsetenv")==0)
        || (strcmp(new_argv_0,"cd")==0) || (strcmp(new_argv_0,"exit")==0)) {
        if (! isvalid_builtin(arg, isuccessful, oTokens)) 
        { freeoTokens(oTokens); continue; }
      }
      /* Not Built-in commands Case - do fork */
      else
      {
        fflush(NULL);
        /* Extra Credit 1 - redirection */
        int is_redirectin = is_RedirectIn(oTokens);
        int is_redirectout = is_RedirectOut(oTokens);
        char *filein = NULL, *fileout = NULL;
        Token_T ptoken;
        if (is_redirectin) 
        {
          filein = strdup((char *)getTokenValue
                      (DynArray_get(oTokens,is_redirectin+1)));
          ptoken = DynArray_removeAt(oTokens, is_redirectin);
          freeToken(ptoken,NULL);
          ptoken = DynArray_removeAt(oTokens, is_redirectin);
          freeToken(ptoken,NULL);
          if(is_redirectout != 0) {is_redirectout--; is_redirectout--;}
          n--; n--;
        }
        if (is_redirectout) 
        {
          fileout = strdup((char *)getTokenValue
                      (DynArray_get(oTokens,is_redirectout+1)));
          ptoken = DynArray_removeAt(oTokens, is_redirectout);
          freeToken(ptoken,NULL);
          ptoken = DynArray_removeAt(oTokens, is_redirectout);
          freeToken(ptoken,NULL);
          n--; n--;
        }
        /* change oTokens into array of strings w/ NULL at the end */
        char **new_argv = malloc((n+1)*sizeof(char *));
        if (new_argv == NULL) //memory short
        {
          fprintf(stderr, "%s: %s\n", arg, strerror(errno));
          /* free all remaining Tokens and oToken */
          freeoTokens(oTokens);
          continue; // go to next command since failed
        }
        new_argv = tokentoargv(oTokens, new_argv);
        pid_t pid = fork();
        if (pid == 0)
        {
          /* child process */
          void (*pfRet) (int);
          int fd;
          /* Get back to the default signals */
          pfRet = signal(SIGINT, SIG_DFL);
          assert(pfRet != SIG_ERR);
          pfRet = signal(SIGQUIT, SIG_DFL);
          assert(pfRet != SIG_ERR);
          pfRet = signal(SIGALRM, SIG_DFL);
          assert(pfRet != SIG_ERR);
          /* Redirection_In */
          if (is_redirectin) {
            fd = open(filein, O_RDONLY);
            if (fd == -1) {
              fprintf(stderr, "%s: %s\n", arg, strerror(errno));
            }
            else 
            {
              close(0); dup(fd); close(fd);
            }
          }
          /* Redirection_OUT */
          if (is_redirectout) {
            fd = creat(fileout, 0600);
            close(1); dup(fd); close(fd);
          }
          execvp(new_argv[0], new_argv);
          fprintf(stderr, "%s: %s\n", new_argv[0], strerror(errno));
          exit(EXIT_FAILURE);
        }
        /* parent process */
        wait(NULL);
        if (filein) free(filein); 
        if (fileout) free(fileout);
        freenew_argv(new_argv, DynArray_getLength(oTokens));
      }
      freeoTokens(oTokens);
    }
  }
}

void execute_stdin(char *acLine, const char* arg, char *cur_dir) 
{
  int isuccessful;
  isuccessful = chdir(cur_dir);
  assert(! isuccessful); // chdir fail
  DynArray_T oTokens;
  /* Read a command from stdin repeatedly */
  while(1) {
    fprintf(stdout, "%% ");
		if (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL) {
      /* ------------------------------------------*/
      /* Lexical Analysis */
      oTokens = DynArray_new(0);
      if (oTokens == NULL) // memory short
      {
        fprintf(stderr, "%s: %s\n", arg, strerror(errno));
        continue; // go to next command since failed
      }
      isuccessful = lexLine(acLine, oTokens);
      /* Failure cases - go to next command */
      if (! isvalid_lex(arg, isuccessful, oTokens))
      { freeoTokens(oTokens); continue; }
      /* ------------------------------------------*/
      /* Syntatic Analysis */
      isuccessful = synLine(oTokens);
      if (! isvalid_syn(arg, isuccessful, oTokens))
      { freeoTokens(oTokens); continue; }
      // go to next command since failed
      /* Syntatic Analysis Success */
      int n = DynArray_getLength(oTokens);
      if (n==0) 
      {
        freeoTokens(oTokens);
        continue; // go to next command since nothing
      }
      /* ------------------------------------------*/
      /* Execute Commands */
      /* Built-in commands Case */
      char * new_argv_0 = (char *)getTokenValue(DynArray_get(oTokens,0));
      if ((strcmp(new_argv_0,"setenv")==0) || (strcmp(new_argv_0,"unsetenv")==0)
        || (strcmp(new_argv_0,"cd")==0) || (strcmp(new_argv_0,"exit")==0)){
        if (! isvalid_builtin(arg, isuccessful, oTokens))
        { freeoTokens(oTokens); continue; }
      }
      /* Not Built-in commands Case - do fork */
      else
      {
        fflush(NULL);
        /* Extra Credit 1 - redirection */
        int is_redirectin = is_RedirectIn(oTokens);
        int is_redirectout = is_RedirectOut(oTokens);
        char *filein = NULL, *fileout = NULL;
        Token_T ptoken;
        if (is_redirectin) 
        {
          filein = strdup((char *)getTokenValue
                      (DynArray_get(oTokens,is_redirectin+1)));
          ptoken = DynArray_removeAt(oTokens, is_redirectin);
          freeToken(ptoken,NULL);
          ptoken = DynArray_removeAt(oTokens, is_redirectin);
          freeToken(ptoken,NULL);
          if(is_redirectout != 0) {is_redirectout--; is_redirectout--;}
          n--; n--;
        }
        if (is_redirectout) 
        {
          fileout = strdup((char *)getTokenValue
                      (DynArray_get(oTokens,is_redirectout+1)));
          ptoken = DynArray_removeAt(oTokens, is_redirectout);
          freeToken(ptoken,NULL);
          ptoken = DynArray_removeAt(oTokens, is_redirectout);
          freeToken(ptoken,NULL);
          n--;n--;
        }
        /* change oTokens into array of strings w/ NULL at the end */
        char **new_argv = malloc((n+1)*sizeof(char *));
        if (new_argv == NULL) //memory short
        {
          fprintf(stderr, "%s: %s\n", arg, strerror(errno));
          /* free all remaining Tokens and oToken */
          freeoTokens(oTokens);
          continue; // go to next command since failed
        }
        new_argv = tokentoargv(oTokens, new_argv);
        pid_t pid = fork();
        if (pid == 0)
        {
          /* child process */
          void (*pfRet) (int);
          int fd;
          /* Get back to the default signals */
          pfRet = signal(SIGINT, SIG_DFL);
          assert(pfRet != SIG_ERR);
          pfRet = signal(SIGQUIT, SIG_DFL);
          assert(pfRet != SIG_ERR);
          pfRet = signal(SIGALRM, SIG_DFL);
          assert(pfRet != SIG_ERR);
          /* Redirection_In */
          if (is_redirectin) {
            fd = open(filein, O_RDONLY);
            if (fd == -1) {
              fprintf(stderr, "%s: %s\n", arg, strerror(errno));
            }
            else 
            {
              close(0); dup(fd); close(fd);
            }
          }
          /* Redirection_OUT */
          if (is_redirectout) {
            fd = creat(fileout, 0600);
            close(1); dup(fd); close(fd);
          }
          execvp(new_argv[0], new_argv);
          fprintf(stderr, "%s: %s\n", new_argv[0], strerror(errno));
          exit(EXIT_FAILURE);
        }
        /* parent process */
        wait(NULL);
        if (filein) free(filein); 
        if (fileout) free(fileout);
        freenew_argv(new_argv, DynArray_getLength(oTokens));
      }
      freeoTokens(oTokens);
    }
    else //EOF
    {
      fprintf(stdout ,"\n");
      break;
    }
  }
}

int main (int argc, const char *argv[]) {

  sigset_t sSet;

  /* Make sure signals are not blocked. */
  sigemptyset(&sSet);
  sigaddset(&sSet, SIGINT);
  sigaddset(&sSet, SIGQUIT);
  sigaddset(&sSet, SIGALRM);
  sigprocmask(SIG_UNBLOCK, &sSet, NULL);
  /* ---------------------------------- */
  /* Ignore the SIGINT signal */
  void (*pfRet) (int);
  pfRet = signal(SIGINT, SIG_IGN);
  assert(pfRet != SIG_ERR);
  /* Set the SIGQUIT signal */
  pfRet = signal(SIGQUIT, SIGQUITHandler);
  assert(pfRet != SIG_ERR);
  /* Set the SIGALRM signal */
  pfRet = signal(SIGALRM, SIGALRMHandler);
  assert(pfRet != SIG_ERR);

  char acLine[MAX_LINE_SIZE];
  
  char *cur_dir = get_current_dir_name();
  assert(cur_dir); // get_current_dir_name fail
  /* execute commands from .ishrc */
  execute_ishrc(acLine, argv[0]);
  
  /* commands from stdin */
  execute_stdin(acLine, argv[0], cur_dir);
  free(cur_dir);
  return 0;
}