/*
 * xoptions.c -- Move list window, part of X front end for XBoard
 *
 * Copyright 2000, 2009, 2010, 2011 Free Software Foundation, Inc.
 * ------------------------------------------------------------------------
 *
 * GNU XBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * GNU XBoard is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.  *
 *
 *------------------------------------------------------------------------
 ** See the file ChangeLog for a revision history.  */

// [HGM] this file is the counterpart of woptions.c, containing xboard popup menus
// similar to those of WinBoard, to set the most common options interactively.

#include "config.h"

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>

#if STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#else /* not STDC_HEADERS */
extern char *getenv();
# if HAVE_STRING_H
#  include <string.h>
# else /* not HAVE_STRING_H */
#  include <strings.h>
# endif /* not HAVE_STRING_H */
#endif /* not STDC_HEADERS */

#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <stdint.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/cursorfont.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Toggle.h>

#include "common.h"
#include "backend.h"
#include "xboard.h"
#include "gettext.h"

#ifdef ENABLE_NLS
# define  _(s) gettext (s)
# define N_(s) gettext_noop (s)
#else
# define  _(s) (s)
# define N_(s)  s
#endif

extern void SendToProgram P((char *message, ChessProgramState *cps));
FILE * XsraSelFile P((Widget w, char *prompt, char *ok, char *cancel, char *failed,
		char *init_path, char *mode, int (*show_entry)(), char **name_return));

extern Widget formWidget, shellWidget, boardWidget, menuBarWidget;
extern Display *xDisplay;
extern int squareSize;
extern Pixmap xMarkPixmap;
extern char *layoutName;
extern Window xBoardWindow;
extern Arg layoutArgs[2], formArgs[2];
Pixel timerForegroundPixel, timerBackgroundPixel;
extern int searchTime;
extern int lineGap;

// [HGM] the following code for makng menu popups was cloned from the FileNamePopUp routines

static Widget previous = NULL;

void SetFocus(Widget w, XtPointer data, XEvent *event, Boolean *b)
{
    Arg args[2];
    char *s;

    if(previous) {
	XtSetArg(args[0], XtNdisplayCaret, False);
	XtSetValues(previous, args, 1);
    }
    XtSetArg(args[0], XtNstring, &s);
    XtGetValues(w, args, 1);
    XtSetArg(args[0], XtNdisplayCaret, True);
    XtSetArg(args[1], XtNinsertPosition, strlen(s));
    XtSetValues(w, args, 2);
    XtSetKeyboardFocus((Widget) data, w);
    previous = w;
}

//--------------------------- New Shuffle Game --------------------------------------------
extern int shuffleOpenings;
extern int startedFromPositionFile;
int shuffleUp;
Widget shuffleShell;

void ShufflePopDown()
{
    if (!shuffleUp) return;
    XtPopdown(shuffleShell);
    XtDestroyWidget(shuffleShell);
    shuffleUp = False;
    ModeHighlight();
}

void ShuffleCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name;
    Widget w2;
    Arg args[16];
    char buf[MSG_SIZ];

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("cancel")) == 0) {
        ShufflePopDown();
        return;
    }
    if (strcmp(name, _("off")) == 0) {
        ShufflePopDown();
	shuffleOpenings = False; // [HGM] should be moved to New Variant menu, once we have it!
	ResetGameEvent();
        return;
    }
    if (strcmp(name, _("random")) == 0) {
      snprintf(buf, MSG_SIZ,  "%d", rand());
	XtSetArg(args[0],XtNvalue, buf); // erase bad (non-numeric) value
	XtSetValues(XtParent(w), args, 1);
        return;
    }
    if (strcmp(name, _("ok")) == 0) {
	int nr; String name;
        name = XawDialogGetValueString(w2 = XtParent(w));
	if(sscanf(name ,"%d",&nr) != 1) {
	  snprintf(buf, MSG_SIZ,  "%d", appData.defaultFrcPosition);
	    XtSetArg(args[0],XtNvalue, buf); // erase bad (non-numeric) value
	    XtSetValues(w2, args, 1);
	    return;
	}
	appData.defaultFrcPosition = nr;
	shuffleOpenings = True;
        ShufflePopDown();
	ResetGameEvent();
        return;
    }
}

void ShufflePopUp()
{
    Arg args[16];
    Widget popup, layout, dialog, edit;
    Window root, child;
    int x, y, i;
    int win_x, win_y;
    unsigned int mask;
    char def[MSG_SIZ];

    i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
    XtSetArg(args[i], XtNwidth, DIALOG_SIZE); i++;
    shuffleShell = popup =
      XtCreatePopupShell(_("New Shuffle Game"), transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));

    snprintf(def, MSG_SIZ,  "%d\n", appData.defaultFrcPosition);
    i = 0;
    XtSetArg(args[i], XtNlabel, _("Start-position number:")); i++;
    XtSetArg(args[i], XtNvalue, def); i++;
    XtSetArg(args[i], XtNborderWidth, 0); i++;
    dialog = XtCreateManagedWidget(_("Shuffle"), dialogWidgetClass,
				   layout, args, i);

//    XtSetArg(args[0], XtNeditType, XawtextEdit);  // [HGM] can't get edit to work decently
//    XtSetArg(args[1], XtNuseStringInPlace, False);
//    XtSetValues(dialog, args, 2);

    XawDialogAddButton(dialog, _("ok"), ShuffleCallback, (XtPointer) dialog);
    XawDialogAddButton(dialog, _("cancel"), ShuffleCallback, (XtPointer) dialog);
    XawDialogAddButton(dialog, _("random"), ShuffleCallback, (XtPointer) dialog);
    XawDialogAddButton(dialog, _("off"), ShuffleCallback, (XtPointer) dialog);

    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "ShufflePopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    shuffleUp = True;

    edit = XtNameToWidget(dialog, "*value");

    XtSetKeyboardFocus(popup, edit);
}

void ShuffleMenuProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
//    if (gameMode == AnalyzeMode || gameMode == AnalyzeFile) {
//	Reset(FALSE, TRUE);
//    }
    ShufflePopUp();
}

//--------------------------- Time-Control Menu Popup ----------------------------------
int TimeControlUp;
Widget TimeControlShell;
int tcInc;
Widget tcMess1, tcMess2, tcData, tcTime, tcOdds1, tcOdds2;
int tcIncrement, tcMoves;

void TimeControlPopDown()
{
    if (!TimeControlUp) return;
    previous = NULL;
    XtPopdown(TimeControlShell);
    XtDestroyWidget(TimeControlShell);
    TimeControlUp = False;
    ModeHighlight();
}

void TimeControlCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name, txt;
    Arg args[16];
    char buf[MSG_SIZ];
    int j;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("classical")) == 0) {
	if(tcInc == 0) return;
	j=0;
	XtSetArg(args[j], XtNlabel, _("minutes for each")); j++;
	XtSetValues(tcMess1, args, j);
	j=0;
	XtSetArg(args[j], XtNlabel, _("moves")); j++;
	XtSetValues(tcMess2, args, j);
	if(tcInc == 1) {
	    j=0;
	    XtSetArg(args[j], XtNstring, &name); j++;
	    XtGetValues(tcData, args, j);
	    tcIncrement = 0; sscanf(name, "%d", &tcIncrement);
	}
	snprintf(buf, MSG_SIZ,  "%d", tcMoves);
	j=0;
	XtSetArg(args[j], XtNstring, buf); j++;
	XtSetValues(tcData, args, j);
	tcInc = 0;
        return;
    }
    if (strcmp(name, _("incremental")) == 0) {
	if(tcInc == 1) return;
	j=0;
	XtSetArg(args[j], XtNlabel, _("minutes, plus")); j++;
	XtSetValues(tcMess1, args, j);
	j=0;
	XtSetArg(args[j], XtNlabel, _("sec/move")); j++;
	XtSetValues(tcMess2, args, j);
	if(tcInc == 0) {
	    j=0;
	    XtSetArg(args[j], XtNstring, &name); j++;
	    XtGetValues(tcData, args, j);
	    tcMoves = appData.movesPerSession; sscanf(name, "%d", &tcMoves);
	}
	snprintf(buf, MSG_SIZ,  "%d", tcIncrement);
	j=0;
	XtSetArg(args[j], XtNstring, buf); j++;
	XtSetValues(tcData, args, j);
	tcInc = 1;
        return;
    }
    if (strcmp(name, _("fixed time")) == 0) {
	if(tcInc == 2) return;
	j=0;
	XtSetArg(args[j], XtNlabel, _("sec/move (max)")); j++;
	XtSetValues(tcMess1, args, j);
	j=0;
	XtSetArg(args[j], XtNlabel, _("")); j++;
	XtSetValues(tcMess2, args, j);
	j=0;
	XtSetArg(args[j], XtNstring, ""); j++;
	XtSetValues(tcData, args, j);
	tcInc = 2;
        return;
    }
    if (strcmp(name, _(" OK ")) == 0) {
	int inc, mps, ok;
	XtSetArg(args[0], XtNstring, &txt);
	XtGetValues(tcData, args, 1);
	switch(tcInc) {
	  case 1:
	    ok = sscanf(txt, "%d", &inc); mps = 0;
	    if(!ok && txt[0] == 0) { inc = 0; ok = 1; } // accept empty string as zero
	    ok &= (inc >= 0);
	    break;
	  case 0:
	    ok = sscanf(txt, "%d", &mps); inc = -1;
	    ok &= (mps > 0);
	    break;
	  case 2:
	    ok = 1; inc = -1; mps = 40;
	}
	if(ok != 1) {
	    XtSetArg(args[0], XtNstring, ""); // erase any offending input
	    XtSetValues(tcData, args, 1);
	    return;
	}
	XtSetArg(args[0], XtNstring, &txt);
	XtGetValues(tcTime, args, 1);
	if(tcInc == 2) {
	    if(sscanf(txt, "%d", &inc) != 1) {
		XtSetArg(args[0], XtNstring, ""); // erase any offending input
		XtSetValues(tcTime, args, 1);
		DisplayError(_("Bad Time-Control String"), 0);
		return;
	    }
	    searchTime = inc;
	} else {
	    if(!ParseTimeControl(txt, inc, mps)) {
		XtSetArg(args[0], XtNstring, ""); // erase any offending input
		XtSetValues(tcTime, args, 1);
		DisplayError(_("Bad Time-Control String"), 0);
		return;
	    }
	    searchTime = 0;
	    appData.movesPerSession = mps;
	    appData.timeIncrement = inc;
	    appData.timeControl = strdup(txt);
	}
	XtSetArg(args[0], XtNstring, &txt);
	XtGetValues(tcOdds1, args, 1);
	appData.firstTimeOdds = first.timeOdds
		= (sscanf(txt, "%d", &j) == 1 && j > 0) ? j : 1;
	XtGetValues(tcOdds2, args, 1);
	appData.secondTimeOdds = second.timeOdds
		= (sscanf(txt, "%d", &j) == 1 && j > 0) ? j : 1;

	Reset(True, True);
        TimeControlPopDown();
        return;
    }
}

