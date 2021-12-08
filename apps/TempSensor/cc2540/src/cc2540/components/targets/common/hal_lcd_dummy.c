/*******************************************************************************
*  Filename:        hal_lcd_dummy.c
*  Revised:         $Date: 2014-06-26 16:08:45 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13249 $
*
*  Description:     Dummy LCD driver. Stubs for use on targets without LCD, thus 
*                   avoiding messy compiler switches in the application code.
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


/*******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "hal_lcd.h"
#include "hal_mcu.h"
#include "hal_board.h"



/*******************************************************************************
 * CONSTANTS AND DEFINES
 */



/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

 /******************************************************************************
 * @fn          halLcdInit
 *
 * @brief       Initalise LCD
 *
 * @param       none
 *
 * @return      none
 */
void halLcdInit(void)
{
}

/*******************************************************************************
 * @fn          halLcdClear
 *
 * @brief       Clear all lines on display
 *
 * @param       none
 *
 * @return      none
 */
void halLcdClear(void)
{
}

/*******************************************************************************
 * @fn          halLcdSetContrast
 *
 * @brief       Set display contrast
 *
 * @param       uint8 value - contrast value
 *
 * @return      none
 */
void halLcdSetContrast(uint8 value)
{
}

/*******************************************************************************
 * @fn          halLcdGetLineLength
 *
 * @brief       Get max number of characters on each line
 *
 * @param       nono
 *
 * @return      uint8 - number of characters on a line
 */
uint8 halLcdGetLineLength(void)
{
    return 0;
}

/*******************************************************************************
 * @fn          halLcdWriteChar
 *
 * @brief       Write single character
 *
 * @param       uint8 line - display line
 *              uint8 col - column
 *              char text - character to display
 *
 * @return      none
 */
void halLcdWriteChar(uint8 line, uint8 col, char text)
{
}

/*******************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write line on display
 *
 * @param       uint8 line - display line
 *              char *pLine2 - pointer to text buffer to write
 *
 * @return      none
 */
void halLcdWriteLine(uint8 line, const char XDATA *pLine)
{
}

/*******************************************************************************
 * @fn          halLcdWriteLines
 *
 * @brief       Write lines on display
 *
 * @param       char *pLine1 - pointer to text buffer for line 1
 *              char *pLine2 - pointer to text buffer for line 2
 *              char *pLine3 - pointer to text buffer for line 3
 *
 * @return      none
 */
void halLcdWriteLines(const char XDATA *pLine1, const char XDATA *pLine2, const char XDATA *pLine3)
{
}

/*******************************************************************************
 * @fn          halLcdDisplayValue
 *
 * @brief       Display value on display with optional text on right and left side
 *
 * @param       uint8 line
 *              char *pLeft
 *              int32 value
 *              char *pRight
 *
 * @return      none
 */
void halLcdDisplayValue(uint8 line, char XDATA *pLeft, int32 value, char XDATA *pRight)
{
}

/*******************************************************************************
 * @fn          halLcdDisplayCounters
 *
 * @brief       Display counter values with text on display
 *
 * @param       uint8 line - display line
 *              int32 lValue - left value
 *              char lChar - left text
 *              int32 rValue - right value
 *              char rChar - right text
 *
 * @return      none
 */
void halLcdDisplayCounters(uint8 line, int32 lValue, char lChar, int32 rValue, char rChar)
{
}

/*******************************************************************************
 * @fn          halLcdDisplayUint16
 *
 * @brief       Write a 16 bit value on the specified line on the LCD
 *
 * @param       uint16 value - value to display
 *              uint8 radix - must be HAL_LCD_RADIX_DEC
 *              uint8 line - line on display
 *
 * @return      none
 */
void halLcdDisplayUint16(uint8 line, uint8 radix, uint16 value)
{
}


/*******************************************************************************
 * @fn          halLcdClearAllSpecChars
 *
 * @brief       Clear special characters
 *
 * @param       none
 *
 * @return      none
 */
void halLcdClearAllSpecChars(void)
{
}

/*******************************************************************************
 * @fn          halLcdCreateSpecChar
 *
 * @brief       Create special character
 *
 * @param       uint8 index
 *              const char *p5x8Spec
 *
 * @return      none
 */
void halLcdCreateSpecChar(uint8 index, const char XDATA *p5x8Spec)
{
}

/*******************************************************************************
 * @fn          halLcdWriteSpecChar
 *
 * @brief       Write special character
 *
 * @param       uint8 line  - display line
 *              uint8 col   - column
 *              uint8 index - index of spec character
 *
 * @return      none
 */
void halLcdWriteSpecChar(uint8 line, uint8 col, uint8 index)
{
}