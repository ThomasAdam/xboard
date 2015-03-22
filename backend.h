/*
 * backend.h -- Interface exported by XBoard back end
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

#ifndef XB_BACKEND
#define XB_BACKEND

#include "lists.h"

typedef int (*FileProc)(FILE *f, int n, char *title);

extern char *wbOptions;
extern int gotPremove;
extern GameMode gameMode;
extern int matchMode;
extern int matchGame;
extern int pausing, cmailMsgLoaded, flipView, mute;
extern char white_holding[], black_holding[];
extern int currentMove, backwardMostMove, forwardMostMove;
extern int blackPlaysFirst;
extern FILE *debugFP;
extern char* programVersion;
extern Board boards[];
extern char marker[BOARD_RANKS][BOARD_FILES];
extern char lastMsg[MSG_SIZ];
extern Boolean bookUp;
extern Boolean addToBookFlag;
extern int tinyLayout, smallLayout;
extern Boolean mcMode;
extern int dragging;
extern char variantError[];
extern char lastTalker[];

void MarkMenuItem(char *, int);
char *CmailMsg(void);
/* Tord: Added the useFEN960 parameter in PositionToFEN() below */
char *PositionToFEN(int, char *, int);
void AlphaRank(char *, int); /* [HGM] Shogi move preprocessor */
void EditPositionPasteFEN(char *);
void TimeDelay(long);
void SendMultiLineToICS(char *);
void AnalysisPeriodicEvent(int);
void SetWhiteToPlayEvent(void);
void SetBlackToPlayEvent(void);
void UploadGameEvent(void);
void InitBackEnd1(void);
void InitBackEnd2(void);
int HasPromotionChoice(int, int, int, int, char *, int);
int InPalace(int, int);
int PieceForSquare(int, int);
int OKToStartUserMove(int, int);
void Reset(int, int);
void ResetGameEvent(void);
Boolean HasPattern(const char *, const char *);
Boolean SearchPattern(const char *, const char *);
int LoadGame(FILE *, int, char *, int);
int LoadGameFromFile(char *, int, char *, int);
int CmailLoadGame(FILE *, int, char *, int);
int ReloadGame(int);
int SaveSelected(FILE *, int, char *);
int SaveGame(FILE *, int, char *);
int SaveGameToFile(char *, int);
int LoadPosition(FILE *, int, char *);
int ReloadPosition(int);
int SavePosition(FILE *, int, char *);
int DrawSeekGraph(void);
int SeekGraphClick(ClickType, int, int, int);
void EditPositionEvent(void);
void FlipViewEvent(void);
void MachineWhiteEvent(void);
void MachineBlackEvent(void);
void TwoMachinesEvent(void);
void EditGameEvent(void);
void TrainingEvent(void);
void IcsClientEvent(void);
void ForwardEvent(void);
void BackwardEvent(void);
void ToEndEvent(void);
void ToStartEvent(void);
void ToNrEvent(int);
void RevertEvent(Boolean);
void RetractMoveEvent(void);
void MoveNowEvent(void);
void TruncateGameEvent(void);
void PauseEvent(void);
void CallFlagEvent(void);
void ClockClick(int);
void AcceptEvent(void);
void DeclineEvent(void);
void RematchEvent(void);
void DrawEvent(void);
void AbortEvent(void);
void AdjournEvent(void);
void ResignEvent(void);
void UserAdjudicationEvent(int);
void StopObservingEvent(void);
void StopExaminingEvent(void);
void PonderNextMoveEvent(int);
void ShowThinkingEvent(void);
void PeriodicUpdatesEvent(int);
void HintEvent(void);
void BookEvent(void);
void AboutGameEvent(void);
void ExitEvent(int);
char *DefaultFileName(char *);
ChessMove UserMoveTest(int, int, int, int, int, Boolean);
void UserMoveEvent(int, int, int, int, int);
void DecrementClocks(void);
char *TimeString(long);
void AutoPlayGameLoop(void);
void AdjustClock(Boolean, int);
void DisplayBothClocks(void);
void EditPositionMenuEvent(ChessSquare, int, int);
void DropMenuEvent(ChessSquare, int, int);
int ParseTimeControl(char *, float, int);
void EscapeExpand(char *, char *);
void ProcessICSInitScript(FILE *);
void EditCommentEvent(void);
void ReplaceComment(int, char *);
int ReplaceTags(char *, GameInfo *);/* returns nonzero on error */
void AppendComment(int, char *, Boolean);
void LoadVariation(int, char *);
void ReloadCmailMsgEvent(int);
void MailMoveEvent(void);
void EditTagsEvent(void);
void GetMoveListEvent(void);
void ExitAnalyzeMode(void);
int  AnalyzeModeEvent(void);
void AnalyzeFileEvent(void);
void MatchEvent(int);
void RecentEngineEvent(int);
void TypeInEvent(char);
void TypeInDoneEvent(char *);
void InitPosition(int);
void NewSettingEvent(int, int *, char *, int);
void SettingsMenuIfReady(void);
void DoEcho(void);
void DontEcho(void);
void TidyProgramName(char *, char *, char *);
void SetGameInfo(void);
void AskQuestionEvent(char *, char *, char *, char *);
Boolean ParseOneMove(char *, int, ChessMove *, int *, int *, int *, int *, char *);
char *VariantName(VariantClass);
VariantClass StringToVariant(char *);
double u64ToDouble(u64);
void OutputChatMessage(int, char *);
void EditPositionDone(Boolean);
Boolean GetArgValue(char *);
Boolean LoadPV(int, int);
Boolean LoadMultiPV(int, int, char *, int, int *, int *, int);
void UnLoadPV(void);
void MovePV(int, int, int);
int PromoScroll(int, int);
void EditBookEvent(void);
Boolean DisplayBook(int);
void SaveToBook(char *);
void AddBookMove(char *);
void PlayBookMove(char *, int);
void HoverEvent(int, int, int, int);
int PackGame(Board);
Boolean ParseFEN(Board, int *, char *, Boolean);
void ApplyMove(int, int, int, int, int, Board);
void PackMove(int, int, int, int, ChessSquare);
void ics_printf(char *, ...);
int GetEngineLine(char *, int);
void AddGameToBook(int);
void FlushBook(void);

