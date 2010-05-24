/**
 * \file errno.h
 * \brief Error codes
 * \author Adrien Forest
 * \version 0.1
 * \date 24 Avril 2010
 *
 */

#ifndef __ERRNO_H
#define __ERRNO_H

enum
{
  NOTFOUND = -11, /*!< These are not the droid you are looking for */
  OUTOMEM,                // Out of memory
  UNKNPID,                      // Unknown pid (process identifier)
  UNKNMID,                      // Unknown mid (message identifier)
  INVPRI,                       // Invalid priority
  OUTOPID,                      // Out of pid (number of processes is limited)
  OUTOMID,                      // Out of mid (number of messages is limited)
  NULLPTR,                      // Null pointer error
  INVARG,                       // Invalid argument
  INVEID,                       // Invalid ID
  FAILNOOB,                     // General error
  OMGROXX                       // No error occured
};

#endif //__ERRNO_H
