/**
 * @file SigHandlers.h
 *
 * @brief Handlers for system signals.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

void forceExit             (int    signal       );
void flagForTermination    (int    signal       );
void setTerminationHandlers(void (*handler)(int));