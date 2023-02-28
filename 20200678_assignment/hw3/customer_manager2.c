/* customer_manager2.c
  Assignment #3, Task 2
  Created by Gyouk Chu on 23/10/2021
  Last Updated on 29/10/2021
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_BUCKET_SIZE 1024
#define MAX_BUCKET_SIZE 1048576 // pow(2,20)
#define failure -1
#define success 0

enum {HASH_MULTIPLIER = 0x741B8CD7};
//0x741B8CD7 is an odd num of CRC32

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo *next_id;  // pointing next userinfo in table_id
  struct UserInfo *next_name;// pointing next userinfo in table_name
};

struct DB {
  struct UserInfo **ptable_id;   // pointer to the table(id)
  struct UserInfo **ptable_name; // pointer to the table(name)
  /* each table is (dynamic) array of UserInfo pointers */
  int curBucketSize;  // max bucket size of table
  int numItems; // # of stored items
};

void Resize(DB_T d); // definition later 

/* Hash_function: Hash the string
  
    pcKey: string that we want to hash
    iBucketCount: number of buckets

    returns: integer hash value between 0 and (iBucketCount-1)
*/
static int Hash_function(const char *pcKey, int iBucketCount)
{
  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
          + (unsigned int)pcKey[i];
  //unsigned overflow is dealt with modulo in math
  return (int)(uiHash % (unsigned int) iBucketCount);
}

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
  /* start with 1024 bucket entries for table(id) */
  d->curBucketSize = UNIT_BUCKET_SIZE;
  d->ptable_id = (struct UserInfo **)calloc(d->curBucketSize,
                sizeof(struct UserInfo *));            
  if (d->ptable_id == NULL) {
    fprintf(stderr, "Can't allocate a memory for table(id) of size %d\n",
	    d->curBucketSize);   
    free(d);
    return NULL;
  }
  /* start with 1024 bucket entries for table(name) */
  d->ptable_name = (struct UserInfo **)calloc(d->curBucketSize,
               sizeof(struct UserInfo *));

  if (d->ptable_name == NULL) {
    fprintf(stderr,"Can't allocate a memory for table(name) of size %d\n",
	    d->curBucketSize);  
    free(d->ptable_id); 
    free(d);
    return NULL;
  }
  d->numItems = 0;

  return d;
}

/* DestroyCustomerDB: Deallocate memory for DB_T & its underlying objects

    d: DB_T that need to be freed
*/
void DestroyCustomerDB(DB_T d)
{
  if (d == NULL) return; // do nothing if d is NULL

  struct UserInfo *temp;
  struct UserInfo *nexttemp;
  for(int i=0;i<d->curBucketSize; i++) {
    for(temp = d->ptable_id[i]; temp!=NULL; temp = nexttemp) {
      nexttemp = temp->next_id;
      free(temp->id); free(temp->name);
      free(temp);
    }
  }
  free(d->ptable_id);
  free(d->ptable_name);
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
  /* failure if id already exists */
  int hash_id = Hash_function(id, d->curBucketSize);
  for(temp=d->ptable_id[hash_id]; temp !=NULL; temp=temp->next_id) {
    if(strcmp(temp->id, id) == 0) return failure;
  }
  
  /* failure if name already exists */
  int hash_name = Hash_function(name, d->curBucketSize);
  for(temp=d->ptable_name[hash_name]; temp!=NULL; temp=temp->next_name) {
    if(strcmp(temp->name, name) == 0) return failure;
  }

  struct UserInfo *newuser = (struct UserInfo *)calloc(1, 
                                          sizeof(struct UserInfo));
  if(newuser == NULL) return failure; // failure if calloc fails
  
  /* failure if copying(strdup) id and name fails */
  newuser->id = (char *) strdup(id);
  if (newuser->id == NULL) {free(newuser); return failure;}
  newuser->name = (char *) strdup(name);
  if (newuser->name == NULL) {
    free(newuser); free(newuser->id); 
    return failure;
  }
  newuser->purchase = (int) purchase;
  newuser->next_id = d->ptable_id[hash_id];
  newuser->next_name = d->ptable_name[hash_name];
  d->ptable_id[hash_id] = newuser;
  d->ptable_name[hash_name] = newuser;
  d->numItems++;
  // expand hash table if condition satisfies
  if ((d->numItems >= (0.75 * d->curBucketSize)) && 
        (d->curBucketSize < MAX_BUCKET_SIZE)) {
    Resize(d); // rehash the DB
  }
  return success;
}

