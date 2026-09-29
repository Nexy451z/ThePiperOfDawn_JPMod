#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <mmsystem.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <atomic>
#include <exception>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

#pragma comment(lib, "user32.lib")

#if !defined(_WIN64)
#error "version.cpp is x64-only (inline hooks use 64-bit absolute jumps)."
#endif

// ============================================================================
// 1. System version.dll Proxy Forwarding
// ============================================================================
static HMODULE g_hSysVersion = nullptr;
static FARPROC g_pfnVersion[17] = { nullptr };

static BOOL CALLBACK InitVersionProxyOnce(PINIT_ONCE, PVOID, PVOID*) {
    wchar_t sysDir[MAX_PATH] = { 0 };
    GetSystemDirectoryW(sysDir, MAX_PATH);
    std::wstring path = std::wstring(sysDir) + L"\\version.dll";
    HMODULE h = LoadLibraryW(path.c_str());
    if (!h) return FALSE;

    const char* names[17] = {
        "GetFileVersionInfoA",
        "GetFileVersionInfoByHandle",
        "GetFileVersionInfoExA",
        "GetFileVersionInfoExW",
        "GetFileVersionInfoSizeA",
        "GetFileVersionInfoSizeExA",
        "GetFileVersionInfoSizeExW",
        "GetFileVersionInfoSizeW",
        "GetFileVersionInfoW",
        "VerFindFileA",
        "VerFindFileW",
        "VerInstallFileA",
        "VerInstallFileW",
        "VerLanguageNameA",
        "VerLanguageNameW",
        "VerQueryValueA",
        "VerQueryValueW"
    };
    FARPROC fns[17] = { nullptr };
    for (int i = 0; i < 17; ++i) {
        fns[i] = GetProcAddress(h, names[i]);
    }
    // Publish only after every slot is filled (no partial-state race).
    for (int i = 0; i < 17; ++i) g_pfnVersion[i] = fns[i];
    g_hSysVersion = h;
    return TRUE;
}

static void InitVersionProxy() {
    static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
    InitOnceExecuteOnce(&once, InitVersionProxyOnce, nullptr, nullptr);
}

extern "C" {
BOOL WINAPI Proxy_GetFileVersionInfoA(LPCSTR a, DWORD b, DWORD c, LPVOID d) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(LPCSTR, DWORD, DWORD, LPVOID);
    return g_pfnVersion[0] ? ((Fn)g_pfnVersion[0])(a, b, c, d) : FALSE;
}
BOOL WINAPI Proxy_GetFileVersionInfoByHandle(DWORD a, HANDLE b, LPVOID* c, PDWORD d) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(DWORD, HANDLE, LPVOID*, PDWORD);
    return g_pfnVersion[1] ? ((Fn)g_pfnVersion[1])(a, b, c, d) : FALSE;
}
BOOL WINAPI Proxy_GetFileVersionInfoExA(DWORD a, LPCSTR b, DWORD c, DWORD d, LPVOID e) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
    return g_pfnVersion[2] ? ((Fn)g_pfnVersion[2])(a, b, c, d, e) : FALSE;
}
BOOL WINAPI Proxy_GetFileVersionInfoExW(DWORD a, LPCWSTR b, DWORD c, DWORD d, LPVOID e) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
    return g_pfnVersion[3] ? ((Fn)g_pfnVersion[3])(a, b, c, d, e) : FALSE;
}
DWORD WINAPI Proxy_GetFileVersionInfoSizeA(LPCSTR a, LPDWORD b) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(LPCSTR, LPDWORD);
    return g_pfnVersion[4] ? ((Fn)g_pfnVersion[4])(a, b) : 0;
}
DWORD WINAPI Proxy_GetFileVersionInfoSizeExA(DWORD a, LPCSTR b, LPDWORD c) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCSTR, LPDWORD);
    return g_pfnVersion[5] ? ((Fn)g_pfnVersion[5])(a, b, c) : 0;
}
DWORD WINAPI Proxy_GetFileVersionInfoSizeExW(DWORD a, LPCWSTR b, LPDWORD c) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCWSTR, LPDWORD);
    return g_pfnVersion[6] ? ((Fn)g_pfnVersion[6])(a, b, c) : 0;
}
DWORD WINAPI Proxy_GetFileVersionInfoSizeW(LPCWSTR a, LPDWORD b) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(LPCWSTR, LPDWORD);
    return g_pfnVersion[7] ? ((Fn)g_pfnVersion[7])(a, b) : 0;
}
BOOL WINAPI Proxy_GetFileVersionInfoW(LPCWSTR a, DWORD b, DWORD c, LPVOID d) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(LPCWSTR, DWORD, DWORD, LPVOID);
    return g_pfnVersion[8] ? ((Fn)g_pfnVersion[8])(a, b, c, d) : FALSE;
}
DWORD WINAPI Proxy_VerFindFileA(DWORD a, LPCSTR b, LPCSTR c, LPCSTR d, LPSTR e, PUINT f, LPSTR g, PUINT h) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT, LPSTR, PUINT);
    return g_pfnVersion[9] ? ((Fn)g_pfnVersion[9])(a, b, c, d, e, f, g, h) : 0;
}
DWORD WINAPI Proxy_VerFindFileW(DWORD a, LPCWSTR b, LPCWSTR c, LPCWSTR d, LPWSTR e, PUINT f, LPWSTR g, PUINT h) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
    return g_pfnVersion[10] ? ((Fn)g_pfnVersion[10])(a, b, c, d, e, f, g, h) : 0;
}
DWORD WINAPI Proxy_VerInstallFileA(DWORD a, LPCSTR b, LPCSTR c, LPCSTR d, LPCSTR e, LPCSTR f, LPSTR g, PUINT h) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT);
    return g_pfnVersion[11] ? ((Fn)g_pfnVersion[11])(a, b, c, d, e, f, g, h) : 0;
}
DWORD WINAPI Proxy_VerInstallFileW(DWORD a, LPCWSTR b, LPCWSTR c, LPCWSTR d, LPCWSTR e, LPCWSTR f, LPWSTR g, PUINT h) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT);
    return g_pfnVersion[12] ? ((Fn)g_pfnVersion[12])(a, b, c, d, e, f, g, h) : 0;
}
DWORD WINAPI Proxy_VerLanguageNameA(DWORD a, LPSTR b, DWORD c) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPSTR, DWORD);
    return g_pfnVersion[13] ? ((Fn)g_pfnVersion[13])(a, b, c) : 0;
}
DWORD WINAPI Proxy_VerLanguageNameW(DWORD a, LPWSTR b, DWORD c) {
    InitVersionProxy();
    using Fn = DWORD(WINAPI*)(DWORD, LPWSTR, DWORD);
    return g_pfnVersion[14] ? ((Fn)g_pfnVersion[14])(a, b, c) : 0;
}
BOOL WINAPI Proxy_VerQueryValueA(LPCVOID a, LPCSTR b, LPVOID* c, PUINT d) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(LPCVOID, LPCSTR, LPVOID*, PUINT);
    return g_pfnVersion[15] ? ((Fn)g_pfnVersion[15])(a, b, c, d) : FALSE;
}
BOOL WINAPI Proxy_VerQueryValueW(LPCVOID a, LPCWSTR b, LPVOID* c, PUINT d) {
    InitVersionProxy();
    using Fn = BOOL(WINAPI*)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
    return g_pfnVersion[16] ? ((Fn)g_pfnVersion[16])(a, b, c, d) : FALSE;
}
} // extern "C"

