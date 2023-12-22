/**
 * @file SigHandlers.h
 *
 * @brief Handlers for system signals.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

/**
 * Sets a handler for SIGTERM, SIGINT, and SIGQUIT signals.
 * 
 * @param handler A callback to be used to handle termination signals.
 */
void setTerminationHandlers(void (*handler)(int));