// ==============================================================
//   ZooDownloader v1.0  --  By iTsTimeCoder
//   Full Multi-Platform GUI Downloader  (yt-dlp + ffmpeg)
//   Supports: YouTube, TikTok, Instagram, Kick, Twitch,
//             Twitter/X, Facebook, Reddit, SoundCloud, Vimeo,
//             Bilibili, Rumble, Bandcamp, and 1000+ more sites.
// ==============================================================

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ==============================================================
//   DARK PURPLE COLOR PALETTE
// ==============================================================
#define CLR_BG       RGB(13, 13, 22)
#define CLR_SURFACE  RGB(22, 22, 38)
#define CLR_SURF2    RGB(32, 32, 52)
#define CLR_ACCENT   RGB(108, 52, 215)
#define CLR_TEXT     RGB(215, 215, 230)
#define CLR_TEXT2    RGB(130, 130, 162)
#define CLR_GREEN    RGB(65,  195,  95)
#define CLR_ORANGE   RGB(230, 150,  50)
#define CLR_RED      RGB(230,  62,  62)
#define CLR_BORDER   RGB(52,  52,  80)

// ==============================================================
//   CONTROL IDs
// ==============================================================
#define IDC_TAB              200
#define IDC_URL_INPUT        201
#define IDC_PATH_INPUT       202
#define IDC_BTN_BROWSE       203
#define IDC_BTN_DOWNLOAD     204
#define IDC_BTN_STOP         205
#define IDC_BTN_UPDATE       206
#define IDC_BTN_CLEAR_LOG    207
#define IDC_BTN_OPEN_DEST    208
#define IDC_RAD_AUTO         209
#define IDC_RAD_AUDIO        210
#define IDC_RAD_VIDEO        211
#define IDC_CMB_AUDIO_FMT    212
#define IDC_CMB_VIDEO_FMT    213
#define IDC_CMB_VIDEO_QUAL   214
#define IDC_CHK_THUMB        215
#define IDC_CHK_META         216
#define IDC_CHK_RESTRICT     217
#define IDC_CHK_PLAYLIST     218
#define IDC_CMB_COOKIES      219
#define IDC_LOG_OUTPUT       220
#define IDC_PROGRESS         221
#define IDC_CHK_SUBS         222
#define IDC_CHK_EMBED_SUBS   223
#define IDC_CMB_SUBS_LANG    224
#define IDC_CMB_SUB_FMT      225
#define IDC_CHK_CHAPTERS     226
#define IDC_CHK_SPONSORBLOCK 227
#define IDC_EDT_PROXY        228
#define IDC_EDT_RATELIMIT    229
#define IDC_EDT_CONCURRENT   230
#define IDC_EDT_CUSTOM_ARGS  231
#define IDC_CHK_WRITE_THUMB  232
#define IDC_CHK_WRITE_INFO   233
#define IDC_CHK_NO_PART      234
#define IDC_CHK_KEEP_FRAG    235
#define IDC_CHK_AGE_BYPASS   236
#define IDC_LBL_PLATFORM     237
#define IDC_CHK_ARIA2        238
#define IDC_EDT_FILENAME     239
#define IDC_CHK_NO_OVERWRITE 240
#define IDC_CHK_SUBTITLES_AUTO 241
#define IDC_CMB_SPEED_PRESET 242
#define IDC_CHK_GEO_BYPASS   243

// ==============================================================
//   GLOBALS
// ==============================================================
static HINSTANCE g_hInst;
static HWND      g_hwnd;
static HWND      g_hTab;
static HWND      g_hPanels[4]; // Download | Advanced | Options | About

// -- Download Panel --
static HWND g_hUrl, g_hPath;
static HWND g_hRadAuto, g_hRadAudio, g_hRadVideo;
static HWND g_hAudFmt, g_hVidFmt, g_hVidQual;
static HWND g_hLblAudFmt, g_hLblVidFmt, g_hLblVidQual;
static HWND g_hThumb, g_hMeta, g_hRestrict, g_hPlaylist, g_hCookies;
static HWND g_hLblPlatform;

// -- Advanced Panel --
static HWND g_hSubs, g_hEmbedSubs, g_hSubsLang, g_hSubFmt, g_hSubsAuto;
static HWND g_hChapters, g_hSponsorBlock, g_hAgeBypass, g_hGeoBypass;
static HWND g_hWriteThumb, g_hWriteInfo, g_hNoPart, g_hKeepFrag, g_hNoOverwrite;
static HWND g_hProxy, g_hRateLimit, g_hConcurrent, g_hCustomArgs;
static HWND g_hAria2, g_hFilenameTmpl;

// -- Log / Progress --
static HWND g_hLog, g_hProgress;

// -- Brushes / Fonts --
static HBRUSH g_brBg, g_brSurf, g_brSurf2;
static HFONT  g_fUI, g_fBold, g_fSmall, g_fMono, g_fTitle, g_fLarge;

// ==============================================================
//   FORWARD DECLARATIONS
// ==============================================================
void  CreateFonts();
void  CreateDownloadPanel();
void  CreateAdvancedPanel();
void  CreateAboutPanel();
void  ShowTab(int idx);
void  BuildAndRun(HWND hwnd);
void  AppendLog(const std::string& s);
void  UpdateTypeControls();

std::string GetWndText(HWND hw);
std::string GetComboSel(HWND hw);
std::string DetectPlatform(const std::string& url);

