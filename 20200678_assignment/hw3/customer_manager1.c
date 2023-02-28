/* customer_manager1.c
  Assignment #3, Task 1
  Edited by Gyouk Chu on 21/10/2021
  Last Updated on 28/10/2021
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024
#define failure -1
#define success 0

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
};

/* CreateCustomerDB: Allocate memory for new DB_T & its underlying objects

    returns: newly generated DB_T if success. NULL otherwise.
*/
DB_T CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  /* start with 1024 elements */
  d->curArrSize = UNIT_ARRAY_SIZE; 
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));

  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  d->numItems = 0; // start with 0 items
  return d;
}

/* DestroyCustomerDB: Deallocate memory for DB_T & its underlying objects

    d: DB_T that need to be freed
*/
void DestroyCustomerDB(DB_T d)
{
  if (d == NULL) return; // do nothing if d is NULL

  struct UserInfo *temp;
  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    free(temp->id);
    free(temp->name);
  }
  free(d->pArray);
  free(d);

  return;
}

/* RegisterCustomer: Deallocate memory for DB_T & its underlying objects

    d: existing DB_T
    id: User id to be registered
    name: User name to be registered
    purchase: purchase amount of user to be registered

    returns: 0 if success, -1 otherwise.
*/
int RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (id == NULL) return failure; // failure if id is NULL
  if (name == NULL) return failure; // failure if name is NULL
  if (purchase <= 0) return failure; // failure if purchase is not pos.
  
  struct UserInfo *temp;
  int emptyindex = -1;
  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) {
      if(emptyindex < 0) emptyindex = i;
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    /* failure if id or name already exists */
    else {
      if((strcmp(temp->id, id) == 0) || (strcmp(temp->name, name) == 0))
        return failure;
    }
  }

  struct UserInfo newuser = {(char *) strdup(name), 
             (char *) strdup(id), (int) purchase};
  /* failure if copying(strdup) id and name fails */
  if ((newuser.name == NULL) || (newuser.id == NULL)) {
    if (newuser.name != NULL) free(newuser.name); 
    if (newuser.id != NULL) free(newuser.id); 
    return failure;
  }

  /* Array should be extended if there is no empty space */
  if(d->numItems == d->curArrSize) {
    struct UserInfo *backup = d->pArray;
    d->pArray = (struct UserInfo *) realloc(d->pArray, 
             (d->curArrSize + UNIT_ARRAY_SIZE) * sizeof(struct UserInfo));
    if(d->pArray == NULL) {
      d->pArray = backup; return failure; // failure if cannot be extended
    }
    else {
      memset(d->pArray + d->curArrSize, 0, 
            (UNIT_ARRAY_SIZE) * sizeof(struct UserInfo));
      // initialize to NULL for newly generated part
      emptyindex = d->curArrSize;
      d->curArrSize = d->curArrSize + UNIT_ARRAY_SIZE;
    }
  }

 if (d->pArray[emptyindex].name != NULL) 
    free(d->pArray[emptyindex].name); 
  d->pArray[emptyindex] = newuser;
  d->numItems++;
  return success;
}

/* UnregisterCustomerByID: Unregister user from DB_T when ID matches

    d: existing DB_T
    id: User id to be unregistered

    returns: 0 if success, -1 otherwise.
*/
int UnregisterCustomerByID(DB_T d, const char *id)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (id == NULL) return failure; // failure if id is NULL
  
  struct UserInfo *temp;

  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) { 
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    if(strcmp(temp->id, id) == 0) {
      d->numItems--;
      free(temp->id);
      temp->id = NULL; // to remark 'un'registration
      return success;
    }
  }

  return failure;
}

/* UnregisterCustomerByName: Unregister user from DB_T when name matches

    d: existing DB_T
    name: User name to be unregistered

    returns: 0 if success, -1 otherwise.
*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (name == NULL) return failure; // failure if name is NULL
  
  struct UserInfo *temp;

  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) { 
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    if(strcmp(temp->name, name) == 0) {
      d->numItems--;
      free(temp->id);
      temp->id = NULL; // to remark 'un'registration
      return success;
    }
  }

  return failure;
}

/* GetPurchaseByID: Search a purchase amount from DB_T when ID matches

    d: existing DB_T
    id: User id to search

    returns: corresponding purchase amount if success, -1 otherwise.
*/
int GetPurchaseByID(DB_T d, const char* id)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (id == NULL) return failure; // failure if id is NULL

  struct UserInfo *temp;

  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) { 
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    if(strcmp(temp->id, id) == 0) return temp->purchase;
  }

  return failure;
}

/* GetPurchaseByName: Search a purchase amount from DB_T when name match

    d: existing DB_T
    name: User name to search

    returns: corresponding purchase amount if success, -1 otherwise.
*/
int GetPurchaseByName(DB_T d, const char* name)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (name == NULL) return failure; // failure if name is NULL

  struct UserInfo *temp;

  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) { 
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    if(strcmp(temp->name, name) == 0) return temp->purchase;
  }

  return failure;
}

/* GetSumCustomerPurchase: Calculate sum of numbers returned by fp 
   by iterating all user items.

    d: existing DB_T
    fp: function pointer that takes user's id, name, and purchase
    as parameters and returns nonnegative integer.

    returns: corresponding total number if success, -1 otherwise.
*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (fp == NULL) return failure; // failure if fp is NULL
  
  int result = 0;
  struct UserInfo *temp;

  for(int i=0;i<d->curArrSize; i++) {
    temp = d->pArray + i;
    if(temp->id == NULL) { 
      if(temp->name == NULL) break; //completely blank
      else continue; //next element may exist
    }
    else {
      int fpvalue = fp((const char*) temp->id, (const char*) temp->name,
              (const int) temp->purchase);
      if(fpvalue<0) continue; //invalid
      else result = result + fpvalue; 
    }
  }

  return result;
}
