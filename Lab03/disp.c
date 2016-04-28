/* disp.c - Pilsan Display Driver Routines File */

/*
DESCRIPTION
This is a file for accessing the FHV board display.
It is based on WindRiver's file m6845vga.c, where a part
of the functionality has been excluded.
The private header file including defines and forward declarations is disp.h
*/

/* includes */

#include "intLib.h"
#include "taskLib.h"
#include "stdio.h"
#include "string.h"
#include "sysLib.h"
#include "disp.h"

/* globals */

DISP_CON_DEV	dispConDv;

/* locals */

LOCAL UCHAR		curSt,curEd;		/* current cursor mode */
LOCAL UCHAR * dispCharTable [] = { (unsigned char *)
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    " !\"#$%&'()*+,-./0123456789:;<=>?"
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
    "`abcdefghijklmnopqrstuvwxyz{|}~\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\040\255\233\234\376\235\174\025\376\376\246\256\252\055\376\376"
    "\370\361\375\376\376\346\024\371\376\376\247\257\254\253\376\250"
    "\376\376\376\376\216\217\222\200\376\220\376\376\376\376\376\376"
    "\376\245\376\376\376\376\231\376\235\376\376\376\232\376\376\341"
    "\205\240\203\376\204\206\221\207\212\202\210\211\215\241\214\213"
    "\376\244\225\242\223\376\224\366\233\227\243\226\201\376\376\230",
};


/*******************************************************************************
*
* dispConBeep - sound beep function (using timer 2 for tone)
* 
* This function is responsible for producing the beep 
* 
* PARAMETER: TRUE: long beep FALSE: short beep
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispConBeep ( BOOL mode ) {
    int		beepTime;
    int		beepPitch;
    FAST int 	oldlevel;

    if (mode) { 
        beepPitch = BEEP_PITCH_L;
        beepTime  = BEEP_TIME_L;	/* long beep */
    }
    else { 
        beepPitch = BEEP_PITCH_S;
        beepTime  = BEEP_TIME_S;	/* short beep */
    }

    oldlevel = intLock ();

    /* set command for counter 2, 2 byte write */

    sysOutByte (PIT_BASE_ADR + 3, 0xb6);	
    sysOutByte (PIT_BASE_ADR + 2, (beepPitch & 0xff));
    sysOutByte (PIT_BASE_ADR + 2, (beepPitch >> 8));

    /* enable counter 2 */
    sysOutByte (DIAG_CTRL, sysInByte (DIAG_CTRL) | 0x03);	

    taskDelay (beepTime);

    /* desable counter 2 */
    sysOutByte (DIAG_CTRL, sysInByte (DIAG_CTRL) & ~0x03);

    intUnlock (oldlevel);
    return;
}

/******************************************************************************
*
* dispHrdInit - initialize the Display
*
* This function is called externally to initialize the disp display
*
* PARAMETER: N/A
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispHrdInit (void) {
    /* get cursor shape and mode */

    sysOutByte ((int) CTRL_SEL_REG, 0x0a);
    curSt = sysInByte ((int) CTRL_VAL_REG);
    sysOutByte ((int) CTRL_SEL_REG, 0x0b);
    curEd = sysInByte ((int) CTRL_VAL_REG);

    dispStatInit ();

    /* clear screen and actPosition cursor at 0,0 */
    dispClear (&dispConDv, 2,' ');
    dispCursorOn ();
    return;
} 

