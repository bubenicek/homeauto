/*******************************************************************************
*  Filename:        hal_lcd_srf05.c
*  Revised:         $Date: 2014-06-26 16:08:45 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13249 $
*
*  Description:     Functions for controlling the LCD (Solomon Systech SSD1801)
*					on SmartRF05EB
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
#include <util.h>


/*******************************************************************************
 * CONSTANTS AND DEFINES
 */

// LCD instruction set definitions
// Set power save mode
#define OSC_OFF                         0x00
#define OSC_ON                          0x01
#define POWER_SAVE_OFF                  0x00
#define POWER_SAVE_ON                   0x02
#define SET_POWER_SAVE_MODE(options)    lcdControl(0x0C | (options))

// Function Set
#define CGROM                           0x00
#define CGRAM                           0x01
#define COM_FORWARD                     0x00
#define COM_BACKWARD                    0x02
#define TWO_LINE                        0x00
#define THREE_LINE                      0x04
#define FUNCTION_SET(options)           lcdControl(0x10 | (options))

// Set Display Start Line
#define LINE1                           0x00
#define LINE2                           0x01
#define LINE3                           0x02
#define LINE4                           0x03
#define SET_DISPLAY_START_LINE(line)    lcdControl(0x18 | (line))

// Set bias control
#define BIAS_1_5                        0x00
#define BIAS_1_4                        0x01
#define SET_BIAS_CTRL(bias)             lcdControl(0x1C | (bias))

// Power control
#define VOLTAGE_DIVIDER_OFF             0x00
#define VOLTAGE_DIVIDER_ON              0x01
#define CONVERTER_AND_REG_OFF           0x00
#define CONVERTER_AND_REG_ON            0x04
#define SET_POWER_CTRL(options)         lcdControl(0x20 | (options))

// Set display control
#define DISPLAY_CTRL_ON                 0x01
#define DISPLAY_CTRL_OFF                0x00
#define DISPLAY_CTRL_BLINK_ON           0x02
#define DISPLAY_CTRL_BLINK_OFF          0x00
#define DISPLAY_CTRL_CURSOR_ON          0x04
#define DISPLAY_CTRL_CURSOR_OFF         0x00
#define SET_DISPLAY_CTRL(options)       lcdControl(0x28 | (options))

// Set DD/CGRAM address
#define SET_DDRAM_ADDR(charIndex)       lcdControl(0x80 | (charIndex))
#define SET_GCRAM_CHAR(specIndex)       lcdControl(0xC0 | ((specIndex) << 3))

// Set ICONRAM address
#define CONTRAST_CTRL_REGISTER          0x10
#define SET_ICONRAM_ADDR(addr)          lcdControl(0x40 | (addr))

// Set double height
#define LINE_1_AND_2                    0x01
#define LINE_2_AND_3                    0x02
#define NORMAL_DISPLAY                  0x00
#define SET_DOUBLE_HEIGHT(options)      lcdControl(0x08 | (options))

// LCD lines
#define LCD_LINE_COUNT                  3
#define LCD_LINE_LENGTH                 16


/*******************************************************************************
 * LOCAL VARIABLES
 */
static char XDATA pLcdLineBuffer[LCD_LINE_LENGTH];

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void lcdControl(uint8 command);
static void lcdWrite(uint8 ch);
static void lcdWriteMany(const char *pData, uint16 count);
static void lcdWriteLine(uint8 line, const char *pText);

/*******************************************************************************
 * @fn          lcdControl
 *
 * @brief       Send command to display
 *
 * @param       uint8 command
 *
 * @return      none
 */
static void lcdControl(uint8 command) {
    LCD_SPI_BEGIN();
    LCD_DO_CONTROL();
    LCD_SPI_TX(command);
    LCD_SPI_WAIT_RXRDY();
    LCD_SPI_END();
}

/*******************************************************************************
 * @fn          lcdWrite
 *
 * @brief       Write character to display
 *
 * @param       uint8 ch - character to display
 *
 * @return      none
 */
static void lcdWrite(uint8 ch) {
    LCD_SPI_BEGIN();
    LCD_DO_WRITE();
    LCD_SPI_TX(ch);
    LCD_SPI_WAIT_RXRDY();
    LCD_SPI_END();
}

/*******************************************************************************
 * @fn          lcdWriteMany
 *
 * @brief       Write text to display
 *
 * @param       char *pData - text buffer
 *              uint16 count - number of bytes
 *
 * @return      none
 */
static void lcdWriteMany(const char XDATA *pData, uint16 count) {
    LCD_SPI_BEGIN();
    LCD_DO_WRITE();
    while (count--) {
        LCD_SPI_TX(*(pData++));
        LCD_SPI_WAIT_RXRDY();
    }
    LCD_SPI_END();
}

/*******************************************************************************
 * @fn          lcdWriteLine
 *
 * @brief       Write line on display
 *
 * @param       uint8 line - display line
 *              char *pText - text buffer to write
 *
 * @return      none
 */
static void lcdWriteLine(uint8 line, const char XDATA *pText) {
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH);
    lcdWriteMany(pText, LCD_LINE_LENGTH);
    return;
}


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * @fn          halLcdInit
 *
 * @brief       Initalise LCD
 *
 * @param       none
 *
 * @return      none
 */