char *StrStr(char *, char *);
char *StrCaseStr(char *, char *);
char *StrSave(char *);
char *StrSavePtr(char *, char **);
char *SavePart(char *);
char* safeStrCpy(char *, const char *, size_t);

#ifndef _amigados
int StrCaseCmp(char *, char *);
int ToLower(int);
int ToUpper(int);
#else
#define StrCaseCmp Stricmp  /*  Use utility.library functions   */
#include <proto/utility.h>
#endif

extern GameInfo gameInfo;

/* ICS vars used with backend.c and zippy.c */
enum ICS_TYPE { ICS_GENERIC, ICS_ICC, ICS_FICS, ICS_CHESSNET /* not really supported */ };
enum ICS_TYPE ics_type;

/* pgntags.c prototypes
 */
char *PGNTags(GameInfo *);
void PrintPGNTags(FILE *, GameInfo *);
int ParsePGNTag(char *, GameInfo *);
char *PGNResult(ChessMove result);


/* gamelist.c prototypes
 */
/* A game node in the double linked list of games.
 */
typedef struct XB_ListGame {
    ListNode node;
    int number;
    int position;
    int moves;
    unsigned long offset;   /*  Byte offset of game within file.     */
    GameInfo gameInfo;      /*  Note that some entries may be NULL. */
} ListGame;

