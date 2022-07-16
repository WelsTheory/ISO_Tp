/*
 * printf_redirect.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "USART.h"

int _write(int fd, char *ptr, int len);

int _write(int fd, char* ptr, int len) {
  // Fail if trying to write to anything besides standard output (FD = 1).
  if (fd != 1) {
    errno = EBADF;
    return -1;
  }
  // Send the data out the UART a character at a time.
  int i;
  for (i = 0; i < len; i++)
    USART_Tx( ptr[i]);
	//USART_TX_6(ptr[i]);
  return i;
}