void TimeControlPopUp()
{
    Arg args[16];
    Widget popup, layout, form,  b_ok, b_cancel, b_clas, b_inc, mess;
    Window root, child;
    int x, y, i, j;
    int win_x, win_y;
    unsigned int mask;
    char def[MSG_SIZ];

    tcInc = searchTime > 0 ? 2 : (appData.timeIncrement >= 0);
    tcMoves = appData.movesPerSession; tcIncrement = appData.timeIncrement;
    if(!tcInc) tcIncrement = 0;
    snprintf(def, MSG_SIZ,  "%d", tcInc ? tcIncrement : tcMoves);

    i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
//    XtSetArg(args[i], XtNwidth, DIALOG_SIZE); i++;
    TimeControlShell = popup =
      XtCreatePopupShell(_("TimeControl Menu"), transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));

    form =
      XtCreateManagedWidget(layoutName, formWidgetClass, layout,
			    formArgs, XtNumber(formArgs));

    j = 0;
//    XtSetArg(args[j], XtNwidth,     (XtArgVal) 300); j++;
//    XtSetArg(args[j], XtNheight,    (XtArgVal) 85); j++;
    XtSetValues(popup, args, j);

    j= 0;
    XtSetArg(args[j], XtNborderWidth, 1); j++;
    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
    XtSetArg(args[j], XtNstring, appData.timeControl);  j++;
    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    XtSetArg(args[j], XtNresizable, True);  j++;
    XtSetArg(args[j], XtNwidth,  85);  j++;
    XtSetArg(args[j], XtNinsertPosition, 9999);  j++;
    tcTime = XtCreateManagedWidget("TC", asciiTextWidgetClass, form, args, j);
    XtAddEventHandler(tcTime, ButtonPressMask, False, SetFocus, (XtPointer) popup);

    j= 0;
    XtSetArg(args[j], XtNlabel, tcInc ? tcInc == 2 ? _("sec/move (max)   ") : _("   minutes, plus   ") : _("minutes for each")); j++;
    XtSetArg(args[j], XtNborderWidth, 0); j++;
    XtSetArg(args[j], XtNfromHoriz, tcTime); j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
  //  XtSetArg(args[j], XtNwidth,  100);  j++;
  //  XtSetArg(args[j], XtNheight, 20);  j++;
    tcMess1 = XtCreateManagedWidget("TCtext", labelWidgetClass, form, args, j);

    j= 0;
    XtSetArg(args[j], XtNborderWidth, 1); j++;
    XtSetArg(args[j], XtNfromHoriz, tcMess1); j++;
    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
    XtSetArg(args[j], XtNstring, def);  j++;
    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    XtSetArg(args[j], XtNresizable, True);  j++;
    XtSetArg(args[j], XtNwidth,  40);  j++;
//    XtSetArg(args[j], XtNheight, 20);  j++;
    tcData = XtCreateManagedWidget("MPS", asciiTextWidgetClass, form, args, j);
    XtAddEventHandler(tcData, ButtonPressMask, False, SetFocus, (XtPointer) popup);

    j= 0;
    XtSetArg(args[j], XtNlabel, tcInc ? tcInc == 2 ? _("             ") : _("sec/move") : _("moves     ")); j++;
    XtSetArg(args[j], XtNjustify, XtJustifyLeft); j++;
    XtSetArg(args[j], XtNborderWidth, 0); j++;
    XtSetArg(args[j], XtNfromHoriz, tcData); j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
//    XtSetArg(args[j], XtNwidth,  80);  j++;
//    XtSetArg(args[j], XtNheight, 20);  j++;
    tcMess2 = XtCreateManagedWidget("MPStext", labelWidgetClass,
				   form, args, j);

    j= 0;
    XtSetArg(args[j], XtNborderWidth, 1); j++;
    XtSetArg(args[j], XtNfromVert, tcTime); j++;
    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
    XtSetArg(args[j], XtNstring, "1");  j++;
    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    XtSetArg(args[j], XtNresizable, True);  j++;
    XtSetArg(args[j], XtNwidth,  40);  j++;
//    XtSetArg(args[j], XtNheight, 20);  j++;
    tcOdds1 = XtCreateManagedWidget("Odds1", asciiTextWidgetClass, form, args, j);
    XtAddEventHandler(tcOdds1, ButtonPressMask, False, SetFocus, (XtPointer) popup);

    j= 0;
    XtSetArg(args[j], XtNborderWidth, 1); j++;
    XtSetArg(args[j], XtNfromVert, tcTime); j++;
    XtSetArg(args[j], XtNfromHoriz, tcOdds1); j++;
    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
    XtSetArg(args[j], XtNstring, "1");  j++;
    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    XtSetArg(args[j], XtNresizable, True);  j++;
    XtSetArg(args[j], XtNwidth,  40);  j++;
//    XtSetArg(args[j], XtNheight, 20);  j++;
    tcOdds2 = XtCreateManagedWidget("Odds2", asciiTextWidgetClass, form, args, j);
    XtAddEventHandler(tcOdds2, ButtonPressMask, False, SetFocus, (XtPointer) popup);

    j= 0;
    XtSetArg(args[j], XtNlabel, _("Engine #1 and #2 Time-Odds Factors")); j++;
    XtSetArg(args[j], XtNjustify, XtJustifyLeft); j++;
    XtSetArg(args[j], XtNborderWidth, 0); j++;
    XtSetArg(args[j], XtNfromVert, tcTime); j++;
    XtSetArg(args[j], XtNfromHoriz, tcOdds2); j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
//    XtSetArg(args[j], XtNwidth,  200);  j++;
//    XtSetArg(args[j], XtNheight, 20);  j++;
    mess = XtCreateManagedWidget("Oddstext", labelWidgetClass,
				   form, args, j);
    j=0;
    XtSetArg(args[j], XtNfromVert, tcOdds1);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    XtSetArg(args[j], XtNstate, tcInc==0); j++;
    b_clas= XtCreateManagedWidget(_("classical"), toggleWidgetClass,
				   form, args, j);
    XtAddCallback(b_clas, XtNcallback, TimeControlCallback, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNradioGroup, b_clas); j++;
    XtSetArg(args[j], XtNfromVert, tcOdds1);  j++;
    XtSetArg(args[j], XtNfromHoriz, b_clas);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    XtSetArg(args[j], XtNstate, tcInc==1); j++;
    b_inc = XtCreateManagedWidget(_("incremental"), toggleWidgetClass,
				   form, args, j);
    XtAddCallback(b_inc, XtNcallback, TimeControlCallback, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNradioGroup, b_inc); j++;
    XtSetArg(args[j], XtNfromVert, tcOdds1);  j++;
    XtSetArg(args[j], XtNfromHoriz, b_inc);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    XtSetArg(args[j], XtNstate, tcInc==2); j++;
    b_inc = XtCreateManagedWidget(_("fixed time"), toggleWidgetClass,
				   form, args, j);
    XtAddCallback(b_inc, XtNcallback, TimeControlCallback, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNfromVert, tcOdds1);  j++;
    XtSetArg(args[j], XtNfromHoriz, tcData);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    b_ok= XtCreateManagedWidget(_(" OK "), commandWidgetClass,
				   form, args, j);
    XtAddCallback(b_ok, XtNcallback, TimeControlCallback, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNfromVert, tcOdds1);  j++;
    XtSetArg(args[j], XtNfromHoriz, b_ok);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    b_cancel= XtCreateManagedWidget(_("cancel"), commandWidgetClass,
				   form, args, j);
    XtAddCallback(b_cancel, XtNcallback, TimeControlPopDown, (XtPointer) 0);

    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "TimeControlPopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    TimeControlUp = True;

    previous = NULL;
    SetFocus(tcTime, popup, (XEvent*) NULL, False);
//    XtSetKeyboardFocus(popup, tcTime);
}

void TimeControlProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   TimeControlPopUp();
}

Widget w1, w2, w3, w4, w5, w6, w7, w8;

//--------------------------- New-Variant Menu PopUp -----------------------------------
struct NewVarButton {
  char   *name;
  char *color;
  Widget handle;
  VariantClass variant;
};