HWND MkLabel(HWND p, const char* t, int x, int y, int w, int h, HFONT f = nullptr);
HWND MkEdit(HWND p, const char* def, int x, int y, int w, int h, int id, DWORD ex = 0);
HWND MkMultiEdit(HWND p, int x, int y, int w, int h, int id);
HWND MkCheck(HWND p, const char* t, int x, int y, int w, int h, int id, bool chk = false);
HWND MkRadio(HWND p, const char* t, int x, int y, int w, int h, int id, DWORD ex = 0);
HWND MkCombo(HWND p, int x, int y, int w, int h, int id);
HWND MkBtn  (HWND p, const char* t, int x, int y, int w, int h, int id, DWORD ex = 0);
void ComboFill(HWND hw, const char* items[], int n, int sel = 0);

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// ==============================================================
//   UTILITIES
// ==============================================================
static inline void SF(HWND hw, HFONT f) {
    SendMessage(hw, WM_SETFONT, (WPARAM)(f ? f : g_fUI), TRUE);
}

std::string GetWndText(HWND hw) {
    int n = GetWindowTextLengthA(hw);
    if (!n) return "";
    std::string s(n + 1, 0);
    GetWindowTextA(hw, &s[0], n + 1);
    s.resize(n);
    return s;
}

std::string GetComboSel(HWND hw) {
    int i = (int)SendMessage(hw, CB_GETCURSEL, 0, 0);
    if (i == CB_ERR) return "";
    char b[512] = {};
    SendMessageA(hw, CB_GETLBTEXT, i, (LPARAM)b);
    return b;
}

void AppendLog(const std::string& s) {
    if (!g_hLog) return;
    int n = GetWindowTextLengthA(g_hLog);
    SendMessageA(g_hLog, EM_SETSEL, n, n);
    SendMessageA(g_hLog, EM_REPLACESEL, FALSE, (LPARAM)(s + "\r\n").c_str());
    SendMessageA(g_hLog, EM_SCROLL, SB_BOTTOM, 0);
}

std::string DetectPlatform(const std::string& url) {
    struct { const char* k; const char* n; } T[] = {
        {"youtu.be",        "YouTube (Short Link)"},
        {"youtube.com",     "YouTube"},
        {"tiktok.com",      "TikTok"},
        {"vm.tiktok.com",   "TikTok (Short)"},
        {"instagram.com",   "Instagram"},
        {"kick.com",        "Kick"},
        {"twitch.tv",       "Twitch"},
        {"clips.twitch.tv", "Twitch Clip"},
        {"twitter.com",     "Twitter / X"},
        {"x.com",           "Twitter / X"},
        {"facebook.com",    "Facebook"},
        {"fb.watch",        "Facebook (Short)"},
        {"reddit.com",      "Reddit"},
        {"v.redd.it",       "Reddit Video"},
        {"soundcloud.com",  "SoundCloud"},
        {"spotify.com",     "Spotify"},
        {"dailymotion.com", "Dailymotion"},
        {"vimeo.com",       "Vimeo"},
        {"bilibili.com",    "Bilibili"},
        {"rumble.com",      "Rumble"},
        {"pinterest.com",   "Pinterest"},
        {"linkedin.com",    "LinkedIn"},
        {"snapchat.com",    "Snapchat"},
        {"odysee.com",      "Odysee (LBRY)"},
        {"trovo.live",      "Trovo"},
        {"streamable.com",  "Streamable"},
        {"bandcamp.com",    "Bandcamp"},
        {"mixcloud.com",    "Mixcloud"},
        {"nicovideo.jp",    "NicoNico"},
        {"niconico.jp",     "NicoNico"},
        {"aparat.com",      "Aparat"},
        {"t.me",            "Telegram"},
        {"threads.net",     "Threads"},
        {"bbc.co.uk",       "BBC"},
        {"cnn.com",         "CNN"},
        {"twimg.com",       "Twitter Media"},
        {"crunchyroll.com", "Crunchyroll"},
        {"funimation.com",  "Funimation"},
        {"naver.com",       "Naver TV"},
        {"weibo.com",       "Weibo"},
        {"coub.com",        "Coub"},
        {"gfycat.com",      "Gfycat"},
        {"liveleak.com",    "LiveLeak"},
        {"brighteon.com",   "Brighteon"},
        {nullptr, nullptr}
    };
    for (int i = 0; T[i].k; i++)
        if (url.find(T[i].k) != std::string::npos) return T[i].n;
    return url.find("http") != std::string::npos ? "Auto-Detect (yt-dlp)" : "---";
}

bool ExtractResource(int id, const char* path) {
    HRSRC hr = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
    if (!hr) return false;
    HGLOBAL hg = LoadResource(NULL, hr);
    DWORD   sz = SizeofResource(NULL, hr);
    void*   pd = LockResource(hg);
    std::ofstream f(path, std::ios::binary);
    if (!f.good()) return false;
    f.write((char*)pd, sz);
    return true;
}

// ==============================================================
//   FONT CREATION
// ==============================================================
void CreateFonts() {
    auto F = [](int h, int w, const char* face) {
        return CreateFontA(h,0,0,0,w,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,face);
    };
    g_fUI    = F(15, FW_NORMAL, "Segoe UI");
    g_fBold  = F(15, FW_BOLD,   "Segoe UI");
    g_fSmall = F(13, FW_NORMAL, "Segoe UI");
    g_fMono  = F(13, FW_NORMAL, "Consolas");
    g_fTitle = F(24, FW_BOLD,   "Segoe UI");
    g_fLarge = F(18, FW_BOLD,   "Segoe UI");
}

