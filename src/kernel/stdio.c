/**
 * \file stdio.c
 * \brief Display functions
 * \author Adrien Forest
 * \version 0.1
 * \date 20 Mai 2010
 */

#include <string.h>
#include <errno.h>
#include "ksyscall.h"

/**
 * Print the string str to the standard output.
 */
int
print(char *str)
{
  int32_t         e;
  do
    e = syscall_one((int32_t) str, PRINT);
  while (e != OMGROXX);

  return e;
}

 /**
 * Print the carriage return character.
 */
int
printn(void)
{
  return print("\n");
}

 /**
 * Print the number n to the standard output.
 */
int
printi(int n)
{
  char            buf[10];
  return print(itos(n, buf));
}

/**
 * Print the string str to the specified output.
 */
int
fprint(int out, char *str)
{
  return syscall_two(out, (int32_t) str, FPRINT);
}

/**
 * Returns the character currently pointed by the internal file position indicator of the input stream.
 */
char
getc(void)
{
  return 'c';
}

/**
 * Reads characters from stream and stores them as a string into str until (num-1) characters have
been readen or either a newline or a the End-of-File is reached, whichever comes first. stream
(keyboard). A null character is added to the end.
 */
int
gets(char *str, int num)
{
  int32_t         e;
  do
    e = syscall_two((int32_t) str, num, READ);
  while (e != OMGROXX);

  return e;
}