struct NewVarButton buttonDesc[] = {
    {N_("normal"),            "#FFFFFF", 0, VariantNormal},
    {N_("FRC"),               "#FFFFFF", 0, VariantFischeRandom},
    {N_("wild castle"),       "#FFFFFF", 0, VariantWildCastle},
    {N_("no castle"),         "#FFFFFF", 0, VariantNoCastle},
    {N_("knightmate"),        "#FFFFFF", 0, VariantKnightmate},
    {N_("berolina"),          "#FFFFFF", 0, VariantBerolina},
    {N_("cylinder"),          "#FFFFFF", 0, VariantCylinder},
    {N_("shatranj"),          "#FFFFFF", 0, VariantShatranj},
    {N_("makruk"),            "#FFFFFF", 0, VariantMakruk},
    {N_("atomic"),            "#FFFFFF", 0, VariantAtomic},
    {N_("two kings"),         "#FFFFFF", 0, VariantTwoKings},
    {N_("3-checks"),          "#FFFFFF", 0, Variant3Check},
    {N_("suicide"),           "#FFFFBF", 0, VariantSuicide},
    {N_("give-away"),         "#FFFFBF", 0, VariantGiveaway},
    {N_("losers"),            "#FFFFBF", 0, VariantLosers},
    {N_("fairy"),             "#BFBFBF", 0, VariantFairy},
    {N_("Seirawan"),          "#FFBFBF", 0, VariantSChess},
    {N_("Superchess"),        "#FFBFBF", 0, VariantSuper},
    {N_("crazyhouse"),        "#FFBFBF", 0, VariantCrazyhouse},
    {N_("bughouse"),          "#FFBFBF", 0, VariantBughouse},
    {N_("shogi (9x9)"),       "#BFFFFF", 0, VariantShogi},
    {N_("xiangqi (9x10)"),    "#BFFFFF", 0, VariantXiangqi},
    {N_("courier (12x8)"),    "#BFFFBF", 0, VariantCourier},
    {N_("Capablanca (10x8)"), "#BFBFFF", 0, VariantCapablanca},
#ifdef GOTHIC
    {N_("Gothic (10x8)"),     "#BFBFFF", 0, VariantGothic},
#endif
    {N_("janus (10x8)"),      "#BFBFFF", 0, VariantJanus},
    {N_("CRC (10x8)"),        "#BFBFFF", 0, VariantCapaRandom},
#ifdef FALCON
    {N_("Falcon (10x8)"),     "#BFBFFF", 0, VariantFalcon},
#endif
    {N_("Spartan"),           "#FF0000", 0, VariantSpartan},
    {NULL,                0, 0, (VariantClass) 0}
};

int NewVariantUp;
Widget NewVariantShell;

void NewVariantPopDown()
{
    if (!NewVariantUp) return;
    XtPopdown(NewVariantShell);
    XtDestroyWidget(NewVariantShell);
    NewVariantUp = False;
    ModeHighlight();
}

void NewVariantCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name;
    Arg args[16];
    VariantClass v;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("  OK  ")) == 0) {
	int nr = (intptr_t) XawToggleGetCurrent(buttonDesc[0].handle) - 1;
	if(nr < 0) return;
	v = buttonDesc[nr].variant;
	if(!appData.noChessProgram) {
	    char *name = VariantName(v), buf[MSG_SIZ];
	    if (first.protocolVersion > 1 && StrStr(first.variants, name) == NULL) {
		/* [HGM] in protocol 2 we check if variant is suported by engine */
	      snprintf(buf, MSG_SIZ,  _("Variant %s not supported by %s"), name, first.tidy);
		DisplayError(buf, 0);
//		NewVariantPopDown();
		return; /* ignore OK if first engine does not support it */
	    } else
	    if (second.initDone && second.protocolVersion > 1 && StrStr(second.variants, name) == NULL) {
	      snprintf(buf, MSG_SIZ,  _("Warning: second engine (%s) does not support this!"), second.tidy);
		DisplayError(buf, 0);   /* use of second engine is optional; only warn user */
	    }
	}

	gameInfo.variant = v;
	appData.variant = VariantName(v);

	shuffleOpenings = FALSE; /* [HGM] shuffle: possible shuffle reset when we switch */
	startedFromPositionFile = FALSE; /* [HGM] loadPos: no longer valid in new variant */
	appData.pieceToCharTable = NULL;
	Reset(True, True);
        NewVariantPopDown();
        return;
    }
}

void NewVariantPopUp()
{
    Arg args[16];
    Widget popup, layout, form, last = NULL, b_ok, b_cancel;
    Window root, child;
    int x, y, i, j;
    int win_x, win_y;
    unsigned int mask;
    XrmValue vFrom, vTo;

    i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
//    XtSetArg(args[i], XtNwidth, 250); i++;
//    XtSetArg(args[i], XtNheight, 300); i++;
    NewVariantShell = popup =
      XtCreatePopupShell(_("NewVariant Menu"), transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));

    form =
      XtCreateManagedWidget("form", formWidgetClass, layout,
			    formArgs, XtNumber(formArgs));

    for(i = 0; buttonDesc[i].name != NULL; i++) {
	Pixel buttonColor;
	if (!appData.monoMode) {
	    vFrom.addr = (caddr_t) buttonDesc[i].color;
	    vFrom.size = strlen(buttonDesc[i].color);
	    XtConvert(shellWidget, XtRString, &vFrom, XtRPixel, &vTo);
	    if (vTo.addr == NULL) {
	  	buttonColor = (Pixel) -1;
	    } else {
		buttonColor = *(Pixel *) vTo.addr;
	    }
	}

	j = 0;
	XtSetArg(args[j], XtNradioGroup, last); j++;
	XtSetArg(args[j], XtNwidth, 125); j++;
//	XtSetArg(args[j], XtNheight, 16); j++;
	XtSetArg(args[j], XtNfromVert, i == 15 ? NULL : last); j++;
	XtSetArg(args[j], XtNfromHoriz, i < 15 ? NULL : buttonDesc[i-15].handle); j++;
	XtSetArg(args[j], XtNradioData, i+1); j++;
	XtSetArg(args[j], XtNbackground, buttonColor); j++;
	XtSetArg(args[j], XtNstate, gameInfo.variant == buttonDesc[i].variant); j++;
	XtSetArg(args[j], XtNsensitive, appData.noChessProgram || strstr(first.variants, VariantName(buttonDesc[i].variant))); j++;
	buttonDesc[i].handle = last =
	    XtCreateManagedWidget(buttonDesc[i].name, toggleWidgetClass, form, args, j);
    }

    j=0;
    XtSetArg(args[j], XtNfromVert, buttonDesc[12].handle);  j++;
    XtSetArg(args[j], XtNfromHoriz, buttonDesc[12].handle);  j++;
    XtSetArg(args[j], XtNheight, 35); j++;
//    XtSetArg(args[j], XtNwidth, 60); j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    b_cancel= XtCreateManagedWidget(_("CANCEL"), commandWidgetClass, form, args, j);
    XtAddCallback(b_cancel, XtNcallback, NewVariantPopDown, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNfromHoriz, b_cancel);  j++;
    XtSetArg(args[j], XtNfromVert, buttonDesc[12].handle);  j++;
    XtSetArg(args[j], XtNheight, 35); j++;
//    XtSetArg(args[j], XtNwidth, 60); j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    b_ok= XtCreateManagedWidget(_("  OK  "), commandWidgetClass, form, args, j);
    XtAddCallback(b_ok, XtNcallback, NewVariantCallback, (XtPointer) 0);

    j=0;
    XtSetArg(args[j], XtNfromVert, buttonDesc[14].handle);  j++;
//    XtSetArg(args[j], XtNheight, 70); j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    XtSetArg(args[j], XtNlabel, _("WARNING: variants with un-orthodox\n"
				  "pieces only have built-in bitmaps\n"
				  "for -boardSize middling, bulky and\n"
				  "petite, and substitute king or amazon\n"
				  "for missing bitmaps. (See manual.)")); j++;
    XtCreateManagedWidget("warning", labelWidgetClass, form, args, j);

	    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "NewVariantPopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    NewVariantUp = True;
}

void NewVariantProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   NewVariantPopUp();
}

//--------------------------- UCI Menu Popup ------------------------------------------
int UciUp;
Widget UciShell;

struct UciControl {
  char *name;
  Widget handle;
  void *ptr;
};

struct UciControl controlDesc[] = {
  {N_("maximum nr of CPUs:"), 0, &appData.smpCores},
  {N_("Polyglot Directory:"), 0, &appData.polyglotDir},
  {N_("Hash Size (MB):"),     0, &appData.defaultHashSize},
  {N_("EGTB Path:"),          0, &appData.defaultPathEGTB},
  {N_("EGTB Cache (MB):"),    0, &appData.defaultCacheSizeEGTB},
  {N_("Polyglot Book:"),      0, &appData.polyglotBook},
  {NULL, 0, NULL},
};

void UciPopDown()
{
    if (!UciUp) return;
    previous = NULL;
    XtPopdown(UciShell);
    XtDestroyWidget(UciShell);
    UciUp = False;
    ModeHighlight();
}

void UciCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name;
    Arg args[16];
    int oldCores = appData.smpCores, ponder = 0;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("OK")) == 0) {
	int i, j; String name;
	for(i=0; i<6; i++) {
	    XtSetArg(args[0], XtNstring, &name);
	    XtGetValues(controlDesc[i].handle, args, 1);
	    if(i&1) {
		if(name)
		    *(char**) controlDesc[i].ptr = strdup(name);
	    } else {
		if(sscanf(name, "%d", &j) == 1)
		    *(int*) controlDesc[i].ptr = j;
	    }
	}
	XtSetArg(args[0], XtNstate, &appData.usePolyglotBook);
	XtGetValues(w1, args, 1);
	XtSetArg(args[0], XtNstate, &appData.firstHasOwnBookUCI);
	XtGetValues(w2, args, 1);
	XtSetArg(args[0], XtNstate, &appData.secondHasOwnBookUCI);
	XtGetValues(w3, args, 1);
	XtSetArg(args[0], XtNstate, &ponder);
	XtGetValues(w4, args, 1);

	// adjust setting in other menu for duplicates
	// (perhaps duplicates should be removed from general Option Menu?)
	XtSetArg(args[0], XtNleftBitmap, ponder ? xMarkPixmap : None);
	XtSetValues(XtNameToWidget(menuBarWidget,
				   "menuOptions.Ponder Next Move"), args, 1);

	// make sure changes are sent to first engine by re-initializing it
	// if it was already started pre-emptively at end of previous game
	if(gameMode == BeginningOfGame) Reset(True, True); else {
	    // Some changed setting need immediate sending always.
	    PonderNextMoveEvent(ponder);
	    if(oldCores != appData.smpCores)
		NewSettingEvent(False, &(first.maxCores), "cores", appData.smpCores);
      }
      UciPopDown();
      return;
    }
}

