/**
 * testing sound
 *
 * @date 22.07.2010
 * @author Darya Dzendzik
*/

#ifndef   SOUND_AND_GRAPHIC_TEST_H
#define   SOUND_AND_GRAPHIC_TEST_H

typedef   unsigned short int	UWORD;
typedef   unsigned char			UBYTE;
typedef   unsigned long			ULONG;
typedef   struct
{
  UBYTE   FormatMsb;
  UBYTE   FormatLsb;
  UBYTE   DataBytesMsb;
  UBYTE   DataBytesLsb;
  UBYTE   ItemsX;
  UBYTE   ItemsY;
  UBYTE   ItemPixelsX;
  UBYTE   ItemPixelsY;
  UBYTE   Data[];
}
FONT;

#define   SAMPLEWORD            ULONG
#define   SAMPLETONENO          16        // No of tone samples
#define   SOUNDIntEnable        {\
									*(unsigned*)AT91C_SSC_IER     = AT91C_SSC_ENDTX;\
                                }

#define   SOUNDIntDisable       {\
									*(unsigned*)AT91C_SSC_IDR     = AT91C_SSC_ENDTX;\
                                }

#define   DURATION_MIN          10        // [mS]
#define   FREQUENCY_MIN         220       // [Hz]
#define   FREQUENCY_MAX         14080     // [Hz]
#define   SOUNDVOLUMESTEPS      4
#define   TRUE                  1
#define   FALSE                 0
#define   OSC                   48054850L
#define   SAMPLEWORDBITS        (sizeof(SAMPLEWORD) * 8)
#define   SAMPLEWORDS           8
#define   DISP_BUFFER_P ((UBYTE*)&(pMapDisplay->Normal))
#define   pMapDisplay ((IOMAPDISPLAY*)(pHeaders[ENTRY_DISPLAY]->pIOMap))
#define   TRANSLATE_Y(_y) ((DISPLAY_HEIGHT-1) - (_y))
#define   DISPLAY_HEIGHT      64        // Y pixels
#define   DISPLAY_WIDTH       100       // X pixels
#define   SPI_BITRATE         2000000
#define   FILENAME_LENGTH     19        // zero termination not included

