/* disp.h - Pilsan Display Header File */

#ifndef __INCdisph
#define __INCdisph

/* beep generator */

#define DIAG_CTRL	    0x61
#define BEEP_PITCH_L	1280 /* 932 Hz */
#define BEEP_PITCH_S	1208 /* 987 Hz */
#define BEEP_TIME_L		(sysClkRateGet () / 3) /* 0.66 sec */
#define BEEP_TIME_S		(sysClkRateGet () / 8) /* 0.15 sec */

/* timer (PIT) */

#define	PIT_BASE_ADR			0x40
#define	PIT0_INT_LVL			0x00
#define	PIT_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */
#define	PIT_CLOCK				1193180

/* display definitions */

#define MONOCHROME          0
#define VGA                 1
#define MONO			    0
#define COLOR			    1
#define	DISP_MEM_BASE		(UCHAR *) 0xb8000
#define	DISP_SEL_REG		(UCHAR *) 0x3d4
#define DISP_VAL_REG        (UCHAR *) 0x3d5
#define MONO_MEM_BASE       (UCHAR *) 0xb0000
#define MONO_SEL_REG        (UCHAR *) 0x3b4
#define MONO_VAL_REG        (UCHAR *) 0x3b5
#define	CHR			        2

#define	TEXT_MODE			0		/* display in text mode */
#define	INSERT_MODE_OFF		0		/* character insert mode off */
#define	INSERT_MODE_ON		1		/* character insert mode on */
#define	FG_ATTR_MASK		0x07	/* foreground attribute mask */
#define	BG_ATTR_MASK		0x70	/* background attribute mask */
#define	INT_BLINK_MASK		0x88	/* intensity and blinking mask */
#define	FORWARD				1		/* scroll direction forward */
#define	BACKWARD			0       /* scroll direction backward */

#define DEFAULT_FG  	ATRB_FG_WHITE
#define DEFAULT_BG 	    ATRB_BG_BLACK
#define DEFAULT_ATR     DEFAULT_FG | DEFAULT_BG
#define CTRL_SEL_REG	DISP_SEL_REG	/* controller select reg */
#define CTRL_VAL_REG	DISP_VAL_REG	/* controller value reg */
#define CTRL_MEM_BASE	DISP_MEM_BASE	/* controller memory base */
#define COLOR_MODE	    MONO			/* color mode */
#define ESC_NORMAL		0x0001		/* normal state */
#define NPARS	16			/* number of escape parameters */

#define UNDERLINE              0x01   /* only if monochrome */
#define ATRB_FG_BLACK		0x00
#define ATRB_FG_BLUE		0x01
#define ATRB_FG_GREEN		0x02
#define ATRB_FG_CYAN		0x03
#define ATRB_FG_RED			0x04
#define ATRB_FG_MAGENTA		0x05
#define ATRB_FG_BROWN		0x06
#define ATRB_FG_WHITE		0x07
#define ATRB_BRIGHT			0x08
#define ATRB_FG_GRAY		 (ATRB_FG_BLACK   | ATRB_BRIGHT)
#define ATRB_FG_LIGHTBLUE	 (ATRB_FG_BLUE    | ATRB_BRIGHT)
#define ATRB_FG_LIGHTGREEN	 (ATRB_FG_GREEN   | ATRB_BRIGHT)
#define ATRB_FG_LIGHTCYAN	 (ATRB_FG_CYAN    | ATRB_BRIGHT)
#define ATRB_FG_LIGHTRED	 (ATRB_FG_RED     | ATRB_BRIGHT)
#define ATRB_FG_LIGHTMAGENTA (ATRB_FG_MAGENTA | ATRB_BRIGHT)
#define ATRB_FG_YELLOW		 (ATRB_FG_BROWN   | ATRB_BRIGHT)
#define ATRB_FG_BRIGHTWHITE	 (ATRB_FG_WHITE   | ATRB_BRIGHT)
#define ATRB_BG_BLACK		0x00
#define ATRB_BG_BLUE		0x10
#define ATRB_BG_GREEN		0x20
#define ATRB_BG_CYAN		0x30
#define ATRB_BG_RED			0x40
#define ATRB_BG_MAGENTA		0x50
#define ATRB_BG_BROWN		0x60
#define ATRB_BG_WHITE		0x70
#define ATRB_BLINK			0x80
#define ATRB_CHR_REV		0x0100

/* disp console device descriptor */

typedef struct 
    {
    UCHAR *	memBase;		/* video memory base */
    UCHAR *	selReg;			/* select register */
    UCHAR *	valReg;			/* value register */
    int 	row, col;		/* current cursor position */
    UCHAR *	curChrPos;      	/* current character position */
    UCHAR	curAttrib;		/* current attribute  */
    UCHAR	defAttrib;      	/* current default attribute */
    int 	nrow, ncol;		/* current screen geometry */
    int 	scst, sced;		/* scroll region from to */
    BOOL	rev;			/* revarse mode char */
    BOOL	autoWrap;		/* auto Wrap mode */
    BOOL	sv_rev;			/* saved revarse mode char */
    int 	sv_row, sv_col;		/* saved cursor position */
    UCHAR	sv_curAttrib;	 	/* saved attributes */
    BOOL	scrollCheck;     	/* scroll check */
    UCHAR *	charSet;         	/* character set Text or Graphics */
    int		dispMode;         	/* added to support graphics Mode */
    BOOL	colorMode;       	/* color mode MONO / COLOR */
    BOOL	insMode;         	/* insert mode on / off */
    char	tab_stop [80];      	/* tab stop mark */
    UINT16	escFlags;		/* 16 bit escape flags */
    int 	escParaCount;	 	/* seen escape parameters (count) */
    int 	escPara[NPARS];  	/* parameters */
    BOOL	escQuestion;     	/* ? mark in escape sequence */
    char	escResp[10];     	/* esc sequence response buffer */
    } DISP_CON_DEV;

#endif /* __INCdisph */

/* function prototypes */

void dispConBeep (BOOL mode);
void dispHrdInit (void);
void dispStatInit (void);
void dispClear (FAST DISP_CON_DEV *  pDispConDv, int position, UCHAR eraseChar);
void dispScreenRev (FAST DISP_CON_DEV * pDispConDv);
void dispScroll (FAST DISP_CON_DEV *  pDispConDv, int pos, int line, BOOL upDn, FAST UCHAR atr);
void dispDelLeftChar (FAST DISP_CON_DEV *  pDispConDv);
void dispCarriageReturn (FAST DISP_CON_DEV *  pDispConDv);
void dispBackSpace (FAST DISP_CON_DEV *  pDispConDv);
void dispTab (FAST DISP_CON_DEV *  pDispConDv);
void dispLineFeed (FAST DISP_CON_DEV * pDispConDv);
void dispCursorPos (FAST UINT16 pos);
void dispCursorOff (void);
void dispCursorOn (void);
void dispCharPos (FAST DISP_CON_DEV * pDispConDv, FAST UINT16 pos);
int dispWriteString (FAST DISP_CON_DEV * pDispConDv, char * text);