void UciPopUp()
{
    Arg args[16];
    Widget popup, layout, form, b_ok, b_cancel, last = NULL, new, upperLeft;
    Window root, child;
    int x, y, i, j;
    int win_x, win_y;
    unsigned int mask;
    char def[MSG_SIZ];

    i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
//    XtSetArg(args[i], XtNwidth, 300); i++;
    UciShell = popup =
      XtCreatePopupShell(_("Engine Settings"), transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));


    form =
      XtCreateManagedWidget("form", formWidgetClass, layout,
			    formArgs, XtNumber(formArgs));

    j = 0;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
//    XtSetArg(args[j], XtNheight, 20); j++;
    for(i = 0; controlDesc[i].name != NULL; i++) {
	j = 3;
	XtSetArg(args[j], XtNfromVert, last); j++;
//	XtSetArg(args[j], XtNwidth, 130); j++;
	XtSetArg(args[j], XtNjustify, XtJustifyLeft); j++;
	XtSetArg(args[j], XtNright, XtChainLeft);  j++;
	XtSetArg(args[j], XtNborderWidth, 0); j++;
	new = XtCreateManagedWidget(controlDesc[i].name, labelWidgetClass, form, args, j);
	if(i==0) upperLeft = new;

	j = 4;
	XtSetArg(args[j], XtNborderWidth, 1); j++;
	XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
	XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
	XtSetArg(args[j], XtNdisplayCaret, False);  j++;
	XtSetArg(args[j], XtNright, XtChainRight);  j++;
	XtSetArg(args[j], XtNresizable, True);  j++;
	XtSetArg(args[j], XtNwidth, i&1 ? 245 : 50); j++;
	XtSetArg(args[j], XtNinsertPosition, 9999);  j++;
	if(i&1) {
	    XtSetArg(args[j], XtNstring, * (char**) controlDesc[i].ptr ?
					 * (char**) controlDesc[i].ptr : ""); j++;
	} else {
	  snprintf(def, MSG_SIZ,  "%d", * (int*) controlDesc[i].ptr);
	    XtSetArg(args[j], XtNstring, def); j++;
	}
	XtSetArg(args[j], XtNfromHoriz, upperLeft); j++;
	controlDesc[i].handle = last =
	    XtCreateManagedWidget("text", asciiTextWidgetClass, form, args, j);
	XtAddEventHandler(last, ButtonPressMask, False, SetFocus, (XtPointer) popup);
    }

    j=0;
    XtSetArg(args[j], XtNfromHoriz, controlDesc[0].handle);  j++;
    XtSetArg(args[j], XtNbottom, XtChainTop);  j++;
    XtSetArg(args[j], XtNtop, XtChainTop);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    XtSetArg(args[j], XtNstate, appData.ponderNextMove);  j++;
    w4 = XtCreateManagedWidget(_("Ponder"), toggleWidgetClass, form, args, j);

    j=0;
    XtSetArg(args[j], XtNfromVert, last);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainLeft);  j++;
    XtSetArg(args[j], XtNright, XtChainLeft);  j++;
    b_ok = XtCreateManagedWidget(_("OK"), commandWidgetClass, form, args, j);
    XtAddCallback(b_ok, XtNcallback, UciCallback, (XtPointer) 0);

    XtSetArg(args[j], XtNfromHoriz, b_ok);  j++;
    b_cancel = XtCreateManagedWidget(_("cancel"), commandWidgetClass, form, args, j);
    XtAddCallback(b_cancel, XtNcallback, UciPopDown, (XtPointer) 0);

    j = 5;
    XtSetArg(args[j], XtNfromHoriz, upperLeft);  j++;
    XtSetArg(args[j], XtNstate, appData.usePolyglotBook);  j++;
    w1 = XtCreateManagedWidget(_(" use book "), toggleWidgetClass, form, args, j);
//    XtAddCallback(w1, XtNcallback, UciCallback, (XtPointer) 0);

    j = 5;
    XtSetArg(args[j], XtNfromHoriz, w1);  j++;
    XtSetArg(args[j], XtNstate, appData.firstHasOwnBookUCI);  j++;
    w2 = XtCreateManagedWidget(_("own book 1"), toggleWidgetClass, form, args, j);
//    XtAddCallback(w2, XtNcallback, UciCallback, (XtPointer) 0);

    j = 5;
    XtSetArg(args[j], XtNfromHoriz, w2);  j++;
    XtSetArg(args[j], XtNstate, appData.secondHasOwnBookUCI);  j++;
    w3 = XtCreateManagedWidget(_("own book 2"), toggleWidgetClass, form, args, j);
//    XtAddCallback(w3, XtNcallback, UciCallback, (XtPointer) 0);

    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "UciPopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    UciUp = True;

    previous = NULL;
    SetFocus(controlDesc[2].handle, popup, (XEvent*) NULL, False);
//    XtSetKeyboardFocus(popup, controlDesc[1].handle);
}

void UciMenuProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   UciPopUp();
}

//--------------------------- Engine-specific options menu ----------------------------------

int SettingsUp;
Widget SettingsShell;
int values[MAX_OPTIONS];
ChessProgramState *currentCps;

void SettingsPopDown()
{
    if (!SettingsUp) return;
    previous = NULL;
    XtPopdown(SettingsShell);
    XtDestroyWidget(SettingsShell);
    SettingsUp = False;
    ModeHighlight();
}

void SpinCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name, val;
    Arg args[16];
    char buf[MSG_SIZ], *p;
    int j;
    int data = (intptr_t) client_data;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    j = 0;
    XtSetArg(args[0], XtNstring, &val);
    XtGetValues(currentCps->option[data].handle, args, 1);
    sscanf(val, "%d", &j);
    if (strcmp(name, "browse") == 0) {
	if(XsraSelFile(SettingsShell, currentCps->option[data].name, NULL, NULL, "", "", 
			 	  currentCps->option[data].type == PathName ? "p" : "f", NULL, &p)) {
		int len = strlen(p);
		if(len && p[len-1] == '/') p[len-1] = NULLCHAR;
		XtSetArg(args[0], XtNstring, p);
		XtSetValues(currentCps->option[data].handle, args, 1);
	}
	SetFocus(currentCps->option[data].handle, SettingsShell, (XEvent*) NULL, False);
	return;
    } else
    if (strcmp(name, "+") == 0) {
	if(++j > currentCps->option[data].max) return;
    } else
    if (strcmp(name, "-") == 0) {
	if(--j < currentCps->option[data].min) return;
    } else return;
    snprintf(buf, MSG_SIZ,  "%d", j);
    XtSetArg(args[0], XtNstring, buf);
    XtSetValues(currentCps->option[data].handle, args, 1);
}

void SettingsCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name, val;
    Arg args[16];
    char buf[MSG_SIZ];
    int i, j;
    int data = (intptr_t) client_data;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("cancel")) == 0) {
        SettingsPopDown();
        return;
    }
    if (strcmp(name, _("OK")) == 0 || data) { // save buttons imply OK
	for(i=0; i<currentCps->nrOptions; i++) { // send all options that had to be OK-ed to engine
	    switch(currentCps->option[i].type) {
		case TextBox:
		    XtSetArg(args[0], XtNstring, &val);
		    XtGetValues(currentCps->option[i].handle, args, 1);
		    if(strcmp(currentCps->option[i].textValue, val)) {
		      safeStrCpy(currentCps->option[i].textValue, val, MSG_SIZ - (currentCps->option[i].textValue - currentCps->option[i].name) );
		      snprintf(buf, MSG_SIZ,  "option %s=%s\n", currentCps->option[i].name, val);
		      SendToProgram(buf, currentCps);
		    }
		    break;
		case Spin:
		    XtSetArg(args[0], XtNstring, &val);
		    XtGetValues(currentCps->option[i].handle, args, 1);
		    sscanf(val, "%d", &j);
		    if(j > currentCps->option[i].max) j = currentCps->option[i].max;
		    if(j < currentCps->option[i].min) j = currentCps->option[i].min;
		    if(currentCps->option[i].value != j) {
			currentCps->option[i].value = j;
			snprintf(buf, MSG_SIZ,  "option %s=%d\n", currentCps->option[i].name, j);
			SendToProgram(buf, currentCps);
		    }
		    break;
		case CheckBox:
		    j = 0;
		    XtSetArg(args[0], XtNstate, &j);
		    XtGetValues(currentCps->option[i].handle, args, 1);
		    if(currentCps->option[i].value != j) {
			currentCps->option[i].value = j;
			snprintf(buf, MSG_SIZ,  "option %s=%d\n", currentCps->option[i].name, j);
			SendToProgram(buf, currentCps);
		    }
		    break;
		case ComboBox:
		    if(currentCps->option[i].value != values[i]) {
			currentCps->option[i].value = values[i];
			snprintf(buf, MSG_SIZ,  "option %s=%s\n", currentCps->option[i].name,
				((char**)currentCps->option[i].textValue)[values[i]]);
			SendToProgram(buf, currentCps);
		    }
		    break;
	    default:
	      if( appData.debugMode )
		fprintf(debugFP, "SettingsPopUp: unexpected case in switch.\n");
	      break;
	    }
	}
	if(data) { // send save-button command to engine
	  snprintf(buf, MSG_SIZ,  "option %s\n", name);
	  SendToProgram(buf, currentCps);
	}
        SettingsPopDown();
        return;
    }
    snprintf(buf, MSG_SIZ,  "option %s\n", name);
    SendToProgram(buf, currentCps);
}

void ComboSelect(w, addr, index) // callback for all combo items
     Widget w;
     caddr_t addr;
     caddr_t index;
{
    Arg args[16];
    int i = ((intptr_t)addr)>>8;
    int j = 255 & (intptr_t) addr;

    values[i] = j; // store in temporary, for transfer at OK
    XtSetArg(args[0], XtNlabel, ((char**)currentCps->option[i].textValue)[j]);
    XtSetValues(currentCps->option[i].handle, args, 1);
}