// ==============================================================
//   WIDGET FACTORIES
// ==============================================================
HWND MkLabel(HWND p, const char* t, int x, int y, int w, int h, HFONT f) {
    HWND hw = CreateWindowExA(0,"STATIC",t,WS_VISIBLE|WS_CHILD,
        x,y,w,h,p,NULL,g_hInst,NULL);
    SF(hw, f); return hw;
}
HWND MkEdit(HWND p, const char* def, int x, int y, int w, int h, int id, DWORD ex) {
    HWND hw = CreateWindowExA(WS_EX_CLIENTEDGE,"EDIT",def,
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|ex,
        x,y,w,h,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, nullptr); return hw;
}
HWND MkMultiEdit(HWND p, int x, int y, int w, int h, int id) {
    HWND hw = CreateWindowExA(WS_EX_CLIENTEDGE,"EDIT","",
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_VSCROLL|
        ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
        x,y,w,h,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, nullptr); return hw;
}
HWND MkCheck(HWND p, const char* t, int x, int y, int w, int h, int id, bool chk) {
    HWND hw = CreateWindowExA(0,"BUTTON",t,
        WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|WS_TABSTOP,
        x,y,w,h,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, nullptr);
    if (chk) SendMessage(hw, BM_SETCHECK, BST_CHECKED, 0);
    return hw;
}
HWND MkRadio(HWND p, const char* t, int x, int y, int w, int h, int id, DWORD ex) {
    HWND hw = CreateWindowExA(0,"BUTTON",t,
        WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON|WS_TABSTOP|ex,
        x,y,w,h,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, nullptr); return hw;
}
HWND MkCombo(HWND p, int x, int y, int w, int h, int id) {
    HWND hw = CreateWindowExA(0,"COMBOBOX","",
        WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST|WS_TABSTOP,
        x,y,w,h+200,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, nullptr); return hw;
}
HWND MkBtn(HWND p, const char* t, int x, int y, int w, int h, int id, DWORD ex) {
    HWND hw = CreateWindowExA(0,"BUTTON",t,
        WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|WS_TABSTOP|ex,
        x,y,w,h,p,(HMENU)(INT_PTR)id,g_hInst,NULL);
    SF(hw, g_fBold); return hw;
}
void ComboFill(HWND hw, const char* items[], int n, int sel) {
    for (int i = 0; i < n; i++)
        SendMessageA(hw, CB_ADDSTRING, 0, (LPARAM)items[i]);
    SendMessage(hw, CB_SETCURSEL, sel, 0);
}

// ==============================================================
//   PANEL WINDOW PROC  (handles dark theme for panel children)
// ==============================================================
LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND: {
        RECT rc; GetClientRect(hwnd, &rc);
        FillRect((HDC)wp, &rc, g_brSurf);
        return 1;
    }
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX: {
        SetTextColor((HDC)wp, CLR_TEXT);
        SetBkColor((HDC)wp, CLR_SURFACE);
        return (LRESULT)g_brSurf;
    }
    case WM_CTLCOLORBTN: {
        SetTextColor((HDC)wp, CLR_TEXT);
        SetBkColor((HDC)wp, CLR_SURF2);
        return (LRESULT)g_brSurf2;
    }
    case WM_COMMAND:
        // Forward to main window so button clicks work
        SendMessage(g_hwnd, WM_COMMAND, wp, lp);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// ==============================================================
