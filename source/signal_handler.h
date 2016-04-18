/**
 * @file signal_handler.h
 * @brief Header for signal handling
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#ifndef _SIGNAL_HANDLER_H
#define _SIGNAL_HANDLER_H

#define ERROR_UNKNOWN 5

extern int G_running;

void install_signal_handler(void);

#endif