void CreateComboPopup(parent, name, n, mb)
     Widget parent;
     String name;
     int n;
     char *mb[];
{
    int i=0, j;
    Widget menu, entry;
    Arg args[16];

    menu = XtCreatePopupShell(name, simpleMenuWidgetClass,
			      parent, NULL, 0);
    j = 0;
    XtSetArg(args[j], XtNwidth, 100);  j++;
//    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    while (mb[i] != NULL) {
	    entry = XtCreateManagedWidget(mb[i], smeBSBObjectClass,
					  menu, args, j);
	    XtAddCallback(entry, XtNcallback,
			  (XtCallbackProc) ComboSelect,
			  (caddr_t)(intptr_t) (256*n+i));
	i++;
    }
}

void
SettingsPopUp(ChessProgramState *cps)
{
    Arg args[16];
    Widget popup, layout, dialog, edit=NULL, form,  last, b_ok, b_cancel, leftMargin = NULL, textField = NULL;
    Window root, child;
    int x, y, i, j, height, width, h, c;
    int win_x, win_y, maxWidth, maxTextWidth;
    unsigned int mask;
    char def[MSG_SIZ];
    static char pane[6] = "paneX";
    Widget texts[100], forelast = NULL, anchor, widest;

    // to do: start up second engine if needed
    if(!cps->initDone || !cps->nrOptions) return; // nothing to be done
    currentCps = cps;

    if(cps->nrOptions > 50) width = 4; else if(cps->nrOptions>24) width = 2; else width = 1;
    height = cps->nrOptions / width + 1;
     i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
    SettingsShell = popup =
      XtCreatePopupShell(_("Settings Menu"), transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));
  for(c=0; c<width; c++) {
    pane[4] = 'A'+c;
    form =
      XtCreateManagedWidget(pane, formWidgetClass, layout,
			    formArgs, XtNumber(formArgs));
    j=0;
    XtSetArg(args[j], XtNfromHoriz, leftMargin);  j++;
    XtSetValues(form, args, j);
    leftMargin = form;

    last = widest = NULL; anchor = forelast;
    for(h=0; h<height; h++) {
	forelast = last;
	i = h + c*height;
        if(i >= cps->nrOptions) break;
	switch(cps->option[i].type) {
	  case Spin:
	    snprintf(def, MSG_SIZ,  "%d", cps->option[i].value);
	  case TextBox:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    texts[h] =
	    dialog = XtCreateManagedWidget(cps->option[i].name, labelWidgetClass, form, args, j);
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, dialog);  j++;
	    XtSetArg(args[j], XtNborderWidth, 1); j++;
	    XtSetArg(args[j], XtNwidth, cps->option[i].type == Spin ? 40 : 175); j++;
	    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
	    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
	    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
	    XtSetArg(args[j], XtNright, XtChainRight);  j++;
	    XtSetArg(args[j], XtNresizable, True);  j++;
	    XtSetArg(args[j], XtNstring, cps->option[i].type==Spin ? def : cps->option[i].textValue);  j++;
	    XtSetArg(args[j], XtNinsertPosition, 9999);  j++;
	    edit = last;
	    cps->option[i].handle = (void*)
		(textField = last = XtCreateManagedWidget("text", asciiTextWidgetClass, form, args, j));
	    XtAddEventHandler(last, ButtonPressMask, False, SetFocus, (XtPointer) popup);
	    if(cps->option[i].type == TextBox) break;

	    // add increment and decrement controls for spin
	    j=0;
	    XtSetArg(args[j], XtNfromVert, edit);  j++;
	    XtSetArg(args[j], XtNfromHoriz, last);  j++;
	    XtSetArg(args[j], XtNheight, 10);  j++;
	    XtSetArg(args[j], XtNwidth, 20);  j++;
	    edit = XtCreateManagedWidget("+", commandWidgetClass, form, args, j);
	    XtAddCallback(edit, XtNcallback, SpinCallback,
			  (XtPointer)(intptr_t) i);

	    j=0;
	    XtSetArg(args[j], XtNfromVert, edit);  j++;
	    XtSetArg(args[j], XtNfromHoriz, last);  j++;
	    XtSetArg(args[j], XtNheight, 10);  j++;
	    XtSetArg(args[j], XtNwidth, 20);  j++;
	    last = XtCreateManagedWidget("-", commandWidgetClass, form, args, j);
	    XtAddCallback(last, XtNcallback, SpinCallback,
			  (XtPointer)(intptr_t) i);
	    break;
	  case CheckBox:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNwidth, 10);  j++;
	    XtSetArg(args[j], XtNheight, 10);  j++;
	    XtSetArg(args[j], XtNstate, cps->option[i].value);  j++;
	    cps->option[i].handle = (void*)
		(dialog = XtCreateManagedWidget(" ", toggleWidgetClass, form, args, j));
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, dialog);  j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    last = XtCreateManagedWidget(cps->option[i].name, labelWidgetClass, form, args, j);
	    break;
	  case SaveButton:
	  case Button:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNstate, cps->option[i].value);  j++;
	    cps->option[i].handle = (void*)
		(dialog = last = XtCreateManagedWidget(cps->option[i].name, commandWidgetClass, form, args, j));
	    XtAddCallback(last, XtNcallback, SettingsCallback,
			  (XtPointer)(intptr_t) (cps->option[i].type == SaveButton));
	    break;
	  case ComboBox:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    dialog = XtCreateManagedWidget(cps->option[i].name, labelWidgetClass, form, args, j);

	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, dialog);  j++;
	    XtSetArg(args[j], XtNwidth, 100);  j++;
	    XtSetArg(args[j], XtNmenuName, XtNewString(cps->option[i].name));  j++;
	    XtSetArg(args[j], XtNlabel, ((char**)cps->option[i].textValue)[cps->option[i].value]);  j++;
	    cps->option[i].handle = (void*)
		(last = XtCreateManagedWidget(" ", menuButtonWidgetClass, form, args, j));
	    CreateComboPopup(last, cps->option[i].name, i, (char **) cps->option[i].textValue);
	    values[i] = cps->option[i].value;
	    break;
	default:
	  if( appData.debugMode )
	    fprintf(debugFP, "SettingsPopUp: unexpected case in switch.\n");
	  break;
	}
    }

    // make an attempt to align all spins and textbox controls
    maxWidth = maxTextWidth = 0;
    for(h=0; h<height; h++) {
	i = h + c*height;
        if(i >= cps->nrOptions) break;
	if(cps->option[i].type == Spin || cps->option[i].type == TextBox) {
	    Dimension w;
	    j=0;
	    XtSetArg(args[j], XtNwidth, &w);  j++;
	    XtGetValues(texts[h], args, j);
	    if(cps->option[i].type == Spin) {
		if(w > maxWidth) maxWidth = w;
		widest = texts[h];
	    } else {
		if(w > maxTextWidth) maxTextWidth = w;
		if(!widest) widest = texts[h];
	    }
	}
    }
    if(maxTextWidth + 110 < maxWidth)
	 maxTextWidth = maxWidth - 110;
    else maxWidth = maxTextWidth + 110;
    for(h=0; h<height; h++) {
	i = h + c*height;
        if(i >= cps->nrOptions) break;
	j=0;
	if(cps->option[i].type == Spin) {
	    XtSetArg(args[j], XtNwidth, maxWidth);  j++;
	    XtSetValues(texts[h], args, j);
	} else
	if(cps->option[i].type == TextBox) {
	    XtSetArg(args[j], XtNwidth, maxTextWidth);  j++;
	    XtSetValues(texts[h], args, j);
	}
    }
  }
    j=0;
    XtSetArg(args[j], XtNfromVert, anchor ? anchor : last);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    XtSetArg(args[j], XtNfromHoriz, widest ? widest : dialog);  j++;
    b_ok = XtCreateManagedWidget(_("OK"), commandWidgetClass, form, args, j);
    XtAddCallback(b_ok, XtNcallback, SettingsCallback, (XtPointer) 0);

    XtSetArg(args[j-1], XtNfromHoriz, b_ok);
    b_cancel = XtCreateManagedWidget(_("cancel"), commandWidgetClass, form, args, j);
    XtAddCallback(b_cancel, XtNcallback, SettingsPopDown, (XtPointer) 0);

    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "SettingsPopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    SettingsUp = True;

    previous = NULL;
    if(textField)SetFocus(textField, popup, (XEvent*) NULL, False);
}

void FirstSettingsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   SettingsPopUp(&first);
}

void SecondSettingsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   if(WaitForSecond(SettingsMenuIfReady)) return;
   SettingsPopUp(&second);
}

//----------------------------Generic dialog --------------------------------------------

// cloned from Engine Settings dialog

typedef void ButtonCallback(int n);

char *trialSound;
static Option *currentOption;
int MakeColors P((void));
void CreateGCs P((int redo));
void CreateXPMBoard P((char *s, int kind));
void CreateXPMPieces P((void));
void GenericReadout();

Option adjudicationOptions[] = {
{ 0, 0,    0, NULL, (void*) &appData.checkMates, "", NULL, CheckBox, _("Detect all Mates") },
{ 0, 0,    0, NULL, (void*) &appData.testClaims, "", NULL, CheckBox, _("Verify Engine Result Claims") },
{ 0, 0,    0, NULL, (void*) &appData.materialDraws, "", NULL, CheckBox, _("Draw if Insufficient Mating Material") },
{ 0, 0,    0, NULL, (void*) &appData.trivialDraws, "", NULL, CheckBox, _("Adjudicate Trivial Draws (3-Move Delay)") },
{ 0, 0,  100, NULL, (void*) &appData.ruleMoves, "", NULL, Spin, _("N-Move Rule:") },
{ 0, 0,    6, NULL, (void*) &appData.drawRepeats, "", NULL, Spin, _("N-fold Repeats:") },
{ 0, 0, 1000, NULL, (void*) &appData.adjudicateDrawMoves, "", NULL, Spin, _("Draw after N Moves Total:") },
{ 0,-5000, 0, NULL, (void*) &appData.adjudicateLossThreshold, "", NULL, Spin, _("Win / Loss Threshold:") },
{ 0, 0,    0, NULL, (void*) &first.scoreIsAbsolute, "", NULL, CheckBox, _("Negate Score of Engine #1") },
{ 0, 0,    0, NULL, (void*) &second.scoreIsAbsolute, "", NULL, CheckBox, _("Negate Score of Engine #2") },
{ 0, 1,    0, NULL, NULL, "", NULL, EndMark , "" }
};

void IcsOptionsOK(int n)
{
    ParseIcsTextColors();
}

Option icsOptions[] = {
{ 0, 0, 0, NULL, (void*) &appData.autoKibitz, "",  NULL, CheckBox, _("Auto-Kibitz") },
{ 0, 0, 0, NULL, (void*) &appData.autoComment, "", NULL, CheckBox, _("Auto-Comment") },
{ 0, 0, 0, NULL, (void*) &appData.autoObserve, "", NULL, CheckBox, _("Auto-Observe") },
{ 0, 0, 0, NULL, (void*) &appData.autoRaiseBoard, "", NULL, CheckBox, _("Auto-Raise Board") },
{ 0, 0, 0, NULL, (void*) &appData.bgObserve, "",   NULL, CheckBox, _("Background Observe while Playing") },
{ 0, 0, 0, NULL, (void*) &appData.dualBoard, "",   NULL, CheckBox, _("Dual Board for Background-Observed Game") },
{ 0, 0, 0, NULL, (void*) &appData.getMoveList, "", NULL, CheckBox, _("Get Move List") },
{ 0, 0, 0, NULL, (void*) &appData.quietPlay, "",   NULL, CheckBox, _("Quiet Play") },
{ 0, 0, 0, NULL, (void*) &appData.seekGraph, "",   NULL, CheckBox, _("Seek Graph") },
{ 0, 0, 0, NULL, (void*) &appData.autoRefresh, "", NULL, CheckBox, _("Auto-Refresh Seek Graph") },
{ 0, 0, 0, NULL, (void*) &appData.premove, "",     NULL, CheckBox, _("Premove") },
{ 0, 0, 0, NULL, (void*) &appData.premoveWhite, "", NULL, CheckBox, _("Premove for White") },
{ 0, 0, 0, NULL, (void*) &appData.premoveWhiteText, "", NULL, TextBox, _("First White Move:") },
{ 0, 0, 0, NULL, (void*) &appData.premoveBlack, "", NULL, CheckBox, _("Premove for Black") },
{ 0, 0, 0, NULL, (void*) &appData.premoveBlackText, "", NULL, TextBox, _("First Black Move:") },
{ 0, 0, 0, NULL, NULL, NULL, NULL, Break, "" },
{ 0, 0, 0, NULL, (void*) &appData.icsAlarm, "", NULL, CheckBox, _("Alarm") },
{ 0, 0, 100000000, NULL, (void*) &appData.icsAlarmTime, "", NULL, Spin, _("Alarm Time (msec):") },
//{ 0, 0, 0, NULL, (void*) &appData.chatBoxes, "", NULL, TextBox, _("Startup Chat Boxes:") },
{ 0, 0, 0, NULL, (void*) &appData.colorize, "", NULL, CheckBox, _("Colorize Messages") },
{ 0, 0, 0, NULL, (void*) &appData.colorShout, "", NULL, TextBox, _("Shout Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorSShout, "", NULL, TextBox, _("S-Shout Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorChannel1, "", NULL, TextBox, _("Channel #1 Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorChannel, "", NULL, TextBox, _("Other Channel Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorKibitz, "", NULL, TextBox, _("Kibitz Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorTell, "", NULL, TextBox, _("Tell Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorChallenge, "", NULL, TextBox, _("Challenge Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorRequest, "", NULL, TextBox, _("Request Text Colors:") },
{ 0, 0, 0, NULL, (void*) &appData.colorSeek, "", NULL, TextBox, _("Seek Text Colors:") },
{ 0, 0, 0, NULL, (void*) &IcsOptionsOK, "", NULL, EndMark , "" }
};

Option loadOptions[] = {
{ 0, 0, 0, NULL, (void*) &appData.autoDisplayTags, "", NULL, CheckBox, _("Auto-Display Tags") },
{ 0, 0, 0, NULL, (void*) &appData.autoDisplayComment, "", NULL, CheckBox, _("Auto-Display Comment") },
{ 0, 0, 0, NULL, NULL, NULL, NULL, Label, _("Auto-Play speed of loaded games\n(0 = instant, -1 = off):") },
{ 0, -1, 10000000, NULL, (void*) &appData.timeDelay, "", NULL, Fractional, _("Seconds per Move:") },
{ 0,  0, 0, NULL, NULL, "", NULL, EndMark , "" }
};

Option saveOptions[] = {
{ 0, 0, 0, NULL, (void*) &appData.autoSaveGames, "", NULL, CheckBox, _("Auto-Save Games") },
{ 0, 0, 0, NULL, (void*) &appData.saveGameFile, "", NULL, FileName,  _("Save Games on File:") },
{ 0, 0, 0, NULL, (void*) &appData.savePositionFile, "", NULL, FileName,  _("Save Final Positions on File:") },
{ 0, 0, 0, NULL, (void*) &appData.pgnEventHeader, "", NULL, TextBox,  _("PGN Event Header:") },
{ 0, 0, 0, NULL, (void*) &appData.oldSaveStyle, "", NULL, CheckBox, _("Old Save Style (as opposed to PGN)") },
{ 0, 0, 0, NULL, (void*) &appData.saveExtendedInfoInPGN, "", NULL, CheckBox, _("Save Score/Depth Info in PGN") },
{ 0, 0, 0, NULL, (void*) &appData.saveOutOfBookInfo, "", NULL, CheckBox, _("Save Out-of-Book Info in PGN           ") },
{ 0, 1, 0, NULL, NULL, "", NULL, EndMark , "" }
};

char *soundNames[] = {
	N_("No Sound"),
	N_("Default Beep"),
	N_("Above WAV File"),
	N_("Ching"),
	N_("Click"),
	N_("Ding"),
	N_("Gong"),
	N_("Laser"),
	N_("Penalty"),
	N_("Phone"),
	N_("Thud"),
	N_("Challenge"),
	N_("Tell"),
	NULL,
	N_("User File")
};

char *soundFiles[] = { // sound files corresponding to above names
	"",
	"$",
	"*", // kludge alert: as first thing in the dialog readout this is replaced with the user-given .WAV filename
	"ching.wav",
	"click.wav",
	"ding1.wav",
	"gong.wav",
	"laser.wav",
	"penalty.wav",
	"phone.wav",
	"thud.wav",
	"challenge.wav",
	"tell.wav",
	NULL,
	NULL
};

void Test(int n)
{
    if(soundFiles[values[3]]) PlaySound(soundFiles[values[3]]);
}

Option soundOptions[] = {
{ 0, 0, 0, NULL, (void*) &appData.soundProgram, "", NULL, TextBox, _("Sound Program:") },
{ 0, 0, 0, NULL, (void*) &appData.soundDirectory, "", NULL, PathName, _("Sounds Directory:") },
{ 0, 0, 0, NULL, (void*) (soundFiles+2) /* kludge! */, "", NULL, FileName, _("User WAV File:") },
{ 0, 0, 0, NULL, (void*) &trialSound, (char*) soundNames, soundFiles, ComboBox, _("Try-Out Sound:") },
{ 0, 1, 0, NULL, (void*) &Test, NULL, NULL, Button, _("Play") },
{ 0, 0, 0, NULL, (void*) &appData.soundMove, (char*) soundNames, soundFiles, ComboBox, _("Move:") },
{ 0, 0, 0, NULL, (void*) &appData.soundIcsWin, (char*) soundNames, soundFiles, ComboBox, _("Win:") },
{ 0, 0, 0, NULL, (void*) &appData.soundIcsLoss, (char*) soundNames, soundFiles, ComboBox, _("Lose:") },
{ 0, 0, 0, NULL, (void*) &appData.soundIcsDraw, (char*) soundNames, soundFiles, ComboBox, _("Draw:") },
{ 0, 0, 0, NULL, (void*) &appData.soundIcsUnfinished, (char*) soundNames, soundFiles, ComboBox, _("Unfinished:") },
{ 0, 0, 0, NULL, (void*) &appData.soundIcsAlarm, (char*) soundNames, soundFiles, ComboBox, _("Alarm:") },
{ 0, 0, 0, NULL, (void*) &appData.soundShout, (char*) soundNames, soundFiles, ComboBox, _("Shout:") },
{ 0, 0, 0, NULL, (void*) &appData.soundSShout, (char*) soundNames, soundFiles, ComboBox, _("S-Shout:") },
{ 0, 0, 0, NULL, (void*) &appData.soundChannel, (char*) soundNames, soundFiles, ComboBox, _("Channel:") },
{ 0, 0, 0, NULL, (void*) &appData.soundChannel1, (char*) soundNames, soundFiles, ComboBox, _("Channel 1:") },
{ 0, 0, 0, NULL, (void*) &appData.soundTell, (char*) soundNames, soundFiles, ComboBox, _("Tell:") },
{ 0, 0, 0, NULL, (void*) &appData.soundKibitz, (char*) soundNames, soundFiles, ComboBox, _("Kibitz:") },
{ 0, 0, 0, NULL, (void*) &appData.soundChallenge, (char*) soundNames, soundFiles, ComboBox, _("Challenge:") },
{ 0, 0, 0, NULL, (void*) &appData.soundRequest, (char*) soundNames, soundFiles, ComboBox, _("Request:") },
{ 0, 0, 0, NULL, (void*) &appData.soundSeek, (char*) soundNames, soundFiles, ComboBox, _("Seek:") },
{ 0, 1, 0, NULL, NULL, "", NULL, EndMark , "" }
};

void SetColor(char *colorName, Widget box)
{
	Arg args[5];
	Pixel buttonColor;
	XrmValue vFrom, vTo;
	if (!appData.monoMode) {
	    vFrom.addr = (caddr_t) colorName;
	    vFrom.size = strlen(colorName);
	    XtConvert(shellWidget, XtRString, &vFrom, XtRPixel, &vTo);
	    if (vTo.addr == NULL) {
	  	buttonColor = (Pixel) -1;
	    } else {
		buttonColor = *(Pixel *) vTo.addr;
	    }
	}
	XtSetArg(args[0], XtNbackground, buttonColor);;
	XtSetValues(box, args, 1);
}

void AdjustColor(int i)
{
    int n = currentOption[i].value, col, j, r, g, b, step = 10;
    char *s, buf[MSG_SIZ]; // color string
    Arg args[5];
    XtSetArg(args[0], XtNstring, &s);
    XtGetValues(currentOption[i-n-1].handle, args, 1);
    if(sscanf(s, "#%x", &col) != 1) return;   // malformed
    b = col & 0xFF; g = col & 0xFF00; r = col & 0xFF0000;
    switch(n) {
	case 1: g -= 0x100*step; b -= step; break;
	case 2: r -= 0x10000*step; b -= step; break;
	case 3: g -= 0x100*step; r -= 0x10000*step; break;
	case 4: r += 0x10000*step; g += 0x100*step; b += step; break;
    }
    if(r < 0) r = 0; if(g < 0) g = 0; if(b < 0) b = 0;
    if(r > 0xFF0000) r = 0xFF0000; if(g > 0xFF00) g = 0xFF00; if(b > 0xFF) b = 0xFF;
    col = r | g | b;
    snprintf(buf, MSG_SIZ, "#%06x", col);
    for(j=1; j<7; j++) if(buf[j] >= 'a') buf[j] -= 32; // capitalize
    SetColor(buf, currentOption[i-n].handle);
    XtSetArg(args[0], XtNstring, buf);
    XtSetValues(currentOption[i-n-1].handle, args, 1);
}

void BoardOptionsOK(int n)
{
    if(appData.overrideLineGap >= 0) lineGap = appData.overrideLineGap;
    MakeColors(); CreateGCs(True);
    CreateXPMPieces();
    CreateXPMBoard(appData.liteBackTextureFile, 1);
    CreateXPMBoard(appData.darkBackTextureFile, 0);
    InitDrawingSizes(-1, 0);
    DrawPosition(True, NULL);
}

Option boardOptions[] = {
{ 0,   0, 70, NULL, (void*) &appData.whitePieceColor, "", NULL, TextBox, _("White Piece Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0,   0, 70, NULL, (void*) &appData.blackPieceColor, "", NULL, TextBox, _("Black Piece Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0,   0, 70, NULL, (void*) &appData.lightSquareColor, "", NULL, TextBox, _("Light Square Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0,   0, 70, NULL, (void*) &appData.darkSquareColor, "", NULL, TextBox, _("Dark Square Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0,   0, 70, NULL, (void*) &appData.highlightSquareColor, "", NULL, TextBox, _("Highlight Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0,   0, 70, NULL, (void*) &appData.premoveHighlightColor, "", NULL, TextBox, _("Premove Highlight Color:") },
{ 1000, 1, 0, NULL, NULL, NULL, NULL, Button, "      " },
{    1, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "R" },
{    2, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "G" },
{    3, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "B" },
{    4, 1, 0, NULL, (void*) &AdjustColor, NULL, NULL, Button, "W" },
{ 0, 0, 0, NULL, (void*) &appData.upsideDown, "", NULL, CheckBox, _("Flip Pieces Shogi Style") },
{ 0, 0, 0, NULL, (void*) &appData.allWhite, "", NULL, CheckBox, _("Use Outline Pieces for Black") },
{ 0, 0, 0, NULL, (void*) &appData.monoMode, "", NULL, CheckBox, _("Mono Mode") },
{ 0,-1, 5, NULL, (void*) &appData.overrideLineGap, "", NULL, Spin, _("Line Gap ( -1 = default for board size):") },
{ 0, 0, 0, NULL, (void*) &appData.liteBackTextureFile, "", NULL, FileName, _("Light-Squares Texture File:") },
{ 0, 0, 0, NULL, (void*) &appData.darkBackTextureFile, "", NULL, FileName, _("Dark-Squares Texture File:") },
{ 0, 0, 0, NULL, (void*) &appData.bitmapDirectory, "", NULL, PathName, _("Directory with Bitmap Pieces:") },
{ 0, 0, 0, NULL, (void*) &appData.pixmapDirectory, "", NULL, PathName, _("Directory with Pixmap Pieces:") },
{ 0, 0, 0, NULL, (void*) &BoardOptionsOK, "", NULL, EndMark , "" }
};

void GenericReadout()
{
    int i, j;
    String name, val;
    Arg args[16];
    char buf[MSG_SIZ];
    float x;
	for(i=0; ; i++) { // send all options that had to be OK-ed to engine
	    switch(currentOption[i].type) {
		case TextBox:
		case FileName:
		case PathName:
		    XtSetArg(args[0], XtNstring, &val);
		    XtGetValues(currentOption[i].handle, args, 1);
		    if(*(char**) currentOption[i].target == NULL || strcmp(*(char**) currentOption[i].target, val)) {
			safeStrCpy(currentOption[i].name + 100, val, MSG_SIZ-100); // text value kept in pivate storage for each option
			*(char**) currentOption[i].target = currentOption[i].name + 100; // option gets to point to that
		    }
		    break;
		case Spin:
		case Fractional:
		    XtSetArg(args[0], XtNstring, &val);
		    XtGetValues(currentOption[i].handle, args, 1);
		    sscanf(val, "%f", &x);
		    if(x > currentOption[i].max) x = currentOption[i].max;
		    if(x < currentOption[i].min) x = currentOption[i].min;
		    if(currentOption[i].value != x) {
			currentOption[i].value = x;
			if(currentOption[i].type == Spin) *(int*) currentOption[i].target = x;
			else *(float*) currentOption[i].target = x;
		    }
		    break;
		case CheckBox:
		    j = 0;
		    XtSetArg(args[0], XtNstate, &j);
		    XtGetValues(currentOption[i].handle, args, 1);
		    if(currentOption[i].value != j) {
			currentOption[i].value = j;
			*(Boolean*) currentOption[i].target = j;
		    }
		    break;
		case ComboBox:
		    val = ((char**)currentOption[i].choice)[values[i]];
		    if(val && (*(char**) currentOption[i].target == NULL || strcmp(*(char**) currentOption[i].target, val))) {
		      if(*(char**) currentOption[i].target) free(*(char**) currentOption[i].target);
		      *(char**) currentOption[i].target = strdup(val);
		    }
		    break;
		case EndMark:
		    if(currentOption[i].target) // callback for implementing necessary actions on OK (like redraw)
			((ButtonCallback*) currentOption[i].target)(i);
		    break;
	    default:
		printf("GenericReadout: unexpected case in switch.\n");
		case Button:
		case Label:
	      break;
	    }
	    if(currentOption[i].type == EndMark) break;
	}
}

void GenericCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    String name, val;
    Arg args[16];
    char buf[MSG_SIZ];
    int i, j;
    int data = (intptr_t) client_data;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, _("cancel")) == 0) {
        SettingsPopDown();
        return;
    }
    if (strcmp(name, _("OK")) == 0) { // save buttons imply OK
        GenericReadout();
        SettingsPopDown();
        return;
    }
    ((ButtonCallback*) currentOption[data].target)(data);
}

void
GenericPopUp(Option *option, char *title)
{
    Arg args[16];
    Widget popup, layout, dialog, edit=NULL, form,  last, b_ok, b_cancel, leftMargin = NULL, textField = NULL;
    Window root, child;
    int x, y, i, j, height=999, width=1, h, c, w;
    int win_x, win_y, maxWidth, maxTextWidth;
    unsigned int mask;
    char def[MSG_SIZ], *msg;
    static char pane[6] = "paneX";
    Widget texts[100], forelast = NULL, anchor, widest, lastrow = NULL;

    currentOption = option; // make available to callback
    // kludge: fake address of a ChessProgramState struct that contains the options, so Spin and Combo callbacks work on it
    currentCps = (ChessProgramState *) ((char *) option - ((char *)&first.option - (char *)&first));

//    if(cps->nrOptions > 50) width = 4; else if(cps->nrOptions>24) width = 2; else width = 1;
//    height = cps->nrOptions / width + 1;
     i = 0;
    XtSetArg(args[i], XtNresizable, True); i++;
    SettingsShell = popup =
      XtCreatePopupShell(title, transientShellWidgetClass,
			 shellWidget, args, i);

    layout =
      XtCreateManagedWidget(layoutName, formWidgetClass, popup,
			    layoutArgs, XtNumber(layoutArgs));
  for(c=0; c<width; c++) {
    pane[4] = 'A'+c;
    form =
      XtCreateManagedWidget(pane, formWidgetClass, layout,
			    formArgs, XtNumber(formArgs));
    j=0;
    XtSetArg(args[j], XtNfromHoriz, leftMargin);  j++;
    XtSetValues(form, args, j);
    leftMargin = form;

    last = widest = NULL; anchor = lastrow;
    for(h=0; h<height; h++) {
	i = h + c*height;
	if(option[i].type == EndMark) break;
	lastrow = forelast;
	forelast = last;
	switch(option[i].type) {
	  case Fractional:
	    snprintf(def, MSG_SIZ,  "%.2f", *(float*)option[i].target);
	    option[i].value = *(float*)option[i].target;
	    goto tBox;
	  case Spin:
	    snprintf(def, MSG_SIZ,  "%d", option[i].value = *(int*)option[i].target);
	  case TextBox:
	  case FileName:
	  case PathName:
          tBox:
	    if(option[i].name[0]) {
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNright, XtChainLeft); j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    texts[h] =
	    dialog = XtCreateManagedWidget(option[i].name, labelWidgetClass, form, args, j);
	    } else texts[h] = dialog = NULL;
	    w = option[i].type == Spin || option[i].type == Fractional ? 70 : option[i].max ? option[i].max : 205;
	    if(option[i].type == FileName || option[i].type == PathName) w -= 55;
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, dialog);  j++;
	    XtSetArg(args[j], XtNborderWidth, 1); j++;
	    XtSetArg(args[j], XtNwidth, w); j++;
	    if(option[i].type == TextBox && option[i].min) XtSetArg(args[j], XtNheight, option[i].min); j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNeditType, XawtextEdit);  j++;
	    XtSetArg(args[j], XtNuseStringInPlace, False);  j++;
	    XtSetArg(args[j], XtNdisplayCaret, False);  j++;
	    XtSetArg(args[j], XtNright, XtChainRight);  j++;
	    XtSetArg(args[j], XtNresizable, True);  j++;
	    XtSetArg(args[j], XtNstring, option[i].type==Spin || option[i].type==Fractional ? def : *(char**)option[i].target);  j++;
	    XtSetArg(args[j], XtNinsertPosition, 9999);  j++;
	    edit = last;
	    option[i].handle = (void*)
		(textField = last = XtCreateManagedWidget("text", asciiTextWidgetClass, form, args, j));
	    XtAddEventHandler(last, ButtonPressMask, False, SetFocus, (XtPointer) popup);

	    if(option[i].type == TextBox || option[i].type == Fractional) break;

	    // add increment and decrement controls for spin
	    j=0;
	    XtSetArg(args[j], XtNfromVert, edit);  j++;
	    XtSetArg(args[j], XtNfromHoriz, last);  j++;
	    XtSetArg(args[j], XtNleft, XtChainRight); j++;
	    XtSetArg(args[j], XtNright, XtChainRight); j++;
	    if(option[i].type == FileName || option[i].type == PathName) {
		w = 50; msg = "browse";
	    } else {
		XtSetArg(args[j], XtNheight, 10);  j++;
		w = 20; msg = "+";
	    }
	    XtSetArg(args[j], XtNwidth, w);  j++;
	    edit = XtCreateManagedWidget(msg, commandWidgetClass, form, args, j);
	    XtAddCallback(edit, XtNcallback, SpinCallback,
			  (XtPointer)(intptr_t) i);

	    if(option[i].type != Spin) break;

	    j=0;
	    XtSetArg(args[j], XtNfromVert, edit);  j++;
	    XtSetArg(args[j], XtNfromHoriz, last);  j++;
	    XtSetArg(args[j], XtNheight, 10);  j++;
	    XtSetArg(args[j], XtNwidth, 20);  j++;
	    XtSetArg(args[j], XtNleft, XtChainRight); j++;
	    XtSetArg(args[j], XtNright, XtChainRight); j++;
	    last = XtCreateManagedWidget("-", commandWidgetClass, form, args, j);
	    XtAddCallback(last, XtNcallback, SpinCallback,
			  (XtPointer)(intptr_t) i);
	    break;
	  case CheckBox:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNwidth, 10);  j++;
	    XtSetArg(args[j], XtNheight, 10);  j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNright, XtChainLeft); j++;
	    XtSetArg(args[j], XtNstate, option[i].value = *(Boolean*)option[i].target);  j++;
	    option[i].handle = (void*)
		(dialog = XtCreateManagedWidget(" ", toggleWidgetClass, form, args, j));
	  case Label:
	    msg = option[i].name;
	    if(*msg == NULLCHAR) msg = option[i].textValue;
	    if(!msg) break;
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, option[i].type != Label ? dialog : NULL);  j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    last = XtCreateManagedWidget(msg, labelWidgetClass, form, args, j);
	    break;
	  case Button:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, option[i].min & 1 ? lastrow : last);  j++;
	    if(option[i].min & 1) { XtSetArg(args[j], XtNfromHoriz, last);  j++; }
	    else  { XtSetArg(args[j], XtNfromHoriz, NULL);  j++; lastrow = forelast; }
	    if(option[i].max) XtSetArg(args[j], XtNwidth, option[i].max);  j++;
	    if(option[i].textValue) { // special for buttons of New Variant dialog
		XtSetArg(args[j], XtNsensitive, appData.noChessProgram || option[i].value < 0
					 || strstr(first.variants, VariantName(option[i].value))); j++;
		XtSetArg(args[j], XtNborderWidth, (gameInfo.variant == option[i].value)+1); j++;
	    }
	    option[i].handle = (void*)
		(dialog = last = XtCreateManagedWidget(option[i].name, commandWidgetClass, form, args, j));
	    if(option[i].target == NULL) SetColor( *(char**) option[i-1].target, last); else
	    XtAddCallback(last, XtNcallback, GenericCallback,
			  (XtPointer)(intptr_t) i);
	    if(option[i].textValue) SetColor( option[i].textValue, last);
	    forelast = lastrow; // next button can go on same row
	    break;
	  case ComboBox:
	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNright, XtChainLeft); j++;
	    XtSetArg(args[j], XtNborderWidth, 0);  j++;
	    XtSetArg(args[j], XtNjustify, XtJustifyLeft);  j++;
	    texts[h] = dialog = XtCreateManagedWidget(option[i].name, labelWidgetClass, form, args, j);

	    for(j=0; option[i].choice[j]; j++)
		if(*(char**)option[i].target && !strcmp(*(char**)option[i].target, option[i].choice[j])) break;
	    option[i].value = j + (option[i].choice[j] == NULL);

	    j=0;
	    XtSetArg(args[j], XtNfromVert, last);  j++;
	    XtSetArg(args[j], XtNfromHoriz, dialog);  j++;
	    XtSetArg(args[j], XtNwidth, option[i].max ? option[i].max : 100);  j++;
	    XtSetArg(args[j], XtNleft, XtChainLeft); j++;
	    XtSetArg(args[j], XtNmenuName, XtNewString(option[i].name));  j++;
	    XtSetArg(args[j], XtNlabel, ((char**)option[i].textValue)[option[i].value]);  j++;
	    option[i].handle = (void*)
		(last = XtCreateManagedWidget(" ", menuButtonWidgetClass, form, args, j));
	    CreateComboPopup(last, option[i].name, i, (char **) option[i].textValue);
	    values[i] = option[i].value;
	    break;
	  case Break:
	    width++;
	    height = i+1;
	    break;
	default:
	    printf("GenericPopUp: unexpected case in switch.\n");
	    break;
	}
    }

    // make an attempt to align all spins and textbox controls
    maxWidth = maxTextWidth = 0;
    for(h=0; h<height; h++) {
	i = h + c*height;
	if(option[i].type == EndMark) break;
	if(option[i].type == Spin || option[i].type == TextBox || option[i].type == ComboBox
				  || option[i].type == PathName || option[i].type == FileName) {
	    Dimension w;
	    if(!texts[h]) continue;
	    j=0;
	    XtSetArg(args[j], XtNwidth, &w);  j++;
	    XtGetValues(texts[h], args, j);
	    if(option[i].type == Spin) {
		if(w > maxWidth) maxWidth = w;
		widest = texts[h];
	    } else {
		if(w > maxTextWidth) maxTextWidth = w;
		if(!widest) widest = texts[h];
	    }
	}
    }
    if(maxTextWidth + 110 < maxWidth)
	 maxTextWidth = maxWidth - 110;
    else maxWidth = maxTextWidth + 110;
    for(h=0; h<height; h++) {
	i = h + c*height;
	if(option[i].type == EndMark) break;
	if(!texts[h]) continue;
	j=0;
	if(option[i].type == Spin) {
	    XtSetArg(args[j], XtNwidth, maxWidth);  j++;
	    XtSetValues(texts[h], args, j);
	} else
	if(option[i].type == TextBox || option[i].type == ComboBox || option[i].type == PathName || option[i].type == FileName) {
	    XtSetArg(args[j], XtNwidth, maxTextWidth);  j++;
	    XtSetValues(texts[h], args, j);
	}
    }
  }

  if(!(option[i].min & 2)) {
    j=0;
    if(option[i].min & 1) { XtSetArg(args[j], XtNfromHoriz, last); last = forelast; } else
    XtSetArg(args[j], XtNfromHoriz, widest ? widest : dialog);  j++;
    XtSetArg(args[j], XtNfromVert, anchor ? anchor : last);  j++;
    XtSetArg(args[j], XtNbottom, XtChainBottom);  j++;
    XtSetArg(args[j], XtNtop, XtChainBottom);  j++;
    XtSetArg(args[j], XtNleft, XtChainRight);  j++;
    XtSetArg(args[j], XtNright, XtChainRight);  j++;
    b_ok = XtCreateManagedWidget(_("OK"), commandWidgetClass, form, args, j);
    XtAddCallback(b_ok, XtNcallback, GenericCallback, (XtPointer) 0);

    XtSetArg(args[0], XtNfromHoriz, b_ok);
    b_cancel = XtCreateManagedWidget(_("cancel"), commandWidgetClass, form, args, j);
    XtAddCallback(b_cancel, XtNcallback, SettingsPopDown, (XtPointer) 0);
  }

    XtRealizeWidget(popup);
    CatchDeleteWindow(popup, "SettingsPopDown");

    XQueryPointer(xDisplay, xBoardWindow, &root, &child,
		  &x, &y, &win_x, &win_y, &mask);

    XtSetArg(args[0], XtNx, x - 10);
    XtSetArg(args[1], XtNy, y - 30);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    SettingsUp = True;

    previous = NULL;
    if(textField)SetFocus(textField, popup, (XEvent*) NULL, False);
}


void IcsOptionsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   GenericPopUp(icsOptions, _("ICS Options"));
}

void LoadOptionsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   GenericPopUp(loadOptions, _("Load Game Options"));
}

void SaveOptionsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   GenericPopUp(saveOptions, _("Save Game Options"));
}

void SoundOptionsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   soundFiles[2] = "*";
   GenericPopUp(soundOptions, _("Sound Options"));
}

void BoardOptionsProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   GenericPopUp(boardOptions, _("Board Options"));
}

void EngineMenuProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
   GenericPopUp(adjudicationOptions, "Adjudicate non-ICS Games");
}
//---------------------------- Chat Windows ----------------------------------------------

void OutputChatMessage(int partner, char *mess)
{
    return; // dummy
}