enum
{
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

enum      TEXTLINE_NO                   // Used in macro "TEXTLINE_BIT"
{
  TEXTLINE_1,                           // Upper most line
  TEXTLINE_2,                           //
  TEXTLINE_3,                           //
  TEXTLINE_4,                           //
  TEXTLINE_5,                           //
  TEXTLINE_6,                           //
  TEXTLINE_7,                           //
  TEXTLINE_8,                           // Buttom line
  TEXTLINES
};

SAMPLEWORD		ToneBuffer[SAMPLETONENO];
SAMPLEWORD 		SampleBuffer[16][SAMPLEWORDS];
UBYTE     SoundReady;                // Sound channel ready (idle)
UWORD     MelodyPointer;
UBYTE     SoundDivider;              // Volume
UWORD     SoundSamplesLeft;          // Number of samples left on actual sound buffer
UWORD     SoundSamplesLeftNext;      // Number of samples left on next sound buffer
ULONG     ToneCycles;                // No of tone cycles
ULONG     ToneCyclesReady;           // No of tone cycles for ready

extern const SAMPLEWORD TonePattern[SOUNDVOLUMESTEPS + 1][SAMPLETONENO];

typedef   struct
{
  UBYTE   FormatMsb;
  UBYTE   FormatLsb;
  UBYTE   DataBytesMsb;
  UBYTE   DataBytesLsb;
  UBYTE   ItemsX;
  UBYTE   ItemsY;
  UBYTE   ItemPixelsX;
  UBYTE   ItemPixelsY;
  UBYTE   Data[];
}ICON;

typedef   struct
{
  ULONG   ModuleID;
  UBYTE   ModuleName[FILENAME_LENGTH + 1];
  void    (*cInit)(void* pHeader);
  void    (*cCtrl)(void);
  void    (*cExit)(void);
  void    *pIOMap;
  void    *pVars;
  UWORD   IOMapSize;
  UWORD   VarsSize;
  UWORD   ModuleSize;
}HEADER;

static    HEADER    **pHeaders;

typedef   struct
{
  UBYTE   FormatMsb;
  UBYTE   FormatLsb;
  UBYTE   DateBytesMsb;
  UBYTE   DataBytesLsb;
  UBYTE   StartX;
  UBYTE   StartY;
  UBYTE   PixelsX;
  UBYTE   PixelsY;
  UBYTE   Data[];
}BMPMAP;

enum      SCREEN_NO                     // Used in macro "SCREEN_BIT"
{
  SCREEN_BACKGROUND,                    // Entire screen
  SCREEN_LARGE,                         // Entire screen except status line
  SCREEN_SMALL,                         // Screen between menu icons and status line
  SCREENS
};

enum      BITMAP_NO                     // Used in macro "BITMAP_BIT"
{
  BITMAP_1,                             // Bitmap 1
  BITMAP_2,                             // Bitmap 2
  BITMAP_3,                             // Bitmap 3
  BITMAP_4,                             // Bitmap 4
  BITMAPS
};

enum      MENUICON_NO                   // Used in macro "MENUICON_BIT"
{
  MENUICON_LEFT,                        // Left icon
  MENUICON_CENTER,                      // Center icon
  MENUICON_RIGHT,                       // Right icon
  MENUICONS
};

enum      STATUSICON_NO                 // Used in macro "STATUSICON_BIT"
{
  STATUSICON_BLUETOOTH,                 // BlueTooth status icon collection
  STATUSICON_USB,                       // USB status icon collection
  STATUSICON_VM,                        // VM status icon collection
  STATUSICON_BATTERY,                   // Battery status icon collection
  STATUSICONS
};

enum      STEP_NO                       // Used in macro "STEPICON_BIT"
{
  STEPICON_1,                           // Left most step icon
  STEPICON_2,                           //
  STEPICON_3,                           //
  STEPICON_4,                           //
  STEPICON_5,                           // Right most step icon
  STEPICONS
};

typedef   struct
{
  void    (*pFunc)(UBYTE,UBYTE,UBYTE,UBYTE,UBYTE,UBYTE);    // Simple draw entry

  ULONG   EraseMask;                                        // Section erase mask   (executed first)
  ULONG   UpdateMask;                                       // Section update mask  (executed next)

  FONT    *pFont;                                           // Pointer to font file
  UBYTE   *pTextLines[TEXTLINES];                           // Pointer to text strings

  UBYTE   *pStatusText;                                     // Pointer to status text string
  ICON    *pStatusIcons;                                    // Pointer to status icon collection file

  BMPMAP  *pScreens[SCREENS];                               // Pointer to screen bitmap file
  BMPMAP  *pBitmaps[BITMAPS];                               // Pointer to free bitmap files

  UBYTE   *pMenuText;                                       // Pointer to menu icon text                (NULL == none)
  UBYTE   *pMenuIcons[MENUICONS];                           // Pointer to menu icon images              (NULL == none)

  ICON    *pStepIcons;                                      // Pointer to step icon collection file

  UBYTE   *Display;                                         // Display content copied to physical display every 17 mS

  UBYTE   StatusIcons[STATUSICONS];                         // Index in status icon collection file     (index = 0 -> none)

  UBYTE   StepIcons[STEPICONS];                             // Index in step icon collection file       (index = 0 -> none)

  UBYTE   Flags;                                            // Update flags enumerated above

  UBYTE   TextLinesCenterFlags;                             // Mask to center TextLines

  UBYTE   Normal[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH];        // Raw display memory for normal screen
  UBYTE   Popup[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH];         // Raw display memory for popup screen
}IOMAPDISPLAY;


#endif /* SOUND_AND_GRAPHIC_TEST_H */
