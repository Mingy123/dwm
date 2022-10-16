#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;   /* border pixel of windows */
static const unsigned int gappx     = 3;   /* gap pixel between windows */
static const unsigned int snap      = 32;  /* snap pixel */
static const int showbar            = 1;   /* 0 means no bar */
static const int topbar             = 0;   /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=16" , "fontawesome:size=16" };
static const char dmenufont[]       = "monospace:size=12";
static const char col_gray1[]       = "#333f48";
static const char col_gray2[]       = "#111111";
static const char col_gray3[]       = "#00a499";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_gray1,  col_cyan  },
};

/* fade inactive */
static const double activeopacity   = 1.0f;     /* Window opacity when it's focused (0 <= opacity <= 1) */
static const double inactiveopacity = 0.90f;   /* Window opacity when it's inactive (0 <= opacity <= 1) */
static Bool bUseOpacity = True;     /* Starts with opacity on any unfocused windows */

/* tagging */
// fontawesome 6.2: f8ab -> cassette (tag 9)
static const char *tags[] = { "\uf6ad", "\uf1a0", "\uf630", "\uf126", "\uf108", "\uf008", "\uf233", "\uf120", "\uf8ab" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Pcmanfm",  NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
    { "com.example.mingy.Intro", NULL, NULL, 0,         1,           -1 },
    { NULL,       NULL,      "hexpicker", 0,            1,           -1 }
};

/* layout(s) */
static const float mfact     = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static int fakefullscreen = 0; // 1 will make windows fullscreen into their tiles

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "< >",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, \
    { MODKEY|ALTKEY,                KEY,      swaptags,       {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char* dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char* termcmd[] = {"urxvt", NULL};

static const char* lowerBri[] = {"/scripts/bri_down.sh", NULL};
static const char* raiseBri[] = {"/scripts/bri_up.sh", NULL};
static const char* lowerRed[] = {"/scripts/red-down.sh", NULL};
static const char* raiseRed[] = {"/scripts/red-up.sh", NULL};


#include "movestack.c"
#include "shiftview.c"
static Key keys[] = {
	/* modifier                 key             function        argument */
	{ MODKEY,                   XK_p,           spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,         XK_Return,      spawn,          {.v = termcmd} },
	{ MODKEY,                   XK_j,           focusstack,     {.i = +1 } },
	{ MODKEY,                   XK_k,           focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_j,           movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_k,           movestack,      {.i = -1 } },
	{ MODKEY,                   XK_h,           setmfact,       {.f = -0.05} },
	{ MODKEY,                   XK_l,           setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,         XK_f,           togglelockfs,   {0} },
	{ MODKEY|ShiftMask,         XK_o,           togglefakefs,   {0} },
	{ MODKEY,                   XK_Return,      zoom,           {0} },
//	{ MODKEY,                   XK_Tab,         view,           {0} },
	{ MODKEY,                   XK_Tab,         focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_c,           killclient,     {0} },
	{ MODKEY,                   XK_q,           killclient,     {0} },
	{ MODKEY,                   XK_t,           setlayout,      {.v = &layouts[0]} }, // tile
	{ MODKEY,                   XK_f,           setlayout,      {.v = &layouts[1]} }, // floating
	{ MODKEY,                   XK_m,           setlayout,      {.v = &layouts[2]} }, // monocle
	{ MODKEY,                   XK_space,       togglebar,      {0} },
	{ MODKEY|ShiftMask,         XK_space,       setlayout,      {0} },
	{ MODKEY,                   XK_0,           view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,         XK_0,           tag,            {.ui = ~0 } },
	{ MODKEY,                   XK_period,      shiftview,      {.i = +1 } },
	{ MODKEY,                   XK_comma,       shiftview,      {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_period,      incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_comma,       incnmaster,     {.i = +1 } },
	{ MODKEY,                   XK_o,           toggleopacity,  {0} },
	{ MODKEY|ShiftMask,         XK_BackSpace,   maketiled,      {0} },

	{ 0,                        XK_Print,       spawn,          SHCMD("scrot -o ~/Downloads/scrot.png") },
	{ ShiftMask,                XK_Print,       spawn,          SHCMD("scrot -u -o ~/Downloads/scrot.png") },
	{ MODKEY|ShiftMask,         XK_s,           spawn,          SHCMD("scrot -s -i -f -o ~/Downloads/scrot.png") },

	{ MODKEY|ShiftMask,         XK_h,           spawn,          SHCMD("hexpicker") },
	{ MODKEY,                   XK_b,           spawn,          SHCMD("brave &> /dev/null") },
	{ MODKEY,                   XK_e,           spawn,          SHCMD("pcmanfm") },
	{ ControlMask|ShiftMask,    XK_l,           spawn,          SHCMD("betterlockscreen -l dim") },
	{ MODKEY|ShiftMask,         XK_l,           spawn,          SHCMD("loginctl suspend") },

	{ 0,           XF86XK_AudioLowerVolume,     spawn,          SHCMD("amixer set Master 2%- unmute > /dev/null") },
	{ 0,           XF86XK_AudioRaiseVolume,     spawn,          SHCMD("amixer set Master 2%+ unmute > /dev/null") },
	{ 0,           XF86XK_AudioMute,            spawn,          SHCMD("amixer set Master toggle > /dev/null") },
	{ 0,           XF86XK_MonBrightnessUp,      spawn,          {.v = raiseBri} },
	{ 0,           XF86XK_MonBrightnessDown,    spawn,          {.v = lowerBri} },
	{ ShiftMask,                XK_F3,          spawn,          {.v = raiseRed} },
	{ ShiftMask,                XK_F2,          spawn,          {.v = lowerRed} },
	{ ShiftMask,                XK_F6,          spawn,          SHCMD("killall mpv") },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