// ============================================================================
// 2. Logging & Paths
// ============================================================================
static std::wstring g_modDir;
static SRWLOCK g_lock = SRWLOCK_INIT;
static std::atomic<bool> g_modEnabled{ true };

static void LogMsg(const char* fmt, ...) {
    if (g_modDir.empty()) return;
    std::wstring logPath = g_modDir + L"\\mod.log";
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(logPath.c_str(), GetFileExInfoStandard, &fad) &&
        (((uint64_t)fad.nFileSizeHigh << 32) | fad.nFileSizeLow) > (1ull << 20)) {
        std::wstring oldPath = logPath + L".old";
        MoveFileExW(logPath.c_str(), oldPath.c_str(), MOVEFILE_REPLACE_EXISTING);
    }
    FILE* fp = _wfopen(logPath.c_str(), L"a");
    if (!fp) return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
    fprintf(fp, "\n");
    fclose(fp);
}

// ============================================================================
// 3. Fast UTF-8 JSON Parser for Language.json & LanguageTalk.json
// ============================================================================
static std::unordered_map<int32_t, std::string> g_langMap;
static std::unordered_map<int32_t, std::string> g_talkMap;

// Cache of GC-rooted localized strings (avoids allocating on every lookup).
struct CachedString {
    void* obj;
    uint32_t handle;
};
static SRWLOCK g_strCacheLock = SRWLOCK_INIT;
static std::unordered_map<int32_t, CachedString> g_langStrCache;
static std::unordered_map<int32_t, CachedString> g_talkStrCache;
static const size_t kStrCacheLimit = 100000;
static void ClearStringCaches();