extern int border;
extern int doubleClick;
extern int storedGames;
extern int opponentKibitzes;
extern ChessSquare gatingPiece;
extern List gameList;
extern int lastLoadGameNumber;
void ClearGameInfo(GameInfo *);
int GameListBuild(FILE *);
void GameListInitGameInfo(GameInfo *);
char *GameListLine(int, GameInfo *);
char * GameListLineFull( int, GameInfo *);
void InitSearch(void);
int GameContainsPosition(FILE *, ListGame *);
void GLT_TagsToList(char *);
void GLT_ParseList(void);
int NamesToList(char *, char **, char **, char *);
int CreateTourney(char *);
char *MakeName(char *);
void SwapEngines(int);
void Substitute(char *, int);

extern char* StripHighlight(char *);  /* returns static data */
extern char* StripHighlightAndTitle(char *);  /* returns static data */
extern void ics_update_width(int);
extern Boolean set_cont_sequence(char *);
extern int wrap(char *, char *, int, int, int *);
int Explode(Board board, int, int, int, int);

typedef enum { CheckBox, ComboBox, TextBox, Button, Spin, ResetButton, SaveButton, ListBox, Graph, PopUp,
		 FileName, PathName, Slider, Message, Fractional, Label, Icon,
		 BoxBegin, BoxEnd, BarBegin, BarEnd, DropDown, Break, EndMark, Skip } Control;

typedef struct XB_OPT {   // [HGM] options: descriptor of UCI-style option
    int value;          // current setting, starts as default
    int min;		// Also used for flags
    int max;
    void *handle;       // for use by front end
    void *target;       // for use by front end
    char *textValue;    // points to beginning of text value in name field
    char **choice;      // points to array of combo choices in cps->combo
    Control type;
    char *name;         // holds both option name and text value (in allocated memory)
    char **font;
} Option;

typedef struct XB_CPS {
    char *which;
    int maybeThinking;
    ProcRef pr;
    InputSourceRef isr;
    char *twoMachinesColor; /* "white\n" or "black\n" */
    char *program;
    char *host;
    char *dir;
    struct XB_CPS *other;
    char *initString;
    char *computerString;
    int sendTime; /* 0=don't, 1=do, 2=test */
    int sendDrawOffers;
    int useSigint;
    int useSigterm;
    int offeredDraw; /* countdown */
    int reuse;
    int useSetboard; /* 0=use "edit"; 1=use "setboard" */
    int extendedEdit;/* 1=also set holdings with "edit" */
    int useSAN;      /* 0=use coordinate notation; 1=use SAN */
    int usePing;     /* 0=not OK to use ping; 1=OK */
    int lastPing;
    int lastPong;
    int usePlayother;/* 0=not OK to use playother; 1=OK */
    int useColors;   /* 0=avoid obsolete white/black commands; 1=use them */
    int useUsermove; /* 0=just send move; 1=send "usermove move" */
    int sendICS;     /* 0=don't use "ics" command; 1=do */
    int sendName;    /* 0=don't use "name" command; 1=do */
    int sdKludge;    /* 0=use "sd DEPTH" command; 1=use "depth\nDEPTH" */
    int stKludge;    /* 0=use "st TIME" command; 1=use "level 1 TIME" */
    int excludeMoves;/* 0=don't use "exclude" command; 1=do */
    char *tidy;
    int matchWins;
    char *variants;
    int analysisSupport;
    int analyzing;
    int protocolVersion;
    int initDone;
    int pseudo;

    /* Added by Tord: */
    int useFEN960;   /* 0=use "KQkq" style FENs, 1=use "HAha" style FENs */
    int useOOCastle; /* 0="O-O" notation for castling, 1="king capture rook" notation */
    /* End of additions by Tord */

    int scoreIsAbsolute; /* [AS] 0=don't know (standard), 1=score is always from white side */
    int isUCI;           /* [AS] 0=no (Winboard), 1=UCI (requires Polyglot) */
    int hasOwnBookUCI;   /* [AS] 0=use GUI or Polyglot book, 1=has own book */

    /* [HGM] time odds */
    float timeOdds; /* factor through which we divide time for this engine  */
    int debug;      /* [HGM] ignore engine debug lines starting with '#'    */
    int maxNrOfSessions; /* [HGM] secondary TC: max args in 'level' command */
    int accumulateTC; /* [HGM] secondary TC: how to handle extra sessions   */
    int drawDepth;    /* [HGM] egbb: search depth to play egbb draws        */
    int nps;          /* [HGM] nps: factor for node count to replace time   */
    int supportsNPS;
    int alphaRank;    /* [HGM] shogi: engine uses shogi-type coordinates    */
    int maxCores;     /* [HGM] SMP: engine understands cores command        */
    int memSize;      /* [HGM] memsize: engine understands memory command   */
    char *egtFormats; /* [HGM] EGT: supported tablebase formats             */
    int bookSuspend;  /* [HGM] book: go was deferred because of book hit    */
    int pause;        /* [HGM] pause: 1=supports it, 2=actually paused      */
    int highlight;    /* [HGM] engine wants to get lift and put commands    */
    int nrOptions;    /* [HGM] options: remembered option="..." features    */
#define MAX_OPTIONS 200
    Option option[MAX_OPTIONS];
    int comboCnt;
    char *comboList[20*MAX_OPTIONS];
    char *optionSettings;
    void *programLogo; /* [HGM] logo: bitmap of the logo                    */
    char *fenOverride; /* [HGM} FRC: force FEN casling & ep fields by hand  */
    char userError;    /* [HGM] crash: flag to suppress fatal-error messages*/
    char reload;       /* [HGM] options: flag to resend options with xreuse */
} ChessProgramState;

