/*
 * menus.h -- platform-indendent menu handling code for XBoard
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard,
 * Massachusetts.
 *
 * Enhancements Copyright 1992-2001, 2002, 2003, 2004, 2005, 2006,
 * 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Free Software Foundation, Inc.
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



typedef void MenuProc(void);

typedef struct {
    char *string;
    char *accel;
    char *ref;
    MenuProc *proc;
    void *handle;
} MenuItem;

typedef struct {
    char *name;
    char *ref;
    MenuItem *mi;
} Menu;

typedef struct {
    char *name;
    Boolean value;
} Enables;

extern Menu menuBar[];

void ErrorPopUp(char *, char *, int);
void AppendEnginesToMenu(char *);
void LoadGameProc(void);
void LoadNextGameProc(void);
void LoadPrevGameProc(void);
void ReloadGameProc(void);
void LoadPositionProc(void);
void LoadNextPositionProc(void);
void LoadPrevPositionProc(void);
void ReloadPositionProc(void);
void CopyPositionProc(void);
void PastePositionProc(void);
void CopyGameProc(void);
void CopyGameListProc(void);
void PasteGameProc(void);
void SaveGameProc(void);
void SavePositionProc(void);
void ReloadCmailMsgProc(void);
void QuitProc(void);
void AnalyzeModeProc(void);
void AnalyzeFileProc(void);
void MatchProc(void);
void MatchOptionsProc(void);
void EditTagsProc(void);
void EditCommentProc(void);
void IcsInputBoxProc(void);
void ChatProc(void);
void AdjuWhiteProc(void);
void AdjuBlackProc(void);
void AdjuDrawProc(void);
void RevertProc(void);
void AnnotateProc(void);
void AlwaysQueenProc(void);
void AnimateDraggingProc(void);
void AnimateMovingProc(void);
void AutoflagProc(void);
void AutoflipProc(void);
void BlindfoldProc(void);
void FlashMovesProc(void);
void FlipViewProc(void);
void HighlightDraggingProc(void);
void HighlightLastMoveProc(void);
void HighlightArrowProc(void);
void MoveSoundProc(void);
//void IcsAlarmProc(void);
void OneClickProc(void);
void PeriodicUpdatesProc(void);
void PonderNextMoveProc(void);
void PopupMoveErrorsProc(void);
void PopupExitMessageProc(void);
//void PremoveProc(void);
void ShowCoordsProc(void);
void ShowThinkingProc(void);
void HideThinkingProc(void);
void TestLegalityProc(void);
void SaveSettingsProc(void);
void SaveOnExitProc(void);
void InfoProc(void);
void ManProc(void);
void GuideProc(void);
void HomePageProc(void);
void NewsPageProc(void);
void BugReportProc(void);
void AboutGameProc(void);
void AboutProc(void);
void DebugProc(void);
void NothingProc(void);
void ShuffleMenuProc(void);
void EngineMenuProc(void);
void UciMenuProc(void);
void TimeControlProc(void);
void OptionsProc(void);
void NewVariantProc(void);
void IcsTextProc(void);
void LoadEngine1Proc(void);
void LoadEngine2Proc(void);
void FirstSettingsProc(void);
void SecondSettingsProc(void);
void GameListOptionsProc(void);
void IcsOptionsProc(void);
void SoundOptionsProc(void);
void BoardOptionsProc(void);
void LoadOptionsProc(void);
void SaveOptionsProc(void);
void SaveSettings(char *);
void EditBookProc(void);
void InitMenuMarkers(void);
void ShowGameListProc(void); // in ngamelist.c
void HistoryShowProc(void);  // in nhistory.c

// only here because it is the only header shared by xoptions.c and usystem.c
void SetTextColor(char **, int, int, int);
void ConsoleWrite(char *, int);

// must be moved to xengineoutput.h

void EngineOutputProc(void);
void EvalGraphProc(void);

int SaveGameListAsText(FILE *);
void FileNamePopUp(char *, char *, char *, FileProc, char *);

void AppendMenuItem(char *, int);
MenuItem *MenuNameToItem(char *);
void SetMenuEnables(Enables *);
void EnableButtonBar(int);
char *ModeToWidgetName(GameMode);
void CreateAnimVars(void);
void CopySomething(char *);


extern char  *gameCopyFilename, *gamePasteFilename;
extern Boolean saveSettingsOnExit;
extern char *settingsFileName;
extern int firstEngineItem;



#define CHECK (void *) 1
#define RADIO (void *) 2

#define OPTIONSDIALOG
#define INFOFILE     "xboard.info"
