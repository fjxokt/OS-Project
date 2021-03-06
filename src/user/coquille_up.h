/**
 * \file coquille_up.h
 * \brief Contains the shell programs chg_prio, ps, tuer, malta
 * \author Adrien Forest
 * \version 0.1
 * \date 26 Mai 2010
 */

#ifndef __COQUILLE_UP_H
#define __COQUILLE_UP_H

void            chg_prio(int argc, char *argv[]);

void            ps(int argc, char *argv[]);

void             tuer(int argc, char *argv[]);

void            malta(int argc, char *argv[]);

void            help(int argc, char *argv[]);

void            proc_info(int argc, char *argv[]);

#endif //__COQUILLE_UP_H