void halLcdInit(void) {
    // Initialize I/O
    LCD_CTRL_INIT_PORTS();

    // Perform the initialization sequence
    FUNCTION_SET(CGRAM | COM_FORWARD | THREE_LINE);
    halLcdSetContrast(15);
    SET_POWER_SAVE_MODE(OSC_OFF | POWER_SAVE_ON);
    SET_POWER_CTRL(VOLTAGE_DIVIDER_ON | CONVERTER_AND_REG_ON);
    SET_BIAS_CTRL(BIAS_1_5);
    halMcuWaitMs(21); // 21 ms

    // Clear the display
    halLcdClear();
    halLcdClearAllSpecChars();
    SET_DISPLAY_CTRL(DISPLAY_CTRL_ON | DISPLAY_CTRL_BLINK_OFF | DISPLAY_CTRL_CURSOR_OFF);
    return;
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
    uint8 n;
    SET_DDRAM_ADDR(0x00);
    for (n = 0; n < (LCD_LINE_COUNT * LCD_LINE_LENGTH); n++) {
        lcdWrite(' ');
    }
    return;
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
void halLcdSetContrast(uint8 value) {
    SET_ICONRAM_ADDR(CONTRAST_CTRL_REGISTER);
    lcdWrite(value);
    return;
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
uint8 halLcdGetLineLength(void) {
    return LCD_LINE_LENGTH;
}


/*******************************************************************************
 * @fn          halLcdGetNumLines
 *
 * @brief       Get number of lines on display
 *
 * @param       nono
 *
 * @return      uint8 - number of characters on a line
 */
uint8 halLcdGetNumLines(void) {
  return LCD_LINE_COUNT;
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
void halLcdWriteChar(uint8 line, uint8 col, char text) {
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH + col);
    lcdWrite(text);
    return;
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
void halLcdWriteLine(uint8 line, const char XDATA *pLine) {
    uint8 n;
    if (pLine) {
        for (n = 0; n < LCD_LINE_LENGTH; n++) {
            if (*pLine) {
                pLcdLineBuffer[n] = *(pLine++);
            } else {
                pLcdLineBuffer[n] = ' ';
            }
        }
        lcdWriteLine(line, pLcdLineBuffer);
    }
    return;
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
void halLcdWriteLines(const char XDATA *pLine1, const char XDATA *pLine2, const char XDATA *pLine3) {
    if (pLine1) halLcdWriteLine(1, pLine1);
    if (pLine2) halLcdWriteLine(2, pLine2);
    if (pLine3) halLcdWriteLine(3, pLine3);
    return;
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
void halLcdDisplayValue(uint8 line, char XDATA *pLeft, int32 value, char XDATA *pRight) {
    uint8 n;
    char *pValue = convInt32ToText(value);
    for (n = 0; n < LCD_LINE_LENGTH; n++) {
        if (pLeft && *pLeft) {
            pLcdLineBuffer[n] = *(pLeft++);
        } else if (*pValue) {
            pLcdLineBuffer[n] = *(pValue++);
        } else if (pRight && *pRight) {
            pLcdLineBuffer[n] = *(pRight++);
        } else {
            pLcdLineBuffer[n] = ' ';
        }
    }
    lcdWriteLine(line, pLcdLineBuffer);
    return;
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
void halLcdDisplayCounters(uint8 line, int32 lValue, char lChar, int32 rValue, char rChar) {
    uint8 n;
    char *pCounter;

    // Left part
    pLcdLineBuffer[0] = lChar;
    pLcdLineBuffer[1] = '=';
    pCounter = convInt32ToText(lValue);
    for (n = 2; n < 8; n++) {
        if (*pCounter) {
            pLcdLineBuffer[n] = *(pCounter++);
        } else {
            pLcdLineBuffer[n] = ' ';
        }
    }
    pLcdLineBuffer[8] = rChar;
    pLcdLineBuffer[9] = '=';
    pCounter = convInt32ToText(rValue);
    for (n = 10; n < 16; n++) {
        if (*pCounter) {
            pLcdLineBuffer[n] = *(pCounter++);
        } else {
            pLcdLineBuffer[n] = ' ';
        }
    }
    lcdWriteLine(line, pLcdLineBuffer);
    return;
}

static const char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
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
void halLcdDisplayUint16(uint8 line, uint8 radix, uint16 value) {
    if (radix == HAL_LCD_RADIX_DEC) {
        uint8 n = 0;
        char *pValue = convInt32ToText((uint32)value);
        while (*pValue)
            halLcdWriteChar(line, n++, *(pValue++));
    }
    else if (radix == HAL_LCD_RADIX_HEX) {
        halLcdWriteChar(line, 0, '0');
        halLcdWriteChar(line, 1, 'x');
        halLcdWriteChar(line, 2, hex[(value & 0xF000) >> 12]);
        halLcdWriteChar(line, 3, hex[(value & 0x0F00) >>  8]);
        halLcdWriteChar(line, 4, hex[(value & 0x00F0) >>  4]);
        halLcdWriteChar(line, 5, hex[(value & 0x000F)]);
    }
    return;
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
void halLcdClearAllSpecChars(void) {
    uint8 n;
    SET_GCRAM_CHAR(0);
    for (n = 0; n < (8 * 8); n++) {
        lcdWrite(0x00);
    }
    return;
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
void halLcdCreateSpecChar(uint8 index, const char XDATA *p5x8Spec) {
    uint8 n;
    SET_GCRAM_CHAR(index);
    for (n = 0; n < 8; n++) {
        lcdWrite(p5x8Spec[n]);
    }
    return;
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
void halLcdWriteSpecChar(uint8 line, uint8 col, uint8 index) {
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH + col);
    lcdWrite(index);
    return;
}