/*******************************************************************************
*
* dispStatInit - initialize the Display state
*
* PARAMETER: N/A
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispStatInit (void)
    {
    FAST DISP_CON_DEV * pDispConDv;	/* pointer to hold disp descriptor */

	pDispConDv = &dispConDv;

	/* Display status initialization */
	pDispConDv->memBase	      = CTRL_MEM_BASE;
	pDispConDv->colorMode     = COLOR_MODE;
	pDispConDv->sv_col        = pDispConDv->col   = 0;
	pDispConDv->sv_row        = pDispConDv->row   = 0;
	pDispConDv->sv_curAttrib  = pDispConDv->curAttrib = DEFAULT_ATR;
	pDispConDv->defAttrib     = DEFAULT_ATR;
	pDispConDv->curChrPos     = pDispConDv->memBase;  /* current  actPosition */
	pDispConDv->sv_rev        = pDispConDv->rev   = FALSE;
	pDispConDv->ncol          = 80;                /* Number of columns */
	pDispConDv->nrow          = 25;                /* Number of text rows */
	pDispConDv->scst          = 0;                 /* scroll start */
	pDispConDv->sced          = 24;                /* scroll end */
	pDispConDv->autoWrap      = TRUE;              /* auto Wrap mode */
	pDispConDv->scrollCheck   = FALSE;             /* scroll  flag off */
	pDispConDv->charSet       = dispCharTable[0];  /* character set */
	pDispConDv->dispMode      = TEXT_MODE;         /* video as text mode  */
	pDispConDv->insMode       = INSERT_MODE_OFF;   /* insert mode */
	pDispConDv->escFlags      = ESC_NORMAL;        /* normal character */
	pDispConDv->escParaCount  = 0;                 /* zero parameters */
	pDispConDv->escQuestion   = FALSE;             /* ? flag set to false */
	bzero ((char *)pDispConDv->escPara, sizeof(pDispConDv->escPara));
	bzero (pDispConDv->tab_stop, sizeof(pDispConDv->tab_stop));
	pDispConDv->tab_stop [ 0] = 1;
	pDispConDv->tab_stop [ 8] = 1;
	pDispConDv->tab_stop [16] = 1;
	pDispConDv->tab_stop [24] = 1;
	pDispConDv->tab_stop [32] = 1;
	pDispConDv->tab_stop [40] = 1;
	pDispConDv->tab_stop [48] = 1;
	pDispConDv->tab_stop [56] = 1;
	pDispConDv->tab_stop [64] = 1;
	pDispConDv->tab_stop [72] = 1;
    return;
} 

/******************************************************************************
*
* dispClear - Clear Screen
*
* This routine clears the screen depending on the value of actPosition.
* If actPosition is 2 then it clears the whole screen and moves the cursor to 
* location 0,0. If actPosition is 1 then the screen is erased from the cursor to
* until the end of display. If actPosition is equal to any other value then 
* the screen is erased until the cursor.
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* int actPosition: cursor actPosition  0 or 1 or 2
* UCHAR eraseChar: erase character eg space is  0x20
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispClear (FAST DISP_CON_DEV * pDispConDv, int actPosition, UCHAR eraseChar) {
    FAST UINT16 *	cp;	/* hold the beginning actPosition */
    FAST UINT16 *	end;	/* hold the end actPosition */
    FAST UINT16 	erase;	/* erase character with attribute */

    erase = (pDispConDv->defAttrib << 8) + eraseChar;
    if ( actPosition == 2 ) { 
		cp  = (UINT16 *) pDispConDv->memBase;
		end = (UINT16 *) (pDispConDv->memBase + 2048 * CHR);
		pDispConDv->col = pDispConDv->row = 0;
		pDispConDv->curChrPos = (UCHAR *) pDispConDv->memBase;
	}
    else if ( actPosition == 0 ) {
		cp  = (UINT16 *)pDispConDv->curChrPos;
		end = (UINT16 *)(pDispConDv->memBase + 2048 * CHR);
	}
    else {
		cp  = (UINT16 *) pDispConDv->memBase;
		end = (UINT16 *) (pDispConDv->curChrPos + CHR);
	}
    for (; cp < end; cp++ ) {
        *cp     = erase;  
    }
}

/******************************************************************************
*
* dispScreenRev - Reverse Screen
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispScreenRev (FAST DISP_CON_DEV * pDispConDv) {
    UCHAR *	cp;	/* to hold the current pointer */
    UCHAR	atr;	/* to hold the attribute character */

    for (cp = pDispConDv->memBase; cp < pDispConDv->memBase + 2000 * CHR; cp += CHR) {
        atr = *(cp+1);
        *(cp+1)  = atr & INT_BLINK_MASK;
        *(cp+1) |= (atr << 4) & BG_ATTR_MASK;
        *(cp+1) |= (atr >> 4) & FG_ATTR_MASK;
    }
}