static void AppendUtf8(std::string& out, uint32_t cp) {
    if (cp <= 0x7F) {
        out.push_back((char)cp);
    } else if (cp <= 0x7FF) {
        out.push_back((char)(0xC0 | ((cp >> 6) & 0x1F)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back((char)(0xE0 | ((cp >> 12) & 0x0F)));
        out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    } else {
        out.push_back((char)(0xF0 | ((cp >> 18) & 0x07)));
        out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    }
}

static void SkipWs(const char*& p, const char* end) {
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p;
}

static uint32_t ParseHex4(const char*& p, const char* end) {
    uint32_t val = 0;
    for (int i = 0; i < 4 && p < end; ++i, ++p) {
        char c = *p;
        val <<= 4;
        if (c >= '0' && c <= '9') val |= (c - '0');
        else if (c >= 'a' && c <= 'f') val |= (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
    }
    return val;
}

static bool ParseJsonString(const char*& p, const char* end, std::string& out) {
    out.clear();
    SkipWs(p, end);
    if (p >= end || *p != '"') return false;
    ++p;
    while (p < end) {
        char c = *p++;
        if (c == '"') return true;
        if (c == '\\') {
            if (p >= end) break;
            char esc = *p++;
            switch (esc) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u': {
                    uint32_t cp = ParseHex4(p, end);
                    if (cp >= 0xD800 && cp <= 0xDBFF && p + 2 <= end && p[0] == '\\' && p[1] == 'u') {
                        p += 2;
                        uint32_t low = ParseHex4(p, end);
                        if (low >= 0xDC00 && low <= 0xDFFF) {
                            cp = 0x10000 + (((cp - 0xD800) << 10) | (low - 0xDC00));
                        }
                    }
                    AppendUtf8(out, cp);
                    break;
                }
                default: out.push_back(esc); break;
            }
        } else {
            out.push_back(c);
        }
    }
    return false;
}

static bool ParseJsonInt(const char*& p, const char* end, int32_t& out) {
    SkipWs(p, end);
    if (p >= end) return false;
    bool neg = false;
    if (*p == '-') { neg = true; ++p; }
    if (p >= end || *p < '0' || *p > '9') return false;
    uint64_t val = 0;
    bool overflow = false;
    while (p < end && *p >= '0' && *p <= '9') {
        if (val > 0x7FFFFFFFull) {
            overflow = true; // keep consuming digits, discard value
        } else {
            val = val * 10 + (uint64_t)(*p - '0');
            if (val > 0x7FFFFFFFull) overflow = true;
        }
        ++p;
    }
    if (overflow) return false;
    out = neg ? -(int32_t)val : (int32_t)val;
    return true;
}

static void SkipJsonValue(const char*& p, const char* end) {
    SkipWs(p, end);
    if (p >= end) return;
    const char* start = p;
    if (*p == '"') {
        std::string dummy;
        ParseJsonString(p, end, dummy);
    } else if (*p == '{' || *p == '[') {
        char open = *p++;
        char close = (open == '{') ? '}' : ']';
        int depth = 1;
        while (p < end && depth > 0) {
            if (*p == '"') {
                std::string dummy;
                ParseJsonString(p, end, dummy);
            } else {
                if (*p == open) ++depth;
                else if (*p == close) --depth;
                ++p;
            }
        }
    } else {
        while (p < end && *p != ',' && *p != '}' && *p != ']' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
            ++p;
        }
    }
    // Malformed input (e.g. a stray '}' in an array) must still make progress,
    // otherwise the parser loop would spin forever.
    if (p == start && p < end) ++p;
}

static size_t LoadJsonFileIntoMap(const std::wstring& path, std::unordered_map<int32_t, std::string>& targetMap, bool* okOut = nullptr) {
    if (okOut) *okOut = false;
    FILE* fp = _wfopen(path.c_str(), L"rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0) { fclose(fp); return 0; }
    if ((uint64_t)sz > (256ull << 20)) {
        fclose(fp);
        LogMsg("ERROR: JSON file exceeds 256MB limit; ignored");
        return 0;
    }

    std::string buf;
    try {
        buf.resize((size_t)sz);
    } catch (const std::exception&) {
        fclose(fp);
        LogMsg("ERROR: Out of memory while loading JSON");
        return 0;
    }
    size_t got = fread(&buf[0], 1, (size_t)sz, fp);
    fclose(fp);
    if (got != (size_t)sz) {
        LogMsg("ERROR: Short read while loading JSON");
        return 0;
    }

    const char* p = buf.data();
    const char* end = p + buf.size();
    // Skip UTF-8 BOM if present
    if (end - p >= 3 && (uint8_t)p[0] == 0xEF && (uint8_t)p[1] == 0xBB && (uint8_t)p[2] == 0xBF) {
        p += 3;
    }

    SkipWs(p, end);
    if (p >= end) return 0;

    size_t count = 0;
    if (*p == '[') {
        // Format A: [ {"id": 123, "jp": "..."}, ... ]
        ++p;
        while (p < end) {
            SkipWs(p, end);
            if (p >= end || *p == ']') break;
            if (*p == ',') { ++p; continue; }
            if (*p != '{') { SkipJsonValue(p, end); continue; }
            ++p; // inside object
            bool hasId = false;
            int32_t id = 0;
            bool hasJp = false;
            std::string jp;

            while (p < end) {
                SkipWs(p, end);
                if (p >= end || *p == '}') { if (p < end) ++p; break; }
                if (*p == ',') { ++p; continue; }
                std::string key;
                if (!ParseJsonString(p, end, key)) { ++p; continue; }
                SkipWs(p, end);
                if (p < end && *p == ':') ++p;
                SkipWs(p, end);
                if (key == "id") {
                    if (ParseJsonInt(p, end, id)) hasId = true;
                    else SkipJsonValue(p, end);
                } else if (key == "jp") {
                    if (ParseJsonString(p, end, jp)) hasJp = true;
                    else SkipJsonValue(p, end);
                } else {
                    SkipJsonValue(p, end);
                }
            }
            if (hasId && hasJp) {
                targetMap[id] = std::move(jp);
                ++count;
            }
        }
    } else if (*p == '{') {
        // Format B: { "123": "...", ... }
        ++p;
        while (p < end) {
            SkipWs(p, end);
            if (p >= end || *p == '}') break;
            if (*p == ',') { ++p; continue; }
            std::string key;
            if (!ParseJsonString(p, end, key)) { ++p; continue; }
            SkipWs(p, end);
            if (p < end && *p == ':') ++p;
            SkipWs(p, end);
            std::string val;
            if (ParseJsonString(p, end, val)) {
                const char* kp = key.c_str();
                const char* kend = kp + key.size();
                int32_t id = 0;
                bool parsed = ParseJsonInt(kp, kend, id);
                if (parsed && (id != 0 || key == "0")) {
                    targetMap[id] = std::move(val);
                    ++count;
                }
            } else {
                SkipJsonValue(p, end);
            }
        }
    }
    if (okOut) *okOut = true;
    return count;
}

static void ReloadTranslations() {
    std::unordered_map<int32_t, std::string> newLang;
    std::unordered_map<int32_t, std::string> newTalk;
    newLang.reserve(8192);
    newTalk.reserve(45000);

    bool okLang = false;
    bool okTalk = false;
    size_t c1 = LoadJsonFileIntoMap(g_modDir + L"\\Language.json", newLang, &okLang);
    size_t c1_patch = LoadJsonFileIntoMap(g_modDir + L"\\override_Language.json", newLang);
    size_t c2 = LoadJsonFileIntoMap(g_modDir + L"\\LanguageTalk.json", newTalk, &okTalk);
    size_t c2_patch = LoadJsonFileIntoMap(g_modDir + L"\\override_LanguageTalk.json", newTalk);

    if (!okLang || !okTalk) {
        LogMsg("ERROR: Reload aborted (Language.json or LanguageTalk.json missing/unreadable); keeping previous translations");
        return;
    }

    AcquireSRWLockExclusive(&g_lock);
    g_langMap.swap(newLang);
    g_talkMap.swap(newTalk);
    ReleaseSRWLockExclusive(&g_lock);

    ClearStringCaches();

    LogMsg("Loaded translations: Language=%zu (+%zu overrides), LanguageTalk=%zu (+%zu overrides)",
        c1, c1_patch, c2, c2_patch);
}

// ============================================================================
// 4. Recent Lookup Ring Buffer (for F7 Debug Dump)
// ============================================================================
// Packed entry: bit0 = isTalk, bits1..32 = id. Written/read atomically.
static volatile LONG64 g_recentRing[128] = { 0 };
static volatile LONG g_recentIdx = 0;

static void RecordRecent(bool isTalk, int32_t id) {
    LONG idx = InterlockedIncrement(&g_recentIdx) & 127;
    LONG64 v = ((LONG64)(uint32_t)id << 1) | (isTalk ? 1 : 0);
    InterlockedExchange64(&g_recentRing[idx], v);
}

static void DumpRecentIds() {
    std::wstring outPath = g_modDir + L"\\recent_ids.txt";
    FILE* fp = _wfopen(outPath.c_str(), L"wb");
    if (!fp) return;
    fprintf(fp, "=== Recent Localization IDs (Newest Last) ===\n");
    LONG cur = g_recentIdx;
    AcquireSRWLockShared(&g_lock);
    for (int i = 127; i >= 0; --i) {
        LONG64 v = g_recentRing[(cur - i) & 127];
        int32_t id = (int32_t)(v >> 1);
        bool isTalk = (v & 1) != 0;
        if (id == 0) continue;
        const char* table = isTalk ? "LanguageTalk" : "Language";
        const auto& mp = isTalk ? g_talkMap : g_langMap;
        auto it = mp.find(id);
        const char* txt = (it != mp.end()) ? it->second.c_str() : "<not in mod json>";
        fprintf(fp, "[%s] ID=%d : %s\n", table, id, txt);
    }
    ReleaseSRWLockShared(&g_lock);
    fclose(fp);
    LogMsg("Dumped recent IDs to recent_ids.txt");
}

// ============================================================================
// 5. IL2CPP Hooks (bs.eug @ RVA 0x3D2D20 & bs.euh @ RVA 0x3D2E40)
// ============================================================================
using il2cpp_string_new_t = void* (*)(const char* str);
using bs_lookup_t = void* (*)(int32_t id, const void* method);
using get_setting_mgr_t = uintptr_t (*)(const void* method);
using gchandle_new_t = uint32_t (*)(void* obj, bool pinned);
using gchandle_free_t = void (*)(uint32_t handle);

static il2cpp_string_new_t g_il2cpp_string_new = nullptr;
static bs_lookup_t g_orig_bs_eug = nullptr;
static bs_lookup_t g_orig_bs_euh = nullptr;
static get_setting_mgr_t g_get_setting_mgr = nullptr;
static gchandle_new_t g_gchandle_new = nullptr;
static gchandle_free_t g_gchandle_free = nullptr;
static std::atomic<bool> g_strCacheAvailable{ false };

// Check if in-game language is set to Japanese (1)
// In bs.eug/bs.euh: call 0x519E20 -> [rax + 0xC0] == 1 for Japanese (0=CN, 1=JP, 2=TW, 3=EN)
static std::atomic<int64_t> g_langCheckTick{ 0 };
static std::atomic<int> g_langCheckResult{ 1 };

static bool IsGameLanguageJapanese() {
    if (!g_get_setting_mgr) return true;
    // The result only changes when the player edits settings, so cache it briefly
    // instead of calling into the game on every string lookup.
    int64_t now = (int64_t)GetTickCount64();
    if (now - g_langCheckTick.load() < 500) {
        return g_langCheckResult.load() != 0;
    }
    bool jp = true;
    __try {
        uintptr_t mgr = g_get_setting_mgr(nullptr);
        if (mgr) jp = (*(int32_t*)(mgr + 0xC0) == 1);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        jp = true; // fail open: keep translation visible
    }
    g_langCheckResult.store(jp ? 1 : 0);
    g_langCheckTick.store(now);
    return jp;
}

// Return a native Il2CppString for the given translation text.
// When IL2CPP GC-handle APIs are available the result is cached and rooted,
// so repeated lookups reuse one object instead of allocating every frame.
static void* CreateLocalizedString(bool isTalk, int32_t id, const std::string& jp) {
    if (!g_il2cpp_string_new) return nullptr;
    if (!g_strCacheAvailable.load()) {
        return g_il2cpp_string_new(jp.c_str());
    }
    auto& cache = isTalk ? g_talkStrCache : g_langStrCache;

    AcquireSRWLockShared(&g_strCacheLock);
    auto it = cache.find(id);
    if (it != cache.end()) {
        void* obj = it->second.obj;
        ReleaseSRWLockShared(&g_strCacheLock);
        return obj;
    }
    bool atLimit = cache.size() >= kStrCacheLimit;
    ReleaseSRWLockShared(&g_strCacheLock);
    if (atLimit) {
        return g_il2cpp_string_new(jp.c_str());
    }

    void* obj = g_il2cpp_string_new(jp.c_str());
    if (!obj) return nullptr;
    uint32_t handle = g_gchandle_new(obj, true);
    if (!handle) return obj;

    AcquireSRWLockExclusive(&g_strCacheLock);
    auto it2 = cache.find(id);
    if (it2 != cache.end()) {
        void* existing = it2->second.obj;
        ReleaseSRWLockExclusive(&g_strCacheLock);
        g_gchandle_free(handle);
        return existing;
    }
    CachedString entry;
    entry.obj = obj;
    entry.handle = handle;
    cache.emplace(id, entry);
    ReleaseSRWLockExclusive(&g_strCacheLock);
    return obj;
}

static void ClearStringCaches() {
    AcquireSRWLockExclusive(&g_strCacheLock);
    // Do NOT free the GC handles here: a game thread may still hold a string we
    // already handed out. Dropping the map is enough to pick up new content on
    // the next lookup; the few leaked roots are harmless.
    g_langStrCache.clear();
    g_talkStrCache.clear();
    ReleaseSRWLockExclusive(&g_strCacheLock);
}

static void* Hook_bs_eug(int32_t id, const void* method) {
    void* origRet = g_orig_bs_eug(id, method);
    if (!origRet) return origRet;
    RecordRecent(false, id);
    if (!g_modEnabled.load() || !g_il2cpp_string_new || !IsGameLanguageJapanese()) {
        return origRet;
    }
    void* customRet = nullptr;
    AcquireSRWLockShared(&g_lock);
    auto it = g_langMap.find(id);
    if (it != g_langMap.end() && !it->second.empty()) {
        customRet = CreateLocalizedString(false, id, it->second);
    }
    ReleaseSRWLockShared(&g_lock);
    return customRet ? customRet : origRet;
}

static void* Hook_bs_euh(int32_t id, const void* method) {
    void* origRet = g_orig_bs_euh(id, method);
    if (!origRet) return origRet;
    RecordRecent(true, id);
    if (!g_modEnabled.load() || !g_il2cpp_string_new || !IsGameLanguageJapanese()) {
        return origRet;
    }
    void* customRet = nullptr;
    AcquireSRWLockShared(&g_lock);
    auto it = g_talkMap.find(id);
    if (it != g_talkMap.end() && !it->second.empty()) {
        customRet = CreateLocalizedString(true, id, it->second);
    }
    ReleaseSRWLockShared(&g_lock);
    return customRet ? customRet : origRet;
}

static uint8_t* AllocateNearModule(uintptr_t baseAddr, size_t size) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    uint64_t gran = si.dwAllocationGranularity ? si.dwAllocationGranularity : 0x10000;

    // Search upward and downward within +/- 1.5GB
    for (uint64_t dist = gran; dist < 0x60000000ULL; dist += gran) {
        if (baseAddr + dist > baseAddr) {
            void* p = VirtualAlloc((void*)(baseAddr + dist), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (p) return (uint8_t*)p;
        }
        if (baseAddr > dist + gran) {
            void* p = VirtualAlloc((void*)(baseAddr - dist), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (p) return (uint8_t*)p;
        }
    }
    return nullptr;
}

static void WriteAbsJump64(uint8_t* dst, uintptr_t target) {
    dst[0] = 0xFF;
    dst[1] = 0x25;
    dst[2] = 0x00;
    dst[3] = 0x00;
    dst[4] = 0x00;
    dst[5] = 0x00;
    *(uint64_t*)(dst + 6) = (uint64_t)target;
}

static bool IsReadableRange(const void* addr, size_t size) {
    uintptr_t a = (uintptr_t)addr;
    uintptr_t end = a + size;
    if (end < a) return false;
    MEMORY_BASIC_INFORMATION mbi;
    while (a < end) {
        if (!VirtualQuery((LPCVOID)a, &mbi, sizeof(mbi))) return false;
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
        a = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    }
    return true;
}

static size_t GetImageSize(uintptr_t base) {
    auto* dos = (IMAGE_DOS_HEADER*)base;
    if (!IsReadableRange(dos, sizeof(IMAGE_DOS_HEADER)) || dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    auto* nt = (IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);
    if (!IsReadableRange(nt, sizeof(IMAGE_NT_HEADERS64))) return 0;
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    return nt->OptionalHeader.SizeOfImage;
}

static void SuspendOtherThreads(std::vector<HANDLE>& suspended) {
    DWORD pid = GetCurrentProcessId();
    DWORD selfTid = GetCurrentThreadId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID == pid && te.th32ThreadID != selfTid) {
                HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (h) {
                    if (SuspendThread(h) != (DWORD)-1) suspended.push_back(h);
                    else CloseHandle(h);
                }
            }
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
}

static void ResumeOtherThreads(std::vector<HANDLE>& suspended) {
    for (HANDLE h : suspended) {
        ResumeThread(h);
        CloseHandle(h);
    }
    suspended.clear();
}

static void FindPatternAll(uintptr_t base, const uint8_t* pat, size_t patLen,
                           std::vector<uint8_t*>& out, size_t maxHits = 64) {
    out.clear();
    auto* dos = (IMAGE_DOS_HEADER*)base;
    if (!IsReadableRange(dos, sizeof(IMAGE_DOS_HEADER)) || dos->e_magic != IMAGE_DOS_SIGNATURE) return;
    auto* nt = (IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);
    if (!IsReadableRange(nt, sizeof(IMAGE_NT_HEADERS64)) || nt->Signature != IMAGE_NT_SIGNATURE) return;
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
        uint8_t* start = (uint8_t*)(base + sec->VirtualAddress);
        size_t sz = sec->Misc.VirtualSize;
        if (sz < patLen) continue;
        for (size_t off = 0; off <= sz - patLen; ++off) {
            if (memcmp(start + off, pat, patLen) == 0) {
                out.push_back(start + off);
                if (out.size() >= maxHits) return;
            }
        }
    }
}

static bool InstallHooks(uintptr_t base) {
    // RVA 0x3D2D20: bs.eug (first 5 bytes: 48 89 5C 24 08 = mov [rsp+8], rbx)
    // RVA 0x3D2E40: bs.euh (first 6 bytes: 40 53 48 83 EC 20 = push rbx; sub rsp, 20h)
    size_t imgSize = GetImageSize(base);
    if (imgSize == 0) {
        LogMsg("ERROR: Failed to read GameAssembly.dll PE headers");
        return false;
    }
    auto inImage = [base, imgSize](const uint8_t* p, size_t n) -> bool {
        uintptr_t a = (uintptr_t)p;
        return a >= base && a + n <= base + imgSize && IsReadableRange(p, n);
    };

    const uint8_t expectedEug[5] = { 0x48, 0x89, 0x5C, 0x24, 0x08 };
    const uint8_t expectedEuh[6] = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20 };

    // Pattern-scan first so the mod keeps working when a game update shifts code.
    // The signature is a unique switch tail inside bs.euh, at +0xAB from its entry.
    const uint8_t euhTailSig[] = {
        0x8B, 0x90, 0xC0, 0x00, 0x00, 0x00,
        0x85, 0xD2, 0x74, 0x2D,
        0x83, 0xEA, 0x01, 0x74, 0x1E,
        0x83, 0xEA, 0x01, 0x74, 0x0F
    };

    uint8_t* pEug = nullptr;
    uint8_t* pEuh = nullptr;
    bool resolved = false;

    std::vector<uint8_t*> hits;
    FindPatternAll(base, euhTailSig, sizeof(euhTailSig), hits);
    std::vector<std::pair<uint8_t*, uint8_t*>> candidates;
    for (uint8_t* tail : hits) {
        if ((uintptr_t)tail < base + 0xAB + 0x120) continue;
        uint8_t* candEuh = tail - 0xAB;
        uint8_t* candEug = candEuh - 0x120;
        if (candEug >= candEuh || !inImage(candEug, 32) || !inImage(candEuh, 32)) continue;
        if (memcmp(candEug, expectedEug, 5) != 0 || memcmp(candEuh, expectedEuh, 6) != 0) continue;
        if (!(inImage(candEuh + 0xA1, 5) && candEuh[0xA1] == 0xE8)) continue;
        int32_t rel = *(int32_t*)(candEuh + 0xA2);
        uintptr_t tgt = (uintptr_t)(candEuh + 0xA6) + (intptr_t)rel;
        if (tgt < base || tgt >= base + imgSize) continue;
        candidates.push_back({ candEug, candEuh });
    }

    if (candidates.size() == 1) {
        pEug = candidates[0].first;
        pEuh = candidates[0].second;
        resolved = true;
        LogMsg("Pattern scan located hooks (unique): bs.eug RVA 0x%llX, bs.euh RVA 0x%llX",
            (unsigned long long)((uintptr_t)pEug - base),
            (unsigned long long)((uintptr_t)pEuh - base));
    } else if (candidates.size() > 1) {
        LogMsg("ERROR: Pattern scan found %zu candidate(s); refusing to guess", candidates.size());
        return false;
    }

    if (!resolved) {
        // Fall back to the last known RVAs (validated before use).
        pEug = (uint8_t*)(base + 0x3D2D20);
        pEuh = (uint8_t*)(base + 0x3D2E40);
        if (!inImage(pEug, 32) || !inImage(pEuh, 32)) {
            LogMsg("ERROR: Hook target RVAs are outside the GameAssembly.dll image");
            return false;
        }
        if (memcmp(pEug, expectedEug, 5) != 0) {
            LogMsg("ERROR: Prologue mismatch at bs.eug: %02X %02X %02X %02X %02X",
                pEug[0], pEug[1], pEug[2], pEug[3], pEug[4]);
            return false;
        }
        if (memcmp(pEuh, expectedEuh, 6) != 0) {
            LogMsg("ERROR: Prologue mismatch at bs.euh: %02X %02X %02X %02X %02X %02X",
                pEuh[0], pEuh[1], pEuh[2], pEuh[3], pEuh[4], pEuh[5]);
            return false;
        }
        LogMsg("Pattern scan inconclusive; using known RVAs");
    }

    // Dynamically resolve get_setting_mgr from the call instruction at pEuh + 0xA1
    if (inImage(pEuh + 0xA1, 5) && pEuh[0xA1] == 0xE8) {
        int32_t relCall = *(int32_t*)(pEuh + 0xA2);
        uintptr_t targetAddr = (uintptr_t)(pEuh + 0xA6) + (intptr_t)relCall;
        if (targetAddr >= base && targetAddr < base + imgSize && IsReadableRange((void*)targetAddr, 1)) {
            g_get_setting_mgr = (get_setting_mgr_t)targetAddr;
            LogMsg("Resolved get_setting_mgr=%p (RVA 0x%llX)",
                (void*)targetAddr, (unsigned long long)(targetAddr - base));
        } else {
            LogMsg("WARN: get_setting_mgr target is outside the image; language detection disabled");
        }
    } else {
        LogMsg("WARN: get_setting_mgr call site not found; language detection disabled");
    }

    uint8_t* stubPage = AllocateNearModule((uintptr_t)pEug, 4096);
    if (!stubPage) {
        LogMsg("ERROR: Failed to allocate trampoline page near GameAssembly.dll");
        return false;
    }

    // Layout in stubPage:
    // [0x00..0x1F]: Trampoline for bs.eug (5 stolen bytes + 14-byte abs jump to pEug+5)
    // [0x20..0x3F]: Relay for bs.eug (14-byte abs jump to Hook_bs_eug)
    // [0x40..0x5F]: Trampoline for bs.euh (6 stolen bytes + 14-byte abs jump to pEuh+6)
    // [0x60..0x7F]: Relay for bs.euh (14-byte abs jump to Hook_bs_euh)
    uint8_t* trampEug = stubPage + 0x00;
    uint8_t* relayEug = stubPage + 0x20;
    uint8_t* trampEuh = stubPage + 0x40;
    uint8_t* relayEuh = stubPage + 0x60;

    memcpy(trampEug, pEug, 5);
    WriteAbsJump64(trampEug + 5, (uintptr_t)(pEug + 5));
    WriteAbsJump64(relayEug, (uintptr_t)&Hook_bs_eug);
    g_orig_bs_eug = (bs_lookup_t)trampEug;

    memcpy(trampEuh, pEuh, 6);
    WriteAbsJump64(trampEuh + 6, (uintptr_t)(pEuh + 6));
    WriteAbsJump64(relayEuh, (uintptr_t)&Hook_bs_euh);
    g_orig_bs_euh = (bs_lookup_t)trampEuh;

    // Save original bytes so a partial failure can be rolled back safely.
    uint8_t origEug8[8];
    uint8_t origEuh8[8];
    memcpy(origEug8, pEug, 8);
    memcpy(origEuh8, pEuh, 8);

    // Build replacement bytes (eug: E9 rel32; euh: E9 rel32 + NOP).
    uint8_t patchEug[8];
    memcpy(patchEug, origEug8, 8);
    int32_t relEug = (int32_t)((intptr_t)relayEug - ((intptr_t)pEug + 5));
    patchEug[0] = 0xE9;
    memcpy(patchEug + 1, &relEug, 4);

    uint8_t patchEuh[8];
    memcpy(patchEuh, origEuh8, 8);
    int32_t relEuh = (int32_t)((intptr_t)relayEuh - ((intptr_t)pEuh + 5));
    patchEuh[0] = 0xE9;
    memcpy(patchEuh + 1, &relEuh, 4);
    patchEuh[5] = 0x90;

    // Apply a patch with all other threads suspended, so no thread can be
    // executing a half-written instruction while the jump is installed.
    auto applyPatch = [](uint8_t* target, const uint8_t* bytes) -> bool {
        DWORD oldProt = 0;
        if (!VirtualProtect(target, 16, PAGE_EXECUTE_READWRITE, &oldProt)) return false;
        std::vector<HANDLE> suspended;
        SuspendOtherThreads(suspended);
        memcpy(target, bytes, 8);
        FlushInstructionCache(GetCurrentProcess(), target, 16);
        ResumeOtherThreads(suspended);
        VirtualProtect(target, 16, oldProt, &oldProt);
        return true;
    };

    if (!applyPatch(pEug, patchEug)) {
        LogMsg("ERROR: VirtualProtect failed on bs.eug");
        return false;
    }
    if (!applyPatch(pEuh, patchEuh)) {
        LogMsg("ERROR: VirtualProtect failed on bs.euh; rolling back bs.eug");
        DWORD oldProt = 0;
        if (VirtualProtect(pEug, 16, PAGE_EXECUTE_READWRITE, &oldProt)) {
            std::vector<HANDLE> suspended;
            SuspendOtherThreads(suspended);
            memcpy(pEug, origEug8, 8);
            FlushInstructionCache(GetCurrentProcess(), pEug, 16);
            ResumeOtherThreads(suspended);
            VirtualProtect(pEug, 16, oldProt, &oldProt);
        }
        return false;
    }

    // Trampoline page no longer needs to be writable.
    DWORD oldStubProt = 0;
    VirtualProtect(stubPage, 4096, PAGE_EXECUTE_READ, &oldStubProt);
    FlushInstructionCache(GetCurrentProcess(), stubPage, 4096);

    LogMsg("Successfully installed hooks! bs.eug=%p, bs.euh=%p, stubPage=%p", pEug, pEuh, stubPage);
    return true;
}

// ============================================================================
// 6. Worker Thread
// ============================================================================
static bool IsForegroundProcess() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

static DWORD ModWorkerThreadBody() {
    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeStr(exePath);
    size_t slash = exeStr.find_last_of(L"\\/");
    std::wstring gameDir = (slash != std::wstring::npos) ? exeStr.substr(0, slash) : L".";
    std::wstring exeName = (slash != std::wstring::npos) ? exeStr.substr(slash + 1) : exeStr;
    if (_wcsicmp(exeName.c_str(), L"UnityCrashHandler64.exe") == 0) {
        return 0; // crash handler also imports VERSION.dll; do nothing here
    }
    g_modDir = gameDir + L"\\JapaneseMod";
    CreateDirectoryW(g_modDir.c_str(), nullptr);

    LogMsg("=== The Piper of Dawn Japanese Localization Mod Initializing ===");
    ReloadTranslations();

    // Wait for GameAssembly.dll & il2cpp_string_new
    HMODULE hGA = nullptr;
    for (int i = 0; i < 600; ++i) {
        hGA = GetModuleHandleW(L"GameAssembly.dll");
        if (hGA) {
            g_il2cpp_string_new = (il2cpp_string_new_t)GetProcAddress(hGA, "il2cpp_string_new");
            if (g_il2cpp_string_new) break;
        }
        Sleep(50);
    }

    if (!hGA || !g_il2cpp_string_new) {
        LogMsg("ERROR: Timed out waiting for GameAssembly.dll / il2cpp_string_new");
        return 0;
    }

    g_gchandle_new = (gchandle_new_t)GetProcAddress(hGA, "il2cpp_gchandle_new");
    g_gchandle_free = (gchandle_free_t)GetProcAddress(hGA, "il2cpp_gchandle_free");
    g_strCacheAvailable = (g_gchandle_new != nullptr && g_gchandle_free != nullptr);
    LogMsg("String cache: %s", g_strCacheAvailable.load() ? "ENABLED" : "unavailable (allocating per lookup)");

    uintptr_t base = (uintptr_t)hGA;
    LogMsg("GameAssembly.dll base=%p, il2cpp_string_new=%p", (void*)base, (void*)g_il2cpp_string_new);

    if (!InstallHooks(base)) {
        return 0;
    }

    // Hotkey loop:
    // F5 = Reload JapaneseMod/*.json
    // F6 = Toggle Mod ON/OFF
    // F7 = Dump recent 128 IDs to JapaneseMod/recent_ids.txt
    bool prevF5 = false, prevF6 = false, prevF7 = false;
    while (true) {
        // Only react while the game window is in the foreground.
        bool fg = IsForegroundProcess();
        bool curF5 = fg && (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
        bool curF6 = fg && (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
        bool curF7 = fg && (GetAsyncKeyState(VK_F7) & 0x8000) != 0;

        if (curF5 && !prevF5) {
            ReloadTranslations();
            MessageBeep(MB_OK);
        }
        if (curF6 && !prevF6) {
            bool nowEnabled = !g_modEnabled.load();
            g_modEnabled.store(nowEnabled);
            LogMsg("Mod toggled: %s", nowEnabled ? "ENABLED" : "DISABLED");
            MessageBeep(nowEnabled ? MB_OK : MB_ICONASTERISK);
        }
        if (curF7 && !prevF7) {
            DumpRecentIds();
            MessageBeep(MB_OK);
        }

        prevF5 = curF5;
        prevF6 = curF6;
        prevF7 = curF7;
        Sleep(100);
    }
    return 0;
}

static DWORD ModWorkerThreadInner() {
    try {
        return ModWorkerThreadBody();
    }
    catch (const std::exception& e) {
        LogMsg("ERROR: exception in worker thread: %s", e.what());
        return 1;
    }
    catch (...) {
        LogMsg("ERROR: unknown exception in worker thread");
        return 1;
    }
}

static DWORD WINAPI ModWorkerThread(LPVOID) {
    __try {
        return ModWorkerThreadInner();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        LogMsg("ERROR: SEH exception in worker thread (0x%08X)", (unsigned)GetExceptionCode());
        return 1;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        // NOTE: the system version.dll is resolved lazily inside the proxy
        // functions (InitOnce), not here, to avoid LoadLibrary under the loader lock.
        HANDLE hThread = CreateThread(nullptr, 0, ModWorkerThread, nullptr, 0, nullptr);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}