/* Resize: Double the number of buckets in hash table when # of items
        reaches 75% of # of buckets until 2^20(=1048576).
    
    d: existing DB_T
*/
void Resize(DB_T d) {
  if (d == NULL) assert(0);
  if (d->curBucketSize >= MAX_BUCKET_SIZE) assert(0);
  
  d->curBucketSize = d->curBucketSize * 2;
  struct UserInfo **oldtable_id, **newtable_id;
  struct UserInfo **oldtable_name, **newtable_name;
  struct UserInfo *temp, *nexttemp;
  /* #1. Resize the table(id) */
  oldtable_id = d->ptable_id;
  newtable_id = (struct UserInfo **)calloc(d->curBucketSize,
                sizeof(struct UserInfo *));
  if (newtable_id == NULL) return; // calloc fails, cannot resize

  /* #2. Resize the table(name) */
  oldtable_name = d->ptable_name;
  newtable_name = (struct UserInfo **)calloc(d->curBucketSize,
                sizeof(struct UserInfo *));
  if (newtable_name == NULL) {  // calloc fails, cannot resize
    free(newtable_id); return;
  } 
  /* Newly register all the existing UserInfo and free old ones. */
  d->ptable_id = newtable_id;
  d->ptable_name = newtable_name;
  d->numItems = 0;
  for(int i=0;i<(d->curBucketSize/2); i++) {
    for(temp = oldtable_id[i]; temp!=NULL; temp = nexttemp) {
      RegisterCustomer(d,temp->id, temp->name, temp->purchase);
      nexttemp = temp->next_id;
      free(temp->name); free(temp->id);
      free(temp);
    }
  }
  free(oldtable_id);
  free(oldtable_name);
  return;
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
  
  int hash_id = Hash_function(id, d->curBucketSize);
  /* need to free the target UserInfo */
  struct UserInfo *tar = NULL;
  if(d->ptable_id[hash_id] == NULL) return failure;
  /* Handle the case when d->ptable_id[hash_id] is exactly target */
  if(strcmp(d->ptable_id[hash_id]->id, id)==0) {
    tar = d->ptable_id[hash_id];
    d->ptable_id[hash_id] = d->ptable_id[hash_id]->next_id;
  }
  else{
    for(struct UserInfo *temp = d->ptable_id[hash_id]; temp!=NULL;){
      if((temp->next_id != NULL) && 
          (strcmp(temp->next_id->id,id)==0)) {
        struct UserInfo * temp2 = temp->next_id;
        tar = temp2;
        temp->next_id = temp2->next_id;
      }
      else  temp = temp->next_id;
    }
  }

  if(tar == NULL) return failure;
  /* reconstruct the table(name) also */
  int hash_name = Hash_function(tar->name, d->curBucketSize);
  if(strcmp(d->ptable_name[hash_name]->name, tar->name)==0) {
    d->ptable_name[hash_name] = d->ptable_name[hash_name]->next_name;
  }
  else{
    for(struct UserInfo *temp=d->ptable_name[hash_name]; temp!=NULL;){
      if((temp->next_name != NULL) && 
          (strcmp(temp->next_name->name,tar->name)==0)) {
        struct UserInfo *temp2 = temp->next_name;
        temp->next_name = temp2->next_name;
      }
      else  temp = temp->next_name;
    }
  }
  free(tar->name); free(tar->id);
  free(tar);
  d->numItems--;
  return success;
}

/* UnregisterCustomerByName: Unregister user from DB_T when name matches

    d: existing DB_T
    name: User name to be unregistered

    returns: 0 if success, -1 otherwise.
*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
  if (d == NULL) return failure; // failure if d is NULL
  if (name == NULL) return failure; // failure if id is NULL

  int hash_name = Hash_function(name, d->curBucketSize);
  /* need to free the target UserInfo */
  struct UserInfo *tar = NULL;
  if(d->ptable_name[hash_name] == NULL) return failure;
  /* Handle the case when d->ptable_name[hash_name] is exactly target */
  if(strcmp(d->ptable_name[hash_name]->name, name)==0) {
    tar = d->ptable_name[hash_name];
    d->ptable_name[hash_name] = d->ptable_name[hash_name]->next_name;
  }
  else{
    for(struct UserInfo *temp=d->ptable_name[hash_name]; temp!=NULL;){
      if((temp->next_name != NULL) && 
          (strcmp(temp->next_name->name,name)==0)) {
        struct UserInfo *temp2 = temp->next_name;
        tar = temp2;
        temp->next_name = temp2->next_name;
      }
      else  temp = temp->next_name;
    }
  }

  if(tar == NULL) return failure;
  /* reconstruct the table(id) also */
  int hash_id = Hash_function(tar->id, d->curBucketSize);
  if(strcmp(d->ptable_id[hash_id]->id, tar->id)==0) {
    d->ptable_id[hash_id] = d->ptable_id[hash_id]->next_id;
  }
  else{
    for(struct UserInfo *temp=d->ptable_id[hash_id]; temp!=NULL;){
      if((temp->next_id != NULL) && 
          (strcmp(temp->next_id->id,tar->id)==0)) {
        struct UserInfo *temp2 = temp->next_id;
        temp->next_id = temp2->next_id;
      }
      else  temp = temp->next_id;
    }
  }

  free(tar->name); free(tar->id);
  free(tar);
  d->numItems--;
  return success;
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

  int hash_id = Hash_function(id, d->curBucketSize);
  
  struct UserInfo *temp;
  for(temp=d->ptable_id[hash_id]; temp!=NULL; temp=temp->next_id) {
    if(strcmp(temp->id, id)==0) return temp->purchase;
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
  if (name == NULL) return failure; // failure if id is NULL

  int hash_name = Hash_function(name, d->curBucketSize);
  
  struct UserInfo *temp;
  for(temp=d->ptable_name[hash_name]; temp!=NULL; temp=temp->next_name) {
    if(strcmp(temp->name, name)==0) return temp->purchase;
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

  for(int i=0;i<d->curBucketSize; i++) {
    for(temp = d->ptable_id[i]; temp!=NULL; temp = temp->next_id) {
      int fpvalue = fp((const char*) temp->id, (const char*) temp->name,
              (const int) temp->purchase);
      if(fpvalue<0) continue; //invalid
      else result = result + fpvalue;
    }
  }

  return result;
}