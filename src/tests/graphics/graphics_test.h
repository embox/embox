/*
 * @file
 *
 * @date 29.09.2010
 * @author Darya Dzendzik
 */

#ifndef GRAPHICS_TEST_H_
#define GRAPHICS_TEST_H_

#include <types.h>

#define   TRANSLATE_Y(_y) ((DISPLAY_HEIGHT-1) - (_y))
#define   DISPLAY_HEIGHT      64        // Y pixels
#define   DISPLAY_WIDTH       100       // X pixels
#define   SPI_BITRATE         2000000
#define   FILENAME_LENGTH     19        // zero termination not included

enum {
	ENTRY_COMM,
	ENTRY_INPUT,
	ENTRY_BUTTON,
	ENTRY_DISPLAY,
	ENTRY_LOADER,
	ENTRY_LOWSPEED,
	ENTRY_OUTPUT,
	ENTRY_SOUND,
	ENTRY_IOCTRL,
	ENTRY_CMD,
	ENTRY_UI,
	ENTRY_FREE2,
	ENTRY_FREE3,
	ENTRY_FREE4,
	ENTRY_FREE5
};

typedef struct {
	uint8_t   FormatMsb;
	uint8_t   FormatLsb;
	uint8_t   DataBytesMsb;
	uint8_t   DataBytesLsb;
	uint8_t   ItemsX;
	uint8_t   ItemsY;
	uint8_t   ItemPixelsX;
	uint8_t   ItemPixelsY;
	uint8_t   Data[];
} FONT;

enum TEXTLINE_NO {  // Used in macro "TEXTLINE_BIT"
	TEXTLINE_1, // Upper most line
	TEXTLINE_2,
	TEXTLINE_3,
	TEXTLINE_4,
	TEXTLINE_5,
	TEXTLINE_6,
	TEXTLINE_7,
	TEXTLINE_8, // Buttom line
	TEXTLINES
};

#define   DISP_BUFFER_P ((uint8_t*)&(pMapDisplay->Normal))
#define   pMapDisplay ((IOMAPDISPLAY*)(pHeaders[ENTRY_DISPLAY]->pIOMap))

typedef struct {
	uint32_t  ModuleID;
	uint8_t   ModuleName[FILENAME_LENGTH + 1];
	void      (*cInit)(void* pHeader);
	void      (*cCtrl)(void);
	void      (*cExit)(void);
	void      *pIOMap;
	void      *pVars;
	uint16_t  IOMapSize;
	uint16_t  VarsSize;
	uint16_t  ModuleSize;
} HEADER;

typedef struct {
	uint8_t   FormatMsb;
	uint8_t   FormatLsb;
	uint8_t   DateBytesMsb;
	uint8_t   DataBytesLsb;
	uint8_t   StartX;
	uint8_t   StartY;
	uint8_t   PixelsX;
	uint8_t   PixelsY;
	uint8_t   Data[];
} BMPMAP;

typedef struct {
	uint8_t   FormatMsb;
	uint8_t   FormatLsb;
	uint8_t   DataBytesMsb;
	uint8_t   DataBytesLsb;
	uint8_t   ItemsX;
	uint8_t   ItemsY;
	uint8_t   ItemPixelsX;
	uint8_t   ItemPixelsY;
	uint8_t   Data[];
} ICON;

enum SCREEN_NO {           // Used in macro "SCREEN_BIT"
	SCREEN_BACKGROUND, // Entire screen
	SCREEN_LARGE,      // Entire screen except status line
	SCREEN_SMALL,      // Screen between menu icons and status line
	SCREENS
};

enum BITMAP_NO {     // Used in macro "BITMAP_BIT"
	BITMAP_1,    // Bitmap 1
	BITMAP_2,    // Bitmap 2
	BITMAP_3,    // Bitmap 3
	BITMAP_4,    // Bitmap 4
	BITMAPS
};

enum MENUICON_NO {       // Used in macro "MENUICON_BIT"
	MENUICON_LEFT,   // Left icon
	MENUICON_CENTER, // Center icon
	MENUICON_RIGHT,  // Right icon
	MENUICONS
};

enum STATUSICON_NO {          // Used in macro "STATUSICON_BIT"
	STATUSICON_BLUETOOTH, // BlueTooth status icon collection
	STATUSICON_USB,       // USB status icon collection
	STATUSICON_VM,        // VM status icon collection
	STATUSICON_BATTERY,   // Battery status icon collection
	STATUSICONS
};

enum STEP_NO {      // Used in macro "STEPICON_BIT"
	STEPICON_1, // Left most step icon
	STEPICON_2,
	STEPICON_3,
	STEPICON_4,
	STEPICON_5, // Right most step icon
	STEPICONS
};

typedef struct {
	void    (*pFunc)(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t); // Simple draw entry
	uint32_t   EraseMask;    // Section erase mask   (executed first)
	uint32_t   UpdateMask;   // Section update mask  (executed next)
	FONT    *pFont;                 // Pointer to font file
	uint8_t *pTextLines[TEXTLINES]; // Pointer to text strings
	uint8_t *pStatusText;           // Pointer to status text string
	ICON    *pStatusIcons;          // Pointer to status icon collection file
	BMPMAP  *pScreens[SCREENS];     // Pointer to screen bitmap file
	BMPMAP  *pBitmaps[BITMAPS];     // Pointer to free bitmap files
	uint8_t *pMenuText;             // Pointer to menu icon text   (NULL == none)
	uint8_t *pMenuIcons[MENUICONS]; // Pointer to menu icon images (NULL == none)
	ICON    *pStepIcons;            // Pointer to step icon collection file
	uint8_t *Display;               // Display content copied to physical display every 17 mS
	uint8_t  StatusIcons[STATUSICONS]; // Index in status icon collection file (index = 0 -> none)
	uint8_t  StepIcons[STEPICONS];     // Index in step icon collection file (index = 0 -> none)
	uint8_t   Flags;                   // Update flags enumerated above
	uint8_t   TextLinesCenterFlags;    // Mask to center TextLines
	uint8_t   Normal[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH]; // Raw display memory for normal screen
	uint8_t   Popup[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH];  // Raw display memory for popup screen
} IOMAPDISPLAY;

#endif /* GRAPHICS_TEST_H_ */
