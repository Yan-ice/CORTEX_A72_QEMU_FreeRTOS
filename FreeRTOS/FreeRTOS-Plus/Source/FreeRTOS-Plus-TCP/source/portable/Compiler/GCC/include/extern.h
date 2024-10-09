#ifndef EXTERN_H
#define EXTERN_H

////////////////////////////////
// general function.

//this function should ensure critical code block execution.
extern void _std_enter_critical();
extern void _std_exit_critical();

////////////////////////////
// The function that support stdio.

extern void _std_putchar(char a);


////////////////////////////

#endif
