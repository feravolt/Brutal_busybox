// in androids asm/signal.h, sigset_t is a simple unsigned long

#include <signal.h>

int sigisemptyset(const sigset_t *set)
{
  // Cast *set to unsigned long and compare with 0
  return (*(const unsigned long *)set) == 0;
}