/*
 * File: warmup.c
 * this is the warmup exercise from the assignment advice page. It is
 * strongly recommended you use this exercise to get your void* bearings
 * before jumping into the assignment. Fill in the blanks in the code
 * below, build and test, and work through any issues that come up. Don't
 * move on until you are sure your understanding is complete and rock-solid,
 * especially in terms of the proper level of indirection.
 * After you complete the warmup, keep this code around. You can re-purpose
 * this program to work through additional CVector/CMap issues (use of 
 * cleanup,search/sort comparison callback, etc.) that come up when writing 
 * the assignment. Having a little playground for experimentation and exploration
 * of client use of void* interfaces is handy!
 */



#include "cvector.h"
#include "cmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void stringsFreeCallBack(void *element)
{
        char **stringPtr = element;
        free(*stringPtr);
}

static void integers()
{
        int entries[] = {34, 15, 17};
        int n = sizeof(entries)/sizeof(entries[0]);
        CVector *cv = NULL;
//      int *intPtr = entries;
        cv = cvec_create(sizeof(int*), n, NULL); // create CVector to hold ints

        for (int i = 0; i < n; i++) {       // add values from entries array
                cvec_append(cv,&entries[i]);
        }
//      int *cur = cvec_first(cv);
//      int x = sizeof(int);4
//      int y = sizeof(int*);8
//      printf("size of int: %d\n",x);
//      printf("size of intPtr: %d\n", y);

        printf("First = %d\n", *(int*)cvec_first(cv)); //print first elem
        cvec_dispose(cv);
}




static void strings()
{
        char *entries[] = {"watermelon", "kiwi", "honeydew"};
        int n = sizeof(entries)/sizeof(entries[0]);

        CVector *cv = cvec_create(sizeof(char*), n, stringsFreeCallBack);
        for (int i = 0; i < n; i++) {   // add values from entries array
                char *cur = entries[i];
                char *heaped = strdup(cur);
                cvec_append(cv,&heaped);
        }

        printf("First = %s\n", *(char**)cvec_first(cv)); //print first elem
        cvec_dispose(cv);
}
//old
/*
static void strings()
{
        char *entries[] = {"watermelon", "kiwi", "honeydew"};
        int n = sizeof(entries)/sizeof(entries[0]);

        CVector *cv = cvec_create( sizeof(char*), n, NULL );      // filled in for you this time

        for (int i = 0; i < n; i++) {   // add values from entries array
                cvec_append(cv,&entries[i]);
        }

        printf("First = %s\n", *(char**)cvec_first(cv)); //print first elem
        cvec_dispose(cv);
}
*/








int main(int argc, char *argv[])
{
        printf("This is the warmup client program for CVector/CMap.\n");
        integers();
        strings();
        return 0;
}


