/*******************************************************************************
*  Filename:        util_menu.h
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     UART based one-key menu.
*
*
*  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


#ifndef UTIL_MENU_H
#define UTIL_MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
* INCLUDES
*/


#include "hal_types.h"


/*******************************************************************************
 * CONSTANTS
 */
#define MENU_ID_APP     0
#define MENU_ID_ROOT    1
#define MENU_ID_NWK     2
#define MENU_ID_HAL     3
#define N_MAX_MENUES    4


/*******************************************************************************
 * TYPEDEFS
 */

typedef struct {
    uint8 cmd;				// Single key user command
    const char FAR *szDecsr;// Textual description
    void (*pfCmd)(void);    // Function; 0 arguments
} MenuItem;


typedef struct _menuItem {
    struct _menuItem *pParent;
    const char FAR *szPrompt;
    MenuItem FAR *pFirst;
} Menu;


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

void menuInit(void);
void menuInstall(uint8 iMenuID, MenuItem FAR *pMenuHandle, const char FAR *szPr);
uint8 menuCmdExec(uint8 *pBuf, uint8 nBytes);
void menuPrint(void);
void menuPrintPrompt(void);
void menuPrintItem(MenuItem FAR *p);
uint8 menuIsInstalled(uint8 iMenuID);
void menuSelect(uint8 iMenuID);
MenuItem FAR *menuFindItem(Menu *pMenu, uint8 cmd);

#ifdef __cplusplus
}
#endif

#endif