//   UPDATE TYPE CONTROLS  (show/hide based on radio selection)
// ==============================================================
void UpdateTypeControls() {
    bool isAud = (SendMessage(g_hRadAudio, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool isVid = (SendMessage(g_hRadVideo, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Audio-only controls
    ShowWindow(g_hLblAudFmt, isAud ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hAudFmt,    isAud ? SW_SHOW : SW_HIDE);

    // Video/Auto quality controls
    ShowWindow(g_hLblVidQual, !isAud ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hVidQual,    !isAud ? SW_SHOW : SW_HIDE);

    // Video container format (only for explicit Video mode)
    ShowWindow(g_hLblVidFmt, isVid ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hVidFmt,    isVid ? SW_SHOW : SW_HIDE);
}

// ==============================================================
//   PANEL 0  —  DOWNLOAD
// ==============================================================
void CreateDownloadPanel() {
    HWND p = g_hPanels[0];

    // ---- URL Input ----
    MkLabel(p, "URL(s)  —  paste one or more links, one per line:", 10,10, 540,18, g_fBold);
    g_hUrl = MkMultiEdit(p, 10, 32, 728, 68, IDC_URL_INPUT);

    // Platform auto-detect label
    g_hLblPlatform = MkLabel(p, "  Detected Platform:  ---", 10, 108, 550, 18);

    // ---- Save Destination ----
    MkLabel(p, "Save Destination:", 10, 135, 130, 18, g_fBold);
    g_hPath = MkEdit(p, "E:\\Downloads", 10, 155, 598, 26, IDC_PATH_INPUT);
    MkBtn(p, "Browse...",   613, 154, 115, 27, IDC_BTN_BROWSE);
    MkBtn(p, "Open Folder", 613, 184, 115, 26, IDC_BTN_OPEN_DEST);

    // ---- Download Type ----
    MkLabel(p, "Type:", 10, 222, 45, 18, g_fBold);
    g_hRadAuto  = MkRadio(p, "Auto (Best)",  60, 222, 105, 20, IDC_RAD_AUTO, WS_GROUP);
    g_hRadAudio = MkRadio(p, "Audio Only",  170, 222,  95, 20, IDC_RAD_AUDIO);
    g_hRadVideo = MkRadio(p, "Video",       270, 222,  60, 20, IDC_RAD_VIDEO);
    SendMessage(g_hRadAuto, BM_SETCHECK, BST_CHECKED, 0);

    // Audio format combo  (shown only for Audio mode)
    g_hLblAudFmt = MkLabel(p, "Format:", 340, 224, 55, 16);
    g_hAudFmt    = MkCombo(p, 398, 220, 90, 22, IDC_CMB_AUDIO_FMT);
    const char* af[] = {"mp3","flac","aac","opus","wav","m4a","ogg","alac"};
    ComboFill(g_hAudFmt, af, 8);
    ShowWindow(g_hLblAudFmt, SW_HIDE);
    ShowWindow(g_hAudFmt,    SW_HIDE);

    // Video quality combo  (shown for Auto and Video mode)
    g_hLblVidQual = MkLabel(p, "Quality:", 340, 224, 58, 16);
    g_hVidQual    = MkCombo(p, 400, 220, 130, 22, IDC_CMB_VIDEO_QUAL);
    const char* vq[] = {"Best Available","4K (2160p)","1440p (2K)","1080p",
                        "720p","480p","360p","240p","Worst"};
    ComboFill(g_hVidQual, vq, 9);

    // Video container format  (shown only for Video mode)
    g_hLblVidFmt = MkLabel(p, "Container:", 540, 224, 72, 16);
    g_hVidFmt    = MkCombo(p, 615, 220, 113, 22, IDC_CMB_VIDEO_FMT);
    const char* vf[] = {"mp4","mkv","webm","avi","mov","ts"};
    ComboFill(g_hVidFmt, vf, 6);
    ShowWindow(g_hLblVidFmt, SW_HIDE);
    ShowWindow(g_hVidFmt,    SW_HIDE);

    // ---- Quick Options ----
    MkLabel(p, "Options:", 10, 260, 65, 18, g_fBold);
    g_hThumb    = MkCheck(p, "Embed Thumbnail",  80, 258, 140, 22, IDC_CHK_THUMB,    true);
    g_hMeta     = MkCheck(p, "Embed Metadata",  225, 258, 130, 22, IDC_CHK_META,     true);
    g_hRestrict = MkCheck(p, "ASCII Filenames", 360, 258, 125, 22, IDC_CHK_RESTRICT, true);
    g_hPlaylist = MkCheck(p, "Playlist Mode",   490, 258, 115, 22, IDC_CHK_PLAYLIST, false);

    // ---- Cookies ----
    MkLabel(p, "Cookies:", 10, 292, 65, 18, g_fBold);
    g_hCookies = MkCombo(p, 80, 290, 128, 22, IDC_CMB_COOKIES);
    const char* br[] = {"None","chrome","firefox","edge","brave","opera","vivaldi","chromium"};
    ComboFill(g_hCookies, br, 8);
    MkLabel(p, "(Use cookies for age-restricted / private / login-required content)",
        215, 293, 510, 17, g_fSmall);

    // ---- Info Box ----
    MkLabel(p, "Supported Platforms:", 10, 328, 155, 18, g_fBold);
    MkLabel(p,
        "YouTube  |  TikTok  |  Instagram  |  Kick  |  Twitch  |  Twitter/X  |  Facebook  |  Reddit  |  "
        "SoundCloud  |  Vimeo  |  Bilibili  |  Rumble  |  Odysee  |  Bandcamp  |  Dailymotion  |  "
        "Mixcloud  |  Spotify  |  Pinterest  |  NicoNico  |  Aparat  |  Snapchat  |  and 1000+ more!",
        10, 348, 728, 48, g_fSmall);
}

// ==============================================================
//   PANEL 1  —  ADVANCED OPTIONS
// ==============================================================
void CreateAdvancedPanel() {
    HWND p = g_hPanels[1];
    int y = 10;

    // ---- Subtitles ----
    MkLabel(p, "Subtitles", 10, y, 200, 18, g_fLarge); y += 26;
    g_hSubs      = MkCheck(p, "Download Subtitles",    10, y, 158, 22, IDC_CHK_SUBS);
    g_hEmbedSubs = MkCheck(p, "Embed in Video",       173, y, 128, 22, IDC_CHK_EMBED_SUBS);
    g_hSubsAuto  = MkCheck(p, "Auto-Generated OK",    306, y, 148, 22, IDC_CHK_SUBTITLES_AUTO);
    MkLabel(p, "Language:", 460, y+2, 68, 18);
    g_hSubsLang  = MkCombo(p, 530, y, 75, 22, IDC_CMB_SUBS_LANG);
    const char* sl[] = {"en","ar","fr","de","es","ja","zh-Hans","zh-Hant",
                        "ko","ru","pt","it","tr","pl","hi","id","nl","sv","da","no"};
    ComboFill(g_hSubsLang, sl, 20);
    MkLabel(p, "Format:", 613, y+2, 50, 18);
    g_hSubFmt = MkCombo(p, 663, y, 75, 22, IDC_CMB_SUB_FMT);
    const char* sf[] = {"srt","vtt","ass","lrc","json3","ttml"};
    ComboFill(g_hSubFmt, sf, 6);
    y += 38;

    // ---- Video Processing ----
    MkLabel(p, "Video Processing", 10, y, 200, 18, g_fLarge); y += 26;
    g_hChapters     = MkCheck(p, "Split by Chapters",        10,  y, 155, 22, IDC_CHK_CHAPTERS);
    g_hSponsorBlock = MkCheck(p, "Remove SponsorBlock Ads",  170, y, 188, 22, IDC_CHK_SPONSORBLOCK);
    g_hAgeBypass    = MkCheck(p, "Age-Restricted Bypass",    363, y, 172, 22, IDC_CHK_AGE_BYPASS);
    g_hGeoBypass    = MkCheck(p, "Geo-Restriction Bypass",   540, y, 185, 22, IDC_CHK_GEO_BYPASS);
    y += 38;

    // ---- File Output ----
    MkLabel(p, "File Output", 10, y, 200, 18, g_fLarge); y += 26;
    g_hWriteThumb  = MkCheck(p, "Save Thumbnail File",    10,  y, 158, 22, IDC_CHK_WRITE_THUMB);
    g_hWriteInfo   = MkCheck(p, "Save Info JSON",        173,  y, 120, 22, IDC_CHK_WRITE_INFO);
    g_hNoPart      = MkCheck(p, "No .part Files",        298,  y, 112, 22, IDC_CHK_NO_PART);
    g_hKeepFrag    = MkCheck(p, "Keep Fragments",        415,  y, 118, 22, IDC_CHK_KEEP_FRAG);
    g_hNoOverwrite = MkCheck(p, "No Overwrites",         538,  y, 110, 22, IDC_CHK_NO_OVERWRITE);
    y += 38;

    // ---- Network / Speed ----
    MkLabel(p, "Network & Speed", 10, y, 200, 18, g_fLarge); y += 26;
    MkLabel(p, "Proxy:", 10, y+3, 45, 18);
    g_hProxy = MkEdit(p, "", 58, y, 248, 26, IDC_EDT_PROXY);
    MkLabel(p, "Rate Limit (KB/s):", 316, y+3, 130, 18);
    g_hRateLimit = MkEdit(p, "", 449, y, 70, 26, IDC_EDT_RATELIMIT);
    MkLabel(p, "Fragments:", 529, y+3, 78, 18);
    g_hConcurrent = MkEdit(p, "4", 610, y, 50, 26, IDC_EDT_CONCURRENT);
    MkLabel(p, "= parallel download threads per file", 668, y+3, 260, 18, g_fSmall);
    y += 34;
    g_hAria2 = MkCheck(p, "Use aria2c downloader (faster, needs aria2c.exe in path)",
                        10, y, 430, 22, IDC_CHK_ARIA2);
    y += 38;

    // ---- Filename Template ----
    MkLabel(p, "Output Filename Template:", 10, y, 220, 18, g_fLarge); y += 26;
    g_hFilenameTmpl = MkEdit(p, "%(uploader)s - %(title)s.%(ext)s",
                              10, y, 728, 26, IDC_EDT_FILENAME);
    MkLabel(p, "Playlist: %(playlist_index)03d. %(title)s.%(ext)s  |  "
               "Variables: %(title)s  %(uploader)s  %(id)s  %(ext)s  %(upload_date)s",
               10, y+30, 728, 17, g_fSmall);
    y += 58;

    // ---- Custom Args ----
    MkLabel(p, "Extra yt-dlp Arguments (power users):", 10, y, 300, 18, g_fLarge); y += 26;
    g_hCustomArgs = MkEdit(p, "", 10, y, 728, 26, IDC_EDT_CUSTOM_ARGS);
}

// ==============================================================
//   PANEL 2  —  ABOUT
// ==============================================================
void CreateAboutPanel() {
    HWND p = g_hPanels[2];
    int y = 18;

    MkLabel(p, "ZooDownloader v1.0", 18, y, 700, 34, g_fTitle);          y += 44;
    MkLabel(p, "By iTsTimeCoder  |  Powered by yt-dlp & ffmpeg", 18, y, 600, 22, g_fLarge); y += 36;

    MkLabel(p, "A full-featured, dark-themed GUI front-end for yt-dlp — the world's most powerful media downloader.", 18, y, 718, 20); y += 24;
    MkLabel(p, "Supports downloading audio and video from 1000+ websites with advanced options.", 18, y, 700, 20); y += 32;

    MkLabel(p, "Key Features:", 18, y, 200, 20, g_fBold); y += 24;
    const char* feats[] = {
        "  \xBB  Multi-Platform: YouTube, TikTok, Instagram, Kick, Twitch, Twitter/X, Facebook, Reddit, and 1000+ sites",
        "  \xBB  Audio Extraction: MP3, FLAC, AAC, Opus, WAV, M4A, OGG, ALAC at highest quality",
        "  \xBB  Video Download: MP4, MKV, WebM, AVI, MOV  |  Quality: 4K, 2K, 1080p, 720p, 480p, 360p...",
        "  \xBB  Subtitles: Download & embed in 20+ languages  |  Formats: SRT, VTT, ASS, LRC, TTML",
        "  \xBB  SponsorBlock: auto-remove sponsor segments, self-promos & interaction reminders",
        "  \xBB  Chapter splitting, thumbnail embedding, full metadata tagging",
        "  \xBB  Browser cookie extraction (Chrome, Firefox, Edge, Brave, Opera, Vivaldi...)",
        "  \xBB  Proxy support, rate limiting, parallel fragment downloads, aria2c integration",
        "  \xBB  Batch download: paste multiple URLs (one per line)",
        "  \xBB  Geo-restriction & age-restriction bypass options",
        "  \xBB  Custom filename templates with yt-dlp variables",
        "  \xBB  Custom yt-dlp arguments for full control",
    };
    for (const char* f : feats) { MkLabel(p, f, 18, y, 718, 20); y += 20; }

    y += 14;
    MkLabel(p, "Requirements: yt-dlp.exe + ffmpeg.exe  (embedded as resources or placed in the same folder as this .exe)", 18, y, 718, 18, g_fSmall); y += 32;
    MkBtn(p, "  Update yt-dlp Engine  ", 18, y, 200, 36, IDC_BTN_UPDATE);
}

// ==============================================================
//   SHOW TAB
// ==============================================================
void ShowTab(int idx) {
    for (int i = 0; i < 3; i++)
        ShowWindow(g_hPanels[i], i == idx ? SW_SHOW : SW_HIDE);
}

// ==============================================================
//   BUILD COMMAND & EXECUTE
// ==============================================================
void BuildAndRun(HWND hwnd) {
    // Get and trim URL
    std::string url = GetWndText(g_hUrl);
    while (!url.empty() && (url.back()=='\r'||url.back()=='\n'||url.back()==' '))
        url.pop_back();

    if (url.size() < 8) {
        MessageBoxA(hwnd, "Please enter a valid URL!", "ZooDownloader v1.0", MB_ICONWARNING);
        return;
    }

    // Extract first URL for platform detection
    std::string first = url;
    {
        size_t nl = first.find('\n');
        if (nl != std::string::npos) first = first.substr(0, nl);
        while (!first.empty() && (first.back()=='\r'||first.back()==' ')) first.pop_back();
    }

    bool isBatch = (url.find('\n') != std::string::npos);
    std::string dest = GetWndText(g_hPath);
    std::stringstream ss;

    // --- Output path ---
    if (!dest.empty()) ss << "-P \"" << dest << "\" ";

    // --- Format selection ---
    bool isAud = (SendMessage(g_hRadAudio, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool isVid = (SendMessage(g_hRadVideo, BM_GETCHECK, 0, 0) == BST_CHECKED);

    if (isAud) {
        std::string fmt = GetComboSel(g_hAudFmt);
        ss << "-x --audio-format " << (fmt.empty() ? "mp3" : fmt) << " --audio-quality 0 ";
    } else {
        std::string q = GetComboSel(g_hVidQual);
        int h = 0;
        if      (q == "4K (2160p)")  h = 2160;
        else if (q == "1440p (2K)")  h = 1440;
        else if (q == "1080p")       h = 1080;
        else if (q == "720p")        h =  720;
        else if (q == "480p")        h =  480;
        else if (q == "360p")        h =  360;
        else if (q == "240p")        h =  240;

        if (h > 0)
            ss << "-f \"bestvideo[height<=" << h << "]+bestaudio/best[height<=" << h << "]\" ";
        else if (q == "Worst")
            ss << "-f \"worstvideo+worstaudio/worst\" ";
        else
            ss << "-f \"bestvideo+bestaudio/best\" ";

        std::string vf = GetComboSel(g_hVidFmt);
        ss << "--merge-output-format " << (vf.empty() ? "mp4" : vf) << " ";
    }

    // --- Options helper ---
    auto Chk = [](HWND h) { return SendMessage(h, BM_GETCHECK, 0, 0) == BST_CHECKED; };

    if (Chk(g_hThumb))        ss << "--embed-thumbnail ";
    if (Chk(g_hMeta))         ss << "--embed-metadata ";
    if (Chk(g_hRestrict))     ss << "--restrict-filenames ";
    if (Chk(g_hWriteThumb))   ss << "--write-thumbnail ";
    if (Chk(g_hWriteInfo))    ss << "--write-info-json ";
    if (Chk(g_hChapters))     ss << "--split-chapters ";
    if (Chk(g_hAgeBypass))    ss << "--age-limit 99 ";
    if (Chk(g_hGeoBypass))    ss << "--geo-bypass ";
    if (Chk(g_hNoPart))       ss << "--no-part ";
    if (Chk(g_hKeepFrag))     ss << "--keep-fragments ";
    if (Chk(g_hNoOverwrite))  ss << "--no-overwrites ";
    if (Chk(g_hAria2))        ss << "--downloader aria2c "
                                     "--downloader-args \"-c -j8 -s8 -x8 -k1M\" ";
    if (Chk(g_hSponsorBlock)) ss << "--sponsorblock-remove "
                                     "sponsor,selfpromo,interaction,intro,outro ";

    // --- Subtitles ---
    if (Chk(g_hSubs)) {
        ss << "--write-subs --sub-langs " << GetComboSel(g_hSubsLang) << " ";
        ss << "--convert-subs " << GetComboSel(g_hSubFmt) << " ";
        if (Chk(g_hSubsAuto))  ss << "--write-auto-subs ";
        if (Chk(g_hEmbedSubs)) ss << "--embed-subs ";
    }

    // --- Cookies ---
    std::string cook = GetComboSel(g_hCookies);
    if (cook != "None") ss << "--cookies-from-browser " << cook << " ";

    // --- Network ---
    std::string rate = GetWndText(g_hRateLimit);
    if (!rate.empty() && rate != "0") ss << "--rate-limit " << rate << "K ";

    std::string proxy = GetWndText(g_hProxy);
    if (!proxy.empty()) ss << "--proxy \"" << proxy << "\" ";

    std::string conc = GetWndText(g_hConcurrent);
    if (!conc.empty() && conc != "1" && conc != "0")
        ss << "--concurrent-fragments " << conc << " ";

    // --- Custom args ---
    std::string custom = GetWndText(g_hCustomArgs);
    if (!custom.empty()) ss << custom << " ";

    // --- Filename template ---
    std::string tmpl = GetWndText(g_hFilenameTmpl);

    // --- Playlist vs single ---
    if (Chk(g_hPlaylist)) {
        ss << "--yes-playlist ";
        if (!tmpl.empty())
            ss << "-o \"%(playlist_index)03d. " << tmpl << "\" ";
        else
            ss << "-o \"%(playlist_index)03d. %(title)s.%(ext)s\" ";
    } else {
        ss << "--no-playlist ";
        if (!tmpl.empty())
            ss << "-o \"" << tmpl << "\" ";
        else
            ss << "-o \"%(uploader)s - %(title)s.%(ext)s\" ";
    }

    // --- Progress output ---
    ss << "--newline ";

    // --- URL or batch file ---
    if (isBatch) {
        std::string tmp = std::string(getenv("TEMP")) + "\\zoo_batch_urls.txt";
        std::ofstream tf(tmp); tf << url; tf.close();
        ss << "--batch-file \"" << tmp << "\"";
    } else {
        ss << "\"" << first << "\"";
    }

    // --- Log ---
    AppendLog("=========================================");
    AppendLog("   ZooDownloader v1.0  |  By iTsTimeCoder");
    AppendLog("=========================================");
    AppendLog("  Platform : " + DetectPlatform(first));
    AppendLog("  Mode     : " + std::string(isAud ? "Audio ("  + GetComboSel(g_hAudFmt)  + ")"
                                                  : "Video ("  + GetComboSel(g_hVidQual) + ")"));
    AppendLog("  Batch    : " + std::string(isBatch ? "Yes — multiple URLs" : "No"));
    AppendLog("  Dest     : " + (dest.empty() ? "(current dir)" : dest));
    AppendLog("-----------------------------------------");
    std::string cmd_preview = "yt-dlp " + ss.str();
    if (cmd_preview.size() > 220) cmd_preview = cmd_preview.substr(0, 220) + "...";
    AppendLog("  CMD      : " + cmd_preview);
    AppendLog("-----------------------------------------");

    // --- Execute ---
    std::string shellArgs = "/c yt-dlp " + ss.str()
        + " & echo. & echo  ================================================="
          " & echo   ZooDownloader  -  Download Complete!"
          " & echo  ================================================="
          " & pause";

    SHELLEXECUTEINFOA sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask  = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = "open";
    sei.lpFile = "cmd.exe";
    sei.lpParameters = shellArgs.c_str();
    sei.nShow  = SW_SHOW;
    ShellExecuteExA(&sei);

    // Indeterminate progress animation while cmd window is open
    SendMessage(g_hProgress, PBM_SETMARQUEE, TRUE, 50);
}

// ==============================================================
//   MAIN WINDOW PROCEDURE
// ==============================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_CREATE: {
        g_hwnd = hwnd;
        CreateFonts();
        g_brBg   = CreateSolidBrush(CLR_BG);
        g_brSurf = CreateSolidBrush(CLR_SURFACE);
        g_brSurf2= CreateSolidBrush(CLR_SURF2);

        INITCOMMONCONTROLSEX icc = {sizeof(icc),
            ICC_TAB_CLASSES|ICC_PROGRESS_CLASS|ICC_STANDARD_CLASSES};
        InitCommonControlsEx(&icc);

        // ---- Tab Control ----
        g_hTab = CreateWindowExA(0, WC_TABCONTROLA, "",
            WS_VISIBLE|WS_CHILD|TCS_HOTTRACK,
            5, 5, 754, 430, hwnd, (HMENU)IDC_TAB, g_hInst, NULL);
        SF(g_hTab, g_fBold);

        TCITEMA ti = {TCIF_TEXT};
        ti.pszText = (char*)"    Download    "; TabCtrl_InsertItem(g_hTab, 0, &ti);
        ti.pszText = (char*)"    Advanced    "; TabCtrl_InsertItem(g_hTab, 1, &ti);
        ti.pszText = (char*)"    About       "; TabCtrl_InsertItem(g_hTab, 2, &ti);

        // Calculate display area inside tab
        RECT r = {5, 5, 759, 435};
        TabCtrl_AdjustRect(g_hTab, FALSE, &r);

        // ---- Create Panels (children of main window, inside tab area) ----
        for (int i = 0; i < 3; i++) {
            g_hPanels[i] = CreateWindowExA(0, "ZooPanel", "",
                WS_CHILD | (i == 0 ? WS_VISIBLE : 0),
                r.left, r.top,
                r.right - r.left, r.bottom - r.top,
                hwnd, NULL, g_hInst, NULL);
        }

        CreateDownloadPanel();
        CreateAdvancedPanel();
        CreateAboutPanel();
        ShowTab(0);

        // ---- Log Output ----
        MkLabel(hwnd, "Output Log:", 5, 442, 100, 18, g_fBold);
        g_hLog = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE|WS_CHILD|ES_MULTILINE|ES_READONLY|WS_VSCROLL|ES_AUTOVSCROLL,
            5, 462, 754, 110, hwnd, (HMENU)IDC_LOG_OUTPUT, g_hInst, NULL);
        SF(g_hLog, g_fMono);

        // ---- Bottom Action Bar ----
        MkBtn(hwnd, "  DOWNLOAD  ", 5,   578, 240, 50, IDC_BTN_DOWNLOAD, BS_DEFPUSHBUTTON);
        MkBtn(hwnd, "  STOP  ",     252, 578,  80, 50, IDC_BTN_STOP);
        MkBtn(hwnd, "Clear Log",    340, 578, 105, 50, IDC_BTN_CLEAR_LOG);
        MkBtn(hwnd, "Update Engine",452, 578, 145, 50, IDC_BTN_UPDATE);

        // Progress bar (marquee style for indeterminate progress)
        g_hProgress = CreateWindowExA(0, PROGRESS_CLASS, "",
            WS_VISIBLE|WS_CHILD|PBS_SMOOTH|PBS_MARQUEE,
            604, 578, 155, 50, hwnd, (HMENU)IDC_PROGRESS, g_hInst, NULL);
        SendMessage(g_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessage(g_hProgress, PBM_SETBARCOLOR, 0, (LPARAM)CLR_ACCENT);
        SendMessage(g_hProgress, PBM_SETBKCOLOR,  0, (LPARAM)CLR_SURF2);

        AppendLog("  ZooDownloader v1.0  --  By iTsTimeCoder");
        AppendLog("  Supports YouTube, TikTok, Instagram, Kick, Twitch, X, Facebook, and 1000+ sites.");
        AppendLog("  Paste URL(s) in the Download tab, configure options, then click DOWNLOAD.");
        break;
    }

    case WM_NOTIFY: {
        NMHDR* nm = (NMHDR*)lp;
        if (nm->idFrom == IDC_TAB && nm->code == TCN_SELCHANGE)
            ShowTab(TabCtrl_GetCurSel(g_hTab));
        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);
        switch (id) {

        case IDC_BTN_BROWSE: {
            char path[MAX_PATH] = {};
            BROWSEINFOA bi = {};
            bi.hwndOwner = hwnd;
            bi.lpszTitle = "Select Download Folder:";
            bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
            if (pidl) {
                SHGetPathFromIDListA(pidl, path);
                SetWindowTextA(g_hPath, path);
                ILFree(pidl);
            }
            break;
        }

        case IDC_BTN_OPEN_DEST: {
            std::string d = GetWndText(g_hPath);
            if (!d.empty()) ShellExecuteA(NULL, "open", d.c_str(), NULL, NULL, SW_SHOW);
            break;
        }

        case IDC_BTN_DOWNLOAD:
            BuildAndRun(hwnd);
            break;

        case IDC_BTN_STOP:
            SendMessage(g_hProgress, PBM_SETMARQUEE, FALSE, 0);
            SendMessage(g_hProgress, PBM_SETPOS, 0, 0);
            AppendLog("[!] To stop: close the cmd.exe window manually.");
            break;

        case IDC_BTN_UPDATE:
            ShellExecuteA(NULL, "open", "cmd.exe",
                "/c yt-dlp -U & echo. & echo Update complete! & pause", NULL, SW_SHOW);
            AppendLog("[*] Launched yt-dlp update...");
            break;

        case IDC_BTN_CLEAR_LOG:
            SetWindowTextA(g_hLog, "");
            break;

        case IDC_RAD_AUTO:
        case IDC_RAD_AUDIO:
        case IDC_RAD_VIDEO:
            UpdateTypeControls();
            break;
        }

        // Live platform detection on URL change
        if (id == IDC_URL_INPUT && HIWORD(wp) == EN_CHANGE) {
            std::string u = GetWndText(g_hUrl);
            size_t nl = u.find('\n');
            if (nl != std::string::npos) u = u.substr(0, nl);
            while (!u.empty() && (u.back()=='\r'||u.back()==' ')) u.pop_back();

            bool multi = (GetWndText(g_hUrl).find('\n') != std::string::npos);
            std::string label = "  Detected Platform:  " + DetectPlatform(u);
            if (multi) label += "  [+ more URLs]";
            SetWindowTextA(g_hLblPlatform, label.c_str());
        }
        break;
    }

    case WM_ERASEBKGND: {
        RECT rc; GetClientRect(hwnd, &rc);
        FillRect((HDC)wp, &rc, g_brBg);
        return 1;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX: {
        SetTextColor((HDC)wp, CLR_TEXT);
        SetBkColor((HDC)wp, CLR_SURFACE);
        return (LRESULT)g_brSurf;
    }

    case WM_CTLCOLORBTN: {
        SetTextColor((HDC)wp, CLR_TEXT);
        SetBkColor((HDC)wp, CLR_SURF2);
        return (LRESULT)g_brSurf2;
    }

    case WM_DESTROY:
        DeleteObject(g_brBg); DeleteObject(g_brSurf); DeleteObject(g_brSurf2);
        DeleteObject(g_fUI); DeleteObject(g_fBold); DeleteObject(g_fSmall);
        DeleteObject(g_fMono); DeleteObject(g_fTitle); DeleteObject(g_fLarge);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// ==============================================================
//   ENTRY POINT
// ==============================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    g_hInst = hInst;

    // Extract bundled tools (if compiled with resources 1001/1002)
    ExtractResource(1001, "yt-dlp.exe");
    ExtractResource(1002, "ffmpeg.exe");

    INITCOMMONCONTROLSEX icc = {sizeof(icc),
        ICC_TAB_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    // Register panel class (handles dark theme for child controls inside panels)
    WNDCLASSEXA panelWc = {sizeof(panelWc)};
    panelWc.lpfnWndProc   = PanelProc;
    panelWc.hInstance     = hInst;
    panelWc.lpszClassName = "ZooPanel";
    panelWc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    panelWc.hbrBackground = CreateSolidBrush(CLR_SURFACE);
    RegisterClassExA(&panelWc);

    // Register main window class
    WNDCLASSEXA wc = {sizeof(wc)};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "ZooDownloader_v1";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(CLR_BG);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);

    // Create main window  (fixed size: 776 x 650)
    HWND hwnd = CreateWindowExA(
        WS_EX_APPWINDOW,
        "ZooDownloader_v1",
        "ZooDownloader v1.0  --  By iTsTimeCoder",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        776, 680,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// ==============================================================
//   HOW TO COMPILE:
//
//   Using MinGW-w64 (g++):
//     g++ ZooDownloader_v1.cpp -o ZooDownloader.exe -mwindows \
//         -lcomctl32 -lshell32 -lole32 -luuid -std=c++11 -O2
//
//   Using MSVC (Developer Command Prompt):
//     cl /EHsc /O2 ZooDownloader_v1.cpp /Fe:ZooDownloader.exe \
//        /link comctl32.lib shell32.lib ole32.lib user32.lib gdi32.lib
//
//   To embed yt-dlp.exe and ffmpeg.exe as resources, create a .rc file:
//     1001 RCDATA "yt-dlp.exe"
//     1002 RCDATA "ffmpeg.exe"
//   Then compile the .rc and link the resulting .res file.
//   If not embedding, just place yt-dlp.exe and ffmpeg.exe next to the .exe.
// ==============================================================
