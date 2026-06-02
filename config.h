/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 1;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartgaps                 = 1;  /* 1 means no outer gap when there is only one window */
static int gaps                            = 1;  /* 1 means gaps between windows are added */
static const unsigned int gappx            = 4; /* gap pixel between windows */
static const unsigned int borderpx         = 2;  /* border pixel of windows */
static const float rootcolor[]             = COLOR(0x222222ff);
static const float bordercolor[]           = COLOR(0x1d2021ff);
static const float focuscolor[]            = COLOR(0x3c3836ff);
static const float urgentcolor[]           = COLOR(0xff0000ff);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.0f, 0.0f, 0.0f, 1.0f}; /* You can also use glsl colors */
static const int respect_monitor_reserved_area = 0;  /* 1 to monitor center while respecting the monitor's reserved area, 0 to monitor center */

enum {
	WINDOW,
    LAYOUT,
    MUSIC,
    VIEW
};
const char *modes_labels[] = {
	"WINDOW",
    "LAYOUT",
    "MUSIC",
    "VIEW"
};

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (21)

/* logging */
static int log_level = WLR_ERROR;

/* Autostart */
static const char *const autostart[] = {
        "/usr/bin/kanshi", NULL,
        //"/home/klim/workspace/projects/dwl/Waybar/build/waybar", "-c", "/home/klim/.config/waybar/config.dwl", NULL,
        "/home/klim/Scripts/launch-waybar.sh", "/home/klim/.config/waybar/config.dwl", NULL,
        "/usr/bin/swaybg", "-i", "/home/klim/Wallpapers/3200x1800/3035.png", NULL,
        "/usr/bin/gammastep-indicator", NULL,
        "/usr/bin/wl-paste", "--type", "text", "--watch", "cliphist store", NULL,
        "/usr/bin/wl-paste", "--type", "image", "--watch", "cliphist store", NULL,
        "/usr/bin/blueman-applet", NULL,
        "/usr/bin/nm-applet", NULL,
        "/usr/bin/enpass", NULL,
        "/usr/bin/easyeffects", "--gapplication-service", NULL,
        "/usr/bin/dunst", "-conf", "/home/klim/.config/dunst/dunstrc", NULL,
        "/usr/bin/hypridle", "-c", "/home/klim/.config/hypr/hypridle-dwl.conf", NULL,
        NULL /* terminate */
};

/* window resizing */
/* resize_corner:
 * 0: top-left
 * 1: top-right
 * 2: bottom-left
 * 3: bottom-right
 * 4: closest to the cursor
 */
static const int resize_corner = 4;
static const int warp_cursor = 1;	/* 1: warp to corner, 0: don’t warp */
static const int lock_cursor = 0;	/* 1: lock cursor, 0: don't lock */

