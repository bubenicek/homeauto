/**
 * \file app.h       \brief Application common definitions
 */
 
#ifndef __APP_H
#define __APP_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "system.h"
#include "bootloader/app_header.h"

#include "emontask.h"

//
// Global objects
//
extern __attribute__((section(".app_header"))) const app_header_t app_header;

#endif // __APP_H