/*******************************************************************************
*
* dispScroll - Scroll Screen
*
* This function scrolls the screen according to the scroll direction.
* scroll direction FORWARD or BACKWARD 
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* int pos: scroll start line actPosition: 0 = first line
* int lines: scroll line count
* BOOL upDn: scroll direction: TRUE: forward = up, FALSE: backward = down
* UCHAR atr: atrribute for char: 0 = none
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispScroll (FAST DISP_CON_DEV * pDispConDv, int pos, int lines, 
                 BOOL upDn, FAST UCHAR atr) {
    FAST UCHAR *	dest;	/* to hold the destination pointer */
    FAST UCHAR *	src;	/* to hold the source pointer */

    if (pos < pDispConDv->scst || pos > pDispConDv->sced) {
        return;
	}
    if (upDn) {
	/* scroll direction is forward */

        if (pDispConDv->scst + lines > pDispConDv->sced + 1) {
            lines = pDispConDv->sced - pDispConDv->scst + 1;
		}
        for (dest = pDispConDv->memBase + pDispConDv->ncol * CHR * 
	     pDispConDv->scst, src = pDispConDv->memBase + pDispConDv->ncol * 
	     CHR * (pDispConDv->scst + lines); src < pDispConDv->memBase + 
	     pDispConDv->ncol * CHR * (pos + 1); *dest++ = *src++ ) {
		}
        for (dest = pDispConDv->memBase + pDispConDv->ncol * CHR * 
	     (pos - (lines - 1)); dest < pDispConDv->memBase + pDispConDv->ncol *
	     CHR * (pos + 1); dest += CHR ) {
            *dest     = ' ';
            *(dest+1) = atr;
        }
    }
    else {
	/* scroll direction is backward */

        if (pDispConDv->scst + lines > pDispConDv->sced + 1) {
            lines = pDispConDv->sced - pDispConDv->scst + 1;
		}
        for (dest = pDispConDv->memBase + pDispConDv->ncol * CHR * 
	     (pDispConDv->sced + 1) - 1, src = pDispConDv->memBase + 
	     pDispConDv->ncol * CHR * (pDispConDv->sced - (lines - 1)) - 1;
             src > pDispConDv->memBase + pDispConDv->ncol * CHR * pos - 1;
             *dest-- = *src-- ) {
		}
        for (dest = pDispConDv->memBase + pDispConDv->ncol * CHR * 
	     (pos + lines) - 1; dest > pDispConDv->memBase + pDispConDv->ncol * 
	     CHR * pos - 1; dest -= CHR ) {
            *dest     = atr;
            *(dest-1) = ' ';
        }
    }
    return;
}

/*****************************************************************************
*
* dispDelLeftChar - delete the character to the left side of the cursor
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispDelLeftChar (FAST DISP_CON_DEV * pDispConDv) {
    FAST UINT16		erase;	/* erase character with attributes */

    erase = (pDispConDv->defAttrib << 8) + ' ' ;
    if (pDispConDv->autoWrap || pDispConDv->ncol > 0) {
		pDispConDv->col--;
		pDispConDv->curChrPos -= CHR;
	}
    *(UINT16 *)pDispConDv->curChrPos = erase;
    return;
}

/*****************************************************************************
*
* dispCarriageReturn - do a carriage return on the display
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispCarriageReturn (FAST DISP_CON_DEV * pDispConDv) {

    pDispConDv->curChrPos -= pDispConDv->col * CHR;
    pDispConDv->col = 0;

    return;
}

/*****************************************************************************
*
* dispBackSpace - do a back space on the display
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispBackSpace (FAST DISP_CON_DEV * pDispConDv) {

    if (pDispConDv->autoWrap || pDispConDv->ncol > 0) {
		pDispConDv->col--;
		pDispConDv->curChrPos -= CHR;
	}
    if (pDispConDv->col < 0) {
		pDispConDv->col = pDispConDv->ncol - 1;
		pDispConDv->row--;
		pDispConDv->scrollCheck = TRUE;
	}
    return;
}

/*****************************************************************************
*
* dispTab - do a tab on the display
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispTab (FAST DISP_CON_DEV * pDispConDv) {
    int			ix;

    for (ix = pDispConDv->col + 1; ix < 80; ix++) {
		if (pDispConDv->tab_stop [ix]) {
			pDispConDv->col = ix;
			break;
	    }
	}
    if (pDispConDv->autoWrap && ix >= 80) {
		pDispConDv->col = 0;
		pDispConDv->row++;
		pDispConDv->scrollCheck = TRUE;
	}
    pDispConDv->curChrPos = ( pDispConDv->memBase + 
			    pDispConDv->row * pDispConDv->ncol * CHR + 
			    pDispConDv->col * CHR) ;
    return;
}

/******************************************************************************
*
* dispLineFeed - do a line feed on the display
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispLineFeed (FAST DISP_CON_DEV * pDispConDv) {

    pDispConDv->curChrPos += pDispConDv->ncol * CHR;
    pDispConDv->row++;
    pDispConDv->scrollCheck = TRUE;

    return;
}

/******************************************************************************
*
* dispCursorPos - Put the cursor at a specified location
*
* PARAMETER: 
* UINT16 pos: actPosition of the cursor: 
* pos = column + 80 * row
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispCursorPos (FAST UINT16 pos) {

    sysOutByte ((int) CTRL_SEL_REG, 0x0e);
    sysOutByte ((int) CTRL_VAL_REG, (pos >> 8) & 0xff);
    sysOutByte ((int) CTRL_SEL_REG, 0x0f);
    sysOutByte ((int) CTRL_VAL_REG, pos & 0xff);
    return;
}

/******************************************************************************
*
* dispCursorOn - switch the cursor on
*
* PARAMETER: N/A
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispCursorOn (void) {
    sysOutByte ((int) CTRL_SEL_REG, 0x0a);
    sysOutByte ((int) CTRL_VAL_REG, curSt & ~0x20);
    sysOutByte ((int) CTRL_SEL_REG, 0x0b);
    sysOutByte ((int) CTRL_VAL_REG, curEd);
    return;
}

/******************************************************************************
*
* dispCursorOff - swith the cursor off
*
* PARAMETER: N/A
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispCursorOff (void) {
    sysOutByte ((int) CTRL_SEL_REG, 0x0a);
    sysOutByte ((int) CTRL_VAL_REG, 0x20);
    sysOutByte ((int) CTRL_SEL_REG, 0x0b);
    sysOutByte ((int) CTRL_VAL_REG, 0x00);
    return;
}

/******************************************************************************
*
* dispCharPos - Put the character pointer at a specified location
*
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* UINT16 pos: actPosition of the cursor: 
* pos = column + 80 * row
* 
* RETURNS: N/A
* 
* HP-Check: OK
*/

