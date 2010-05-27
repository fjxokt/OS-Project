/**
 * @file kprogram.c
 * @brief Implementation of the kprogram module
 */

#include <string.h>
#include "kprogram.h"
#include "kernel.h"

#include "../user/inf.h"

/*
 * Define
 */

#define NUM_PROG 2

/*
 * Global variable
 */

static prgm     prgm_ls[NUM_PROG] = {
  /*
   * The init program
   */
  {
   "init",
   (uint32_t) & init,
   "Initialize the system."},
	{
   "inf",
   (uint32_t) & inf,
   "Print from 1 to infinity."}
};

/**
 * @brief Search a program into the program list
 * @param *name a pointer to a string, wich represent a possible
 * programm name
 * @return: a pointer to the program, or NULL if not found
 */
prgm           *
search_prgm(char *name)
{
  uint32_t        i;

  for (i = 0; i < NUM_PROG; i++)
    if (strcmp(name, prgm_ls[i].name) == 0)
      return &(prgm_ls[i]);

  return NULL;
}

/**
 * @brief Print all the program with description
 * @param void
 * @return void
 */
void
print_programs(void)
{

}
