/*******************************************************************************
*  Filename:        util_menu.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Single-key menu for console operation.
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


#ifdef WIN32
/*******************************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "util_menu.h"
#include "util_console.h"
#include <ctype.h>
#include <string.h>

/*******************************************************************************
* CONSTANTS
*/
#define LINE_LEN    52

/*******************************************************************************
* MACROS
*/


/*******************************************************************************
* TYPEDEFS
*/


/*******************************************************************************
* GLOBAL VARIABLES
*/
static Menu menu[N_MAX_MENUES];
static Menu *pCurrent= &menu[0];

// Reserved menu entries
#if defined (CC2430EB) || defined(CC2430EM)
static const MenuItem miHelp= { 'h', "Help", NULL };
static const MenuItem miQuit= { 'q', "Back", NULL };
#else
static MenuItem miHelp= { 'h', "Help", NULL };
static MenuItem miQuit= { 'q', "Back", NULL };
#endif

/*******************************************************************************
* FUNCTIONS
*/

/*******************************************************************************
* @fn      menuInit
* @brief   Initialise the menu system
* @param   none
* @param   none
*/
void menuInit(void) {
    memset (menu,0,sizeof(Menu));
}


/*******************************************************************************
 * @fn      menuInstall
 * @brief   Install a menu for single key commands.
 * @param   iMenuID - menu identifier (MENU_ID_SYS | MENU_ID_APP)
 * @param   pMenuHandle - menu table
 * @param   szPr - prompt to be used with this menu
 * @return  none
 */
void menuInstall(uint8 iMenuID, MenuItem *pMenuHandle, const char FAR *szPr) {
    menu[iMenuID].pParent= &menu[MENU_ID_ROOT];
    menu[iMenuID].pFirst= pMenuHandle;
    menu[iMenuID].szPrompt= szPr;
}


/*******************************************************************************
 * @fn      menuIsInstalled
 * @brief   -
 * @param   none
 * @return  TRUE if the menu is installed.
 */
uint8 menuIsInstalled(uint8 iMenuID) {
    return menu[iMenuID].pFirst!= NULL;
}


/*******************************************************************************
 * @fn      menuSelect
 * @brief   Change to another menu
 * @param   iMenuID - menu to select
 * @return  none
 */
void menuSelect(uint8 iMenuID)
{
    pCurrent= &menu[iMenuID];
}


/*******************************************************************************
 * @fn      menuPrintPrompt
 * @brief   Print the prompt of the current menu
 * @param   none
 * @return  none
 */
void menuPrintPrompt(void) {
    printChar('\n');
    printStr((char FAR *)pCurrent->szPrompt);
}


/*******************************************************************************
 * @fn      menuPrintItem
 * @brief   Print a single menu item
 * @param   Pointer to menu item
 * @return  none
 */
void menuPrintItem(MenuItem FAR *p) {
    if (isprint(p->cmd)) {
        printChar(' ');
        printChar((char)toupper(p->cmd));
    } else {
        printChar('^');
        printChar((char)('A'+toupper(p->cmd)-1));
    }
    printStr(": ");
    printStr((char FAR *)p->szDecsr);
    printStr("\n");
}


/*******************************************************************************
* @fn      menuPrint
* @brief   Print the complete menu (help).
* @param   none
* @return  none
*/
void menuPrint(void) {
    MenuItem FAR *p;

#ifdef WIN32
    system("cls");
#else
    printChar('\n');
#endif

    printLine('-', LINE_LEN);

    // Always print 'help' entry.
    menuPrintItem((MenuItem FAR *)&miHelp);

    // Print menu entries
    p= pCurrent->pFirst;
    while (p->cmd!= '\0') {
		if (p->pfCmd!=NULL)
			menuPrintItem(p);
        p++;
    }

    // Print 'quit' entry (except for root menu).
    if ( pCurrent != &menu[MENU_ID_ROOT] ) {
        menuPrintItem((MenuItem FAR *)&miQuit);
    }
    printLine('-', LINE_LEN);

}


/*******************************************************************************
* @fn      menuFindItem
* @brief   Extract a menu item.
* @param   pMenu - menu handle
* @param   cmd - command to search for
* @return  menu item pointer
*/
MenuItem FAR *menuFindItem(Menu *pMenu, uint8 cmd) {
    MenuItem FAR *p;

    p= pMenu->pFirst;
    while (p->cmd!='\0' && toupper(p->cmd)!=toupper(cmd)) {
        p++;
    }
    return p->cmd=='\0' ? NULL : p;
}

/*******************************************************************************
* @fn      menuCmdExec
* @brief   Process a single key command in the currently active menu.
* @param   pBuf - buffer to keep input from operator
* @param   nBytes - number of bytes received
* @return  1 if the test was executed, 0 otherwise
*/
uint8 menuCmdExec(uint8 *pBuf, uint8 nBytes) {
    MenuItem FAR *pMenuItem;
    uint8 c, r;

    c= *pBuf;
	r= 0;

    // Menu look up ......
    pMenuItem= menuFindItem(pCurrent, c);

	if (pMenuItem!=NULL && pMenuItem->pfCmd!=NULL) {

        // Execute the command
        printChar('\n');
        (pMenuItem->pfCmd)();
		r= 1;

    } else if (tolower(c)=='q') {

        // Back to parent menu
        pCurrent= pCurrent->pParent;
        menuPrint();

    } else if (tolower(c)=='h') {

        // Print help
        menuPrint();

    } else if (!isspace(c)) {

        // Invalid command
        printStr("Invalid command!");
    } // else:   no action on whitespace input

    menuPrintPrompt();
	return r;
}

#endif