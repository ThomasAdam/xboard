/*
 * frontend.h -- Interface exported by all XBoard front ends
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard,
 * Massachusetts.
 *
 * Enhancements Copyright 1992-2001, 2002, 2003, 2004, 2005, 2006,
 * 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Free Software Foundation, Inc.
 *
 * Enhancements Copyright 2005 Alessandro Scotti
 *
 * The following terms apply to Digital Equipment Corporation's copyright
 * interest in XBoard:
 * ------------------------------------------------------------------------
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ------------------------------------------------------------------------
 *
 * The following terms apply to the enhanced version of XBoard
 * distributed by the Free Software Foundation:
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

#ifndef XB_FRONTEND
#define XB_FRONTEND

#include <stdio.h>

char *T_(char *);
void ModeHighlight(void);
void SetICSMode(void);
void SetGNUMode(void);
void SetNCPMode(void);
void SetCmailMode(void);
void SetTrainingModeOn(void);
void SetTrainingModeOff(void);
void SetUserThinkingEnables(void);
void SetMachineThinkingEnables(void);
void DisplayTitle(String);
void DisplayMessage(String, String);
void DisplayMoveError(String);

void DisplayNote(String);

void DisplayInformation(String);
void AskQuestion(String, String, String, ProcRef);
void DisplayIcsInteractionTitle(String);
void ParseArgsFromString(char *);
void ParseArgsFromFile(FILE *f);
void DrawPosition(int, Board);
void ResetFrontEnd(void);
void NotifyFrontendLogin(void);
void CommentPopUp(String, String);
void CommentPopDown(void);
void EditCommentPopUp(int, String, String);
void ErrorPopDown(void);
int  EventToSquare(int, int);
void DrawSeekAxis(int, int, int, int);
void DrawSeekBackground(int, int, int, int);
void DrawSeekText(char *, int, int);
void DrawSeekDot(int, int, int);
void PopUpMoveDialog(char);

void RingBell(void);
int  Roar(void);
void PlayIcsWinSound(void);
void PlayIcsLossSound(void);
void PlayIcsDrawSound(void);
void PlayIcsUnfinishedSound(void);
void PlayAlarmSound(void);
void PlayTellSound(void);
int  PlaySoundFile(char *);
void PlaySoundByColor(void);
void EchoOn(void);
void EchoOff(void);
void Raw(void);
void Colorize(ColorClass, int);
char *InterpretFileName(char *, char *);
void DoSleep(int);
void DoEvents(void);

char *UserName(void);
char *HostName(void);

int ClockTimerRunning(void);
int StopClockTimer(void);
void StartClockTimer(long);
void DisplayWhiteClock(long, int);
void DisplayBlackClock(long, int);
void UpdateLogos(int);

int LoadGameTimerRunning(void);
int StopLoadGameTimer(void);
void StartLoadGameTimer(long);
void AutoSaveGame(void);

void ScheduleDelayedEvent(DelayedEventCallback, long);
DelayedEventCallback GetDelayedEvent(void);
void CancelDelayedEvent(void);
// [HGM] mouse: next six used by mouse handler, which was moved to backend
extern int fromX, fromY, toX, toY;
void PromotionPopUp(char);
void DragPieceBegin(int, int, Boolean);
void DragPieceEnd(int, int);
void DragPieceMove(int, int);
void LeftClick(ClickType, int, int);
int  RightClick(ClickType, int, int, int *, int *);

int StartChildProcess(char *, char *, ProcRef *);
void DestroyChildProcess(ProcRef, int/*boolean*/);
void InterruptChildProcess(ProcRef);
void RunCommand(char *);

int OpenTelnet(char *, char *, ProcRef *);
int OpenTCP(char *, char *, ProcRef *);
int OpenCommPort(char *, ProcRef *);
int OpenLoopback(ProcRef *);
int OpenRcmd(char *, char *, char *, ProcRef *);

typedef void (*InputCallback)(InputSourceRef, VOIDSTAR, char *, int, int);
/* pr == NoProc means the local keyboard */
InputSourceRef AddInputSource(ProcRef, int, InputCallback, VOIDSTAR);
void RemoveInputSource(InputSourceRef);

/* pr == NoProc means the local display */
int OutputToProcess(ProcRef, char *, int, int *);
int OutputToProcessDelayed(ProcRef, char *, int, int *, long);

void CmailSigHandlerCallBack(InputSourceRef, VOIDSTAR, char *, int, int);

extern ProcRef cmailPR;
extern int shiftKey, controlKey;

/* in xgamelist.c or winboard.c */
void GLT_ClearList(void);
void GLT_DeSelectList(void);
void GLT_AddToList(char *);
Boolean GLT_GetFromList(int, char *);

extern char lpUserGLT[];
extern char *homeDir;

/* these are in wgamelist.c */
void GameListPopUp(FILE *, char *);
void GameListPopDown(void);
void GameListHighlight(int);
void GameListDestroy(void);
void GameListUpdate(void);
FILE *GameFile(void);

/* these are in wedittags.c */
void EditTagsPopUp(char *, char **);
void TagsPopUp(char *, char *);
void TagsPopDown(void);

void ParseIcsTextColors(void);
int  ICSInitScript(void);
void StartAnalysisClock(void);
void EngineOutputPopUp(void);
void EgineOutputPopDown(void);

void SetHighlights(int, int, int, int);
void ClearHighlights(void);
void SetPremoveHighlights(int, int, int, int);
void ClearPremoveHighlights(void);

void AnimateAtomicCapture(Board, int, int, int, int);
void ShutDownFrontEnd(void);
void BoardToTop(void);
void AnimateMove(Board, int, int, int, int);
void HistorySet(char movelist[][2*MOVE_LEN], int, int, int);
void FreezeUI(void);
void ThawUI(void);
void ChangeDragPiece(ChessSquare);
void CopyFENToClipboard(void);
extern char *programName;
extern int commentUp;
extern char *firstChessProgramNames;

void GreyRevert(Boolean);
void EnableNamedMenuItem(char *, int);

typedef struct FrontEndProgramStats_TAG {
    int which;
    int depth;
    u64 nodes;
    int score;
    int time;
    char * pv;
    char * hint;
    int an_move_index;
    int an_move_count;
} FrontEndProgramStats;

void SetProgramStats(FrontEndProgramStats *); /* [AS] */

void EngineOutputPopUp(void);
void EngineOutputPopDown(void);
int  EngineOutputIsUp(void);
int  EngineOutputDialogExists(void);
void EvalGraphPopUp(void);
Boolean EvalGraphIsUp(void);
int  EvalGraphDialogExists(void);
void SlavePopUp(void);
void ActivateTheme(int);
char *Col2Text(int);

/* these are in xhistory.c  */
Boolean MoveHistoryIsUp(void);
void HistoryPopUp(void);
void FindMoveByCharIndex(int);

#endif /* XB_FRONTEND */
