#pragma once
#ifndef _SS_H
#define _SS_H

#include "include.h"

/**
 * @brief Initialize the screenshot module.
 * @return 0 on success, or -1 on error.
 */
int ss_init(void);

/**
 * @brief Get the dimensions of the screen.
 * @param width The width of the screen.
 * @param height The height of the screen.
 * @return 0 on success, or -1 on error.
 */
int ss_get_dimensions(int *, int *);

/**
 * @brief Take a screenshot and save it into the buffer.
 * @param buf The buffer to save the screenshot into.
 * @return The length of the buffer filled, or -1 on error.
 */
int ss_get_screenshot(char *);

/**
 * @brief Get the width of the screen.
 * @return The width of the screen.
 */
int ss_get_width(void);

/**
 * @brief Get the height of the screen.
 * @return The height of the screen.
 */
int ss_get_height(void);

#endif