extern ChessProgramState first, second;

/* Search stats from chessprogram */
typedef struct {
  char movelist[2*MSG_SIZ]; /* Last PV we were sent */
  int depth;              /* Current search depth */
  int nr_moves;           /* Total nr of root moves */
  int moves_left;         /* Moves remaining to be searched */
  char move_name[MOVE_LEN];  /* Current move being searched, if provided */
  u64 nodes;    /* # of nodes searched */
  int time;               /* Search time (centiseconds) */
  int score;              /* Score (centipawns) */
  int got_only_move;      /* If last msg was "(only move)" */
  int got_fail;           /* 0 - nothing, 1 - got "--", 2 - got "++" */
  int ok_to_send;         /* handshaking between send & recv */
  int line_is_book;       /* 1 if movelist is book moves */
  int seen_stat;          /* 1 if we've seen the stat01: line */
} ChessProgramStats;

extern ChessProgramStats_Move pvInfoList[MAX_MOVES];
extern Boolean shuffleOpenings;
extern ChessProgramStats programStats;
extern int opponentKibitzes; // used by wengineo.c
extern int errorExitStatus;
extern char *recentEngines;
extern char *comboLine;
extern Boolean partnerUp, twoBoards;
extern char engineVariant[];
char *EngineDefinedVariant(ChessProgramState *, int);
void SettingsPopUp(ChessProgramState *); // [HGM] really in front-end, but CPS not known in frontend.h
int WaitForEngine(ChessProgramState *, DelayedEventCallback);
void Load(ChessProgramState *, int);
int MultiPV(ChessProgramState *);
void MoveHistorySet(char movelist[][2*MOVE_LEN], int, int, int, ChessProgramStats_Move *);
void MakeEngineOutputTitle(void);
void LoadTheme(void);
void CreateBookEvent(void);
char *SupportedVariant(char *, VariantClass, int, int, int, int, char *);
char *CollectPieceDescriptors(void);


/* A point in time */
typedef struct {
    long sec;  /* Assuming this is >= 32 bits */
    int ms;    /* Assuming this is >= 16 bits */
} TimeMark;

extern TimeMark programStartTime;

void GetTimeMark(TimeMark *);
long SubtractTimeMarks(TimeMark *, TimeMark *);

#endif /* XB_BACKEND */