static const Rule rules[] = {
	/* app_id             title       tags mask     isfloating   monitor   x   y   width   height */
	//{ "chrome-gemini.google.com__-Default", NULL,    0,            1,           -1,       -1, -1, 0.7,   0.8, 'l' }, /* Start on currently visible tags floating, not tiled */
    { "chrome-gemini.google.com__-Default", NULL,  0,       1,           -1,       -1, -1, 0.70, 0.80, 'l'  },
    { "Enpass", "Enpass",  0,       1,           -1,       -1, -1, 0.70, 0.80, 'e'  },
    { "Enpass", "Enpass Assistant",  0,       1,           -1,       -1, -1, -1, -1, 'a'  },
    /* default/example rule: can be changed but cannot be eliminated; at least one rule must exist */
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[E]",      deck },
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients due to
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899 */
static const MonitorRule monrules[] = {
   /* name        mfact  nmaster scale layout       rotate/reflect                x    y
    * example of a HiDPI laptop monitor:
    { "eDP-1",    0.5f,  1,      2,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 }, */
	{ NULL,       0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	/* default monitor rule: can be changed but cannot be eliminated; at least one monitor rule must exist */
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	.options = NULL,
};

static const int repeat_rate = 25;
static const int repeat_delay = 600;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
static const int hide_cursor_when_typing = 1;
static const int toplevel_focus_view_and_client = 1;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = { "/usr/bin/alacritty", NULL };
static const char *menucmd[] = { "/usr/bin/rofi", "-show", "run", NULL };
static const char *windowcmd[] = { "/usr/bin/rofi", "-show", "window", NULL };
static const char *exitcmd[] = { "/home/klim/Scripts/rofi-exit.sh", NULL };
static const char *vpncmd[] =  { "/home/klim/Scripts/rofi-vpn.sh", NULL };
static const char *screenshotcmd[] = { "/home/klim/Scripts/screenshot-wayland.sh", NULL };

/* named scratchpads - First arg only serves to match against key in rules*/
//static const char *llmscratchpadcmd[] = { "l", "/usr/bin/chromium", "--app=https://gemini.google.com", "--user-data-dir=/home/klim/.chromium-gemini", NULL };

static const char *llmscratchpadcmd[] = { 
    "l",
    "/usr/bin/chromium", 
    "--app=https://gemini.google.com", 
    "--user-data-dir=/home/klim/.chromium-gemini", 
    NULL 
};

static const char *pwscratchpadcmd[] = { 
    "e",
    "/usr/bin/enpass", 
    NULL 
};

static const Key keys[] = {
	/* Note that Shift changes certain key codes: 2 -> at, etc. */
	/* modifier                  key                  function          argument */
	{ MODKEY,                    XKB_KEY_r,           spawn,            {.v = menucmd} },
	{ MODKEY,                    XKB_KEY_t,           spawn,            {.v = windowcmd} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_Return,      spawn,            {.v = termcmd} },
	{ MODKEY,                    XKB_KEY_s,           spawn,            {.v = screenshotcmd } },
	{ MODKEY,                    XKB_KEY_h,           focusdir,         {.ui = 0} },
	{ MODKEY,                    XKB_KEY_l,           focusdir,         {.ui = 1} },
	{ MODKEY,                    XKB_KEY_k,           focusdir,         {.ui = 2} },
	{ MODKEY,                    XKB_KEY_j,           focusdir,         {.ui = 3} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_h,           swapdir,          {.ui = 0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_l,           swapdir,          {.ui = 1} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_k,           swapdir,          {.ui = 2} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_j,           swapdir,          {.ui = 3} },
	{ MODKEY,                    XKB_KEY_c,           focusstack,       {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_c,           focusstack,       {.i = -1} },
	{ MODKEY,                    XKB_KEY_o,           focusstackclients,       {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_o,           focusstackclients,       {.i = -1} },
	//{ MODKEY,                    XKB_KEY_i,           incnmaster,       {.i = +1} },
	//{ MODKEY,                    XKB_KEY_d,           incnmaster,       {.i = -1} },
	//{ MODKEY,                    XKB_KEY_h,           setmfact,         {.f = -0.05f} },
	//{ MODKEY,                    XKB_KEY_l,           setmfact,         {.f = +0.05f} },
	{ MODKEY,                    XKB_KEY_Return,      zoom,             {0} },
	{ MODKEY,                    XKB_KEY_Tab,         view,             {0} },
	//{ MODKEY,                    XKB_KEY_g,           togglegaps,       {0} },
    { MODKEY,                    XKB_KEY_space,       togglescratch,    {.v = pwscratchpadcmd } },
    { MODKEY,                    XKB_KEY_BackSpace,   togglescratch,    {.v = llmscratchpadcmd } },
	{ MODKEY,                    XKB_KEY_q,           killclient,       {0} },
	//{ MODKEY,                    XKB_KEY_0,           view,             {.ui = ~0} },
	//{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_parenright,  tag,              {.ui = ~0} },
	{ MODKEY,                    XKB_KEY_space,       focusmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,                    XKB_KEY_onehalf,     focusmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_less,        tagmon,           {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_greater,     tagmon,           {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_Escape,      spawn,            {.v = exitcmd } },
	{ MODKEY,                    XKB_KEY_v,           spawn,            {.v = vpncmd } },
	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                        0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_quotedbl,                      1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                    2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_currency,                      3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                       4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_ampersand,                     5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_slash,                         6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_parenleft,                     7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenright,                    8),
	TAGKEYS(          XKB_KEY_0, XKB_KEY_equal,                         9),
	TAGKEYS(          XKB_KEY_n, XKB_KEY_N,                             10),
	TAGKEYS(          XKB_KEY_i, XKB_KEY_I,                             11),
	TAGKEYS(          XKB_KEY_m, XKB_KEY_M,                             12),
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_q,           quit,             {0} },


    { MODKEY, XKB_KEY_w, entermode, {.i = WINDOW } },
    { MODKEY, XKB_KEY_d, entermode, {.i = VIEW } },
    { MODKEY, XKB_KEY_g, entermode, {.i = LAYOUT } },
    { MODKEY, XKB_KEY_p, entermode, {.i = MUSIC  } },

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server, quit, {0} },
	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Modekey modekeys[] = {
	/* mode      modifier                  key                 function        argument */
	{ WINDOW, { 0,                        XKB_KEY_f,           togglefloating,   {0} } },
	{ WINDOW, { 0,                        XKB_KEY_f,           entermode,        {.i = NORMAL} } },
	{ WINDOW, { 0,                        XKB_KEY_t,           settiling,        {0} } },
	{ WINDOW, { 0,                        XKB_KEY_t,           entermode,        {.i = NORMAL} } },
	{ WINDOW, { WLR_MODIFIER_SHIFT,       XKB_KEY_f,           togglefullscreen, {0}  } },
	{ WINDOW, { WLR_MODIFIER_SHIFT,       XKB_KEY_f,           entermode,        {.i = NORMAL} } },
	{ WINDOW, { 0,                        XKB_KEY_i,           incnmaster,       {.i = +1 }  } },
	{ WINDOW, { 0,                        XKB_KEY_i,           entermode,        {.i = NORMAL} } },
	{ WINDOW, { 0,                        XKB_KEY_d,           incnmaster,       {.i = -1 }  } },
	{ WINDOW, { 0,                        XKB_KEY_d,           entermode,        {.i = NORMAL} } },
	{ WINDOW, { 0,                        XKB_KEY_Escape,      entermode,        {.i = NORMAL} } },


	{ LAYOUT, { 0, XKB_KEY_n, nextlayout, {0} } },
	{ LAYOUT, { 0, XKB_KEY_n, entermode, {.i = NORMAL} } },
	{ LAYOUT, { 0, XKB_KEY_t, setlayout, {.v = &layouts[0]} } },
	{ LAYOUT, { 0, XKB_KEY_t, entermode, {.i = NORMAL} } },
	{ LAYOUT, { 0, XKB_KEY_f, setlayout, {.v = &layouts[1]} } },
	{ LAYOUT, { 0, XKB_KEY_f, entermode, {.i = NORMAL} } },
	{ LAYOUT, { 0, XKB_KEY_m, setlayout, {.v = &layouts[2]} } },
	{ LAYOUT, { 0, XKB_KEY_m, entermode, {.i = NORMAL} } },
	{ LAYOUT, { 0, XKB_KEY_d, setlayout, {.v = &layouts[3]} } },
	{ LAYOUT, { 0, XKB_KEY_d, entermode, {.i = NORMAL} } },
	{ LAYOUT, { 0, XKB_KEY_Escape, entermode, {.i = NORMAL} } },


	{ MUSIC, { 0, XKB_KEY_p, spawn, SHCMD("playerctl play-pause") } },
	{ MUSIC, { 0, XKB_KEY_p, entermode, {.i = NORMAL} } },
	{ MUSIC, { 0, XKB_KEY_p, spawn, SHCMD("playerctl next") } },
	{ MUSIC, { 0, XKB_KEY_n, entermode, {.i = NORMAL} } },
	{ MUSIC, { 0, XKB_KEY_p, spawn, SHCMD("playerctl previous") } },
	{ MUSIC, { 0, XKB_KEY_p, entermode, {.i = NORMAL} } },
	{ MUSIC, { 0, XKB_KEY_Escape, entermode, {.i = NORMAL} } },


	{ VIEW, { 0,  XKB_KEY_h,      focusmon,           {.i = WLR_DIRECTION_LEFT} } },
	{ VIEW, { 0,  XKB_KEY_h,      entermode,          {.i = NORMAL} } },
	{ VIEW, { 0,  XKB_KEY_l,      focusmon,           {.i = WLR_DIRECTION_RIGHT} } },
	{ VIEW, { 0,  XKB_KEY_l,      entermode,          {.i = NORMAL} } },
	{ VIEW, { WLR_MODIFIER_SHIFT,  XKB_KEY_h,      tagmon,           {.i = WLR_DIRECTION_LEFT} } },
	{ VIEW, { WLR_MODIFIER_SHIFT,  XKB_KEY_h,      entermode,        {.i = NORMAL} } },
	{ VIEW, { WLR_MODIFIER_SHIFT,  XKB_KEY_l,      tagmon,           {.i = WLR_DIRECTION_RIGHT} } },
	{ VIEW, { WLR_MODIFIER_SHIFT,  XKB_KEY_l,      entermode,        {.i = NORMAL} } },
	{ VIEW, { 0,  XKB_KEY_l,      entermode,        {.i = NORMAL} } },
	{ VIEW, { 0,  XKB_KEY_Escape, entermode,        {.i = NORMAL} } }
};

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
};