void dispCharPos (FAST DISP_CON_DEV * pDispConDv, FAST UINT16 pos) {

    pDispConDv->curChrPos = pDispConDv->memBase;
    pDispConDv->curChrPos += (pos * CHR);
    pDispConDv->col = pos % 80;
    pDispConDv->row = pos / 80;
	dispCursorPos ((pDispConDv->curChrPos - pDispConDv->memBase) / CHR );

    return;
}

/*******************************************************************************
*
* dispWriteString - Write Character string  to Display
* 
* This function does the write to the disp routine. This routine is provided as
* transmitter startup routine when tyDevInit is called.
* 
* PARAMETER: 
* DISP_CON_DEV *pDispConDv: pointer to the disp descriptor
* char * text: zero terminated text string
*
* RETURNS: number of bytes written to the screen
*
* HP-Check: OK
*/

int dispWriteString (FAST DISP_CON_DEV * pDispConDv, char * text) {
    char		ch;
    FAST int		nBytes;
    FAST UCHAR		atr;
    
    atr = pDispConDv->curAttrib;
    nBytes = 0;
    
    while ((ch = text[nBytes])!= 0) {
	nBytes++;		/* increment the number of bytes */

	/* If character is normal and printable write it */
	
	if ( (pDispConDv->escFlags == ESC_NORMAL) && (pDispConDv->charSet [ch] != 0)) {
	    *(UINT16 *)pDispConDv->curChrPos = (atr << 8) + pDispConDv->charSet [ch];

	    if (pDispConDv->col == pDispConDv->ncol - 1) {
			if (pDispConDv->autoWrap) {
				dispCarriageReturn (pDispConDv);	/* time to wrap */
				dispLineFeed (pDispConDv);
		    }
		}
	    else {
			pDispConDv->col++;
			pDispConDv->curChrPos += CHR;
			continue;
		}
    }

	switch (ch) {
	    case 0x07:		/* BEL */
	        dispConBeep (FALSE);
			continue;
	    
	    case 0x08:		/* Back Space */
			dispBackSpace (pDispConDv);
			continue;

        case '\t':		/* TAB code */
			dispTab (pDispConDv);
			continue;

/*        case '\n':		*//* LF code *//*
		    dispCarriageReturn (pDispConDv);
			dispLineFeed (pDispConDv);
*/
        case 0x0b:		/* VT code */
			dispLineFeed (pDispConDv);

        case 0x0c:		/* Clear Screen code */
			dispClear (pDispConDv, 2, ' ');
			continue;

	    case 0x0d:		/* CR code */
			dispCarriageReturn (pDispConDv);
			continue;

        case 0x0f:		/* no more needed */
			continue;

	    case 0x7f:		/* special character for del */
			dispDelLeftChar (pDispConDv);
			continue;
	  }
	}

    dispCursorPos ((pDispConDv->curChrPos - pDispConDv->memBase) / CHR );

    return nBytes;	/* return the number of characters processed */
}
