/**
 * @file uart.c
 * @brief implemet the uart fuctions
 */

#include <stdlib.h>
#include "kinout.h"
#include "malta.h"
#include "uart.h"
#include "kprocess_list.h"
#include "kpcb_fifo.h"
#include "kscheduler.h"

/*
 * Global variable
 */

static fifo_buffer uart_fifo;

/*
 * The current user of the pcb
 */
static pcb     *user;

/*
 * Print and read buffer
 */
static char    *print_buffer;
static uint32_t print_index;
static char    *read_buffer;
static uint32_t read_buffer_length;

static bool     end_read;

/*
 * Current uart mode
 */
static int32_t  mode;

/*
 * Fifo functions
 */

/**
 * @brief reset the fifo buffer to default value
 * @param void
 * @return void
 */
void
reset_fifo_buffer(void)
{
  uart_fifo.in = 0;
  uart_fifo.out = 0;
  uart_fifo.length = 0;
}

/**
 * @brief push a char in the fifo buffer
 * @param the char to push
 * @return OMGROXX if there isan empty space OUTOMEM otherwise
 */
uint32_t
push_fifo_buffer(char c)
{
  if (uart_fifo.length >= UART_FIFO_SIZE)
    return OUTOMEM;

  uart_fifo.buffer[uart_fifo.in] = c;
  uart_fifo.length++;
  uart_fifo.in = (uart_fifo.in + 1) % UART_FIFO_SIZE;

  return OMGROXX;
}

/**
 * @brief pop a char from the fifo buffer
 * @parama a pointer to the char to pop
 * @return OMGROXX if the buffer is not empty, NULLPTR if the char given is null,  FAILNOOB otherwise
 */
uint32_t
pop_fifo_buffer(char *c)
{
  if (c == NULL)
    return NULLPTR;

  if (uart_fifo.length == 0)
    return FAILNOOB;

  *c = uart_fifo.buffer[uart_fifo.out];
  uart_fifo.length--;
  uart_fifo.out = (uart_fifo.out + 1) % UART_FIFO_SIZE;

  return OMGROXX;
}

/**
 * @brief return a pointer to the fifo_buffer struct
 *
 * --!!! This function is here for test purpose only! Don't use it !!!--
 *
 * @param void
 * @return a pointer to the fifo buffer
 */
fifo_buffer    *
get_fifo_buffer()
{
  return &uart_fifo;
}

/*
 * UART management functions
 */

void
uart_exception()
{
  switch (mode)
  {
  case UART_READ:
    /*uart_read(); */
    break;

  case UART_PRINT:
    uart_print();
    break;

  default:
    ;
  }
}


/**
 * @brief initialize the uart
 * @param void
 * @return void
 */
void
uart_init(void)
{
  /* Set UART word length ('3' meaning 8 bits).
   * Do this early to enable debug printouts (e.g. kdebug_print).
   */
  tty->lcr.field.wls = 3;

  /* Generate interrupts when data is received by UART. */
  tty->ier.field.erbfi = 1;

  /* Some obscure bit that need to be set for UART interrupts to work. */
  tty->mcr.field.out2 = 1;
  reset_fifo_buffer();
  reset_fifo_p();

  user = NULL;

  print_buffer = NULL;
  print_index = 0;
  read_buffer = NULL;
  read_buffer_length = 0;
  end_read = FALSE;
  mode = UART_UNUSED;
}

void
set_uart_user(pcb * p)
{
  user = p;
}

/*
 *
 */
int32_t
uart_give_to(pcb * p)
{

  if (user != NULL && pcb_get_pid(p) != pcb_get_pid(user))
  {
    /*
     * Try to add the pcb to the internal fifo list, and to move
     * it to the waiting list
     */

    if (push_fifo_p(p) != OMGROXX || kblock_pcb(p, WAITING_IO))
    {
      return FAILNOOB;
    }

    /*
     * We did not succeed to give you the UART !
     */
    return FAILNOOB;
  }

  user = p;

  return OMGROXX;
}

void
uart_set_mode(int32_t new_mode, char *str, uint32_t len)
{
  switch (new_mode)
  {
  case UART_PRINT:
    print_buffer = str;
    print_index = 0;
    mode = UART_PRINT;
    break;

  case UART_READ:
    read_buffer = str;
    read_buffer_length = len;
    end_read = FALSE;
    clean_uart();
    tty->ier.field.erbfi = 1;   /* interrupt when data is received */
    mode = UART_READ;
    break;

  default:
    break;
  }
}

/* cleans the uart's receiving buffer of any data that might be left in it */
void
clean_uart()
{
  char            c;
  while (tty->lsr.field.dr)
    c = tty->rbr;
}


/* prints a character from out_buffer and sets the device to interrupt when it is done printing (only if there are remaining characters to print) */
void
uart_print(void)
{
  char            c;

  //kprintln("In uart print");

  /*
   * Is the device ready ?
   */
  if (tty->lsr.field.thre)
  {
    //kprintln("Start printing");
    /*
     * Is there some char to print in the fifo already ?
     */
    if (uart_fifo.length != 0)
    {
      //kprintln("Already people waiting");
      pop_fifo_buffer(&c);
    }

    else
    {
      c = print_buffer[print_index++];
      /*
       * Special treatment for \n
       */
      if (c == '\n')
      {
        c = '\r';
        if (push_fifo_buffer('\n') != OMGROXX)
        {
          //kprintln("man, you fail");
          /*
           * End the print with an error
           */
          end_of_printing(FAILNOOB);
          return;
        }
      }
    }

    /*
     * Print the char
     */
    tty->thr = c;
  }

  /*
   * Is the next char the end ?
   */
  if (print_buffer[print_index] == '\0' && uart_fifo.length == 0)
  {
    //kprintln("End the print");
    /*
     * We end the print with no error
     */
    end_of_printing(OMGROXX);
  }
  /*
   * Not done, interrupt reactivate
   */
  else
  {
    //kprintln("Reactivate the exception");
    tty->ier.field.etbei = 1;
  }
}

/* ends the use of the device by its current owner by waking it up. It also release the device and put the return value in the PCB */
int32_t
end_of_printing(int32_t code)
{
  /*
   * UART unused now
   */
  mode = UART_UNUSED;

  /*
   * Since it's unused we stop the interrupt
   */
  tty->ier.field.etbei = 0;

  /*
   * Reset everything and release
   */
  reset_fifo_buffer();

  return uart_release(code);
}

int32_t
uart_release(int32_t code)
{
  pcb            *p;

  pcb_set_v0(user, code);

  /*
   * Wake up the owner
   */
  kwakeup_pcb(user);

  /*
   * Set back the user to null
   */
  user = NULL;

  /*
   * We pop the new user from the fifo list
   */
  p = pop_fifo_p();

  /*
   * If we found some one, we wake it up, and we set it
   * as the user
   */
  if (p != NULL)
  {
    pcb_set_state(p, WAITING_IO);

    kwakeup_pcb(p);

    user = p;
  }

  return OMGROXX;
}

/* end of file uart.c */
