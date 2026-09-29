#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

#pragma comment(lib, "user32.lib")

// ============================================================================
// 1. System version.dll Proxy Forwarding
// ============================================================================
static HMODULE g_hSysVersion = nullptr;
static FARPROC g_pfnVersion[17] = { nullptr };

static void InitVersionProxy() {
    if (g_hSysVersion) return;
    wchar_t sysDir[MAX_PATH] = { 0 };
    GetSystemDirectoryW(sysDir, MAX_PATH);
    std::wstring path = std::wstring(sysDir) + L"\\version.dll";
    g_hSysVersion = LoadLibraryW(path.c_str());
    if (!g_hSysVersion) return;

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
    for (int i = 0; i < 17; ++i) {
        g_pfnVersion[i] = GetProcAddress(g_hSysVersion, names[i]);
    }
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
static bool g_modEnabled = true;

static void LogMsg(const char* fmt, ...) {
    if (g_modDir.empty()) return;
    std::wstring logPath = g_modDir + L"\\mod.log";
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
    int64_t val = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    out = (int32_t)(neg ? -val : val);
    return true;
}

static void SkipJsonValue(const char*& p, const char* end) {
    SkipWs(p, end);
    if (p >= end) return;
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
}

static size_t LoadJsonFileIntoMap(const std::wstring& path, std::unordered_map<int32_t, std::string>& targetMap) {
    FILE* fp = _wfopen(path.c_str(), L"rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0) { fclose(fp); return 0; }

    std::string buf((size_t)sz, '\0');
    fread(&buf[0], 1, (size_t)sz, fp);
    fclose(fp);

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
                int32_t id = (int32_t)atoi(key.c_str());
                if (id != 0 || key == "0") {
                    targetMap[id] = std::move(val);
                    ++count;
                }
            } else {
                SkipJsonValue(p, end);
            }
        }
    }
    return count;
}

static void ReloadTranslations() {
    std::unordered_map<int32_t, std::string> newLang;
    std::unordered_map<int32_t, std::string> newTalk;
    newLang.reserve(8192);
    newTalk.reserve(45000);

    size_t c1 = LoadJsonFileIntoMap(g_modDir + L"\\Language.json", newLang);
    size_t c1_patch = LoadJsonFileIntoMap(g_modDir + L"\\override_Language.json", newLang);
    size_t c2 = LoadJsonFileIntoMap(g_modDir + L"\\LanguageTalk.json", newTalk);
    size_t c2_patch = LoadJsonFileIntoMap(g_modDir + L"\\override_LanguageTalk.json", newTalk);

    AcquireSRWLockExclusive(&g_lock);
    g_langMap.swap(newLang);
    g_talkMap.swap(newTalk);
    ReleaseSRWLockExclusive(&g_lock);

    LogMsg("Loaded translations: Language=%zu (+%zu overrides), LanguageTalk=%zu (+%zu overrides)",
        c1, c1_patch, c2, c2_patch);
}

// ============================================================================
// 4. Recent Lookup Ring Buffer (for F7 Debug Dump)
// ============================================================================
struct RecentEntry {
    bool isTalk;
    int32_t id;
};
static RecentEntry g_recentRing[128];
static volatile LONG g_recentIdx = 0;

static void RecordRecent(bool isTalk, int32_t id) {
    LONG idx = InterlockedIncrement(&g_recentIdx) & 127;
    g_recentRing[idx].isTalk = isTalk;
    g_recentRing[idx].id = id;
}

static void DumpRecentIds() {
    std::wstring outPath = g_modDir + L"\\recent_ids.txt";
    FILE* fp = _wfopen(outPath.c_str(), L"wb");
    if (!fp) return;
    fprintf(fp, "=== Recent Localization IDs (Newest Last) ===\n");
    LONG cur = g_recentIdx;
    AcquireSRWLockShared(&g_lock);
    for (int i = 127; i >= 0; --i) {
        const auto& e = g_recentRing[(cur - i) & 127];
        if (e.id == 0) continue;
        const char* table = e.isTalk ? "LanguageTalk" : "Language";
        const auto& mp = e.isTalk ? g_talkMap : g_langMap;
        auto it = mp.find(e.id);
        const char* txt = (it != mp.end()) ? it->second.c_str() : "<not in mod json>";
        fprintf(fp, "[%s] ID=%d : %s\n", table, e.id, txt);
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

static il2cpp_string_new_t g_il2cpp_string_new = nullptr;
static bs_lookup_t g_orig_bs_eug = nullptr;
static bs_lookup_t g_orig_bs_euh = nullptr;
static get_setting_mgr_t g_get_setting_mgr = nullptr;

// Check if in-game language is set to Japanese (1)
// In bs.eug/bs.euh: call 0x519E20 -> [rax + 0xC0] == 1 for Japanese (0=CN, 1=JP, 2=TW, 3=EN)
static bool IsGameLanguageJapanese() {
    if (!g_get_setting_mgr) return true;
    uintptr_t mgr = g_get_setting_mgr(nullptr);
    if (!mgr) return true;
    int32_t lang = *(int32_t*)(mgr + 0xC0);
    return (lang == 1);
}

static void* Hook_bs_eug(int32_t id, const void* method) {
    void* origRet = g_orig_bs_eug(id, method);
    if (!origRet) return origRet;
    RecordRecent(false, id);
    if (!g_modEnabled || !g_il2cpp_string_new || !IsGameLanguageJapanese()) {
        return origRet;
    }
    void* customRet = nullptr;
    AcquireSRWLockShared(&g_lock);
    auto it = g_langMap.find(id);
    if (it != g_langMap.end() && !it->second.empty()) {
        customRet = g_il2cpp_string_new(it->second.c_str());
    }
    ReleaseSRWLockShared(&g_lock);
    return customRet ? customRet : origRet;
}

static void* Hook_bs_euh(int32_t id, const void* method) {
    void* origRet = g_orig_bs_euh(id, method);
    if (!origRet) return origRet;
    RecordRecent(true, id);
    if (!g_modEnabled || !g_il2cpp_string_new || !IsGameLanguageJapanese()) {
        return origRet;
    }
    void* customRet = nullptr;
    AcquireSRWLockShared(&g_lock);
    auto it = g_talkMap.find(id);
    if (it != g_talkMap.end() && !it->second.empty()) {
        customRet = g_il2cpp_string_new(it->second.c_str());
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

static uint8_t* FindPatternInModule(uintptr_t base, const uint8_t* pat, size_t patLen) {
    auto* dos = (IMAGE_DOS_HEADER*)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    auto* nt = (IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (sec->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
            uint8_t* start = (uint8_t*)(base + sec->VirtualAddress);
            size_t sz = sec->Misc.VirtualSize;
            if (sz < patLen) continue;
            for (size_t off = 0; off <= sz - patLen; ++off) {
                if (memcmp(start + off, pat, patLen) == 0) {
                    return start + off;
                }
            }
        }
    }
    return nullptr;
}

static bool InstallHooks(uintptr_t base) {
    // RVA 0x3D2D20: bs.eug (first 5 bytes: 48 89 5C 24 08 = mov [rsp+8], rbx)
    // RVA 0x3D2E40: bs.euh (first 6 bytes: 40 53 48 83 EC 20 = push rbx; sub rsp, 20h)
    uint8_t* pEug = (uint8_t*)(base + 0x3D2D20);
    uint8_t* pEuh = (uint8_t*)(base + 0x3D2E40);

    const uint8_t expectedEug[5] = { 0x48, 0x89, 0x5C, 0x24, 0x08 };
    const uint8_t expectedEuh[6] = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20 };

    if (memcmp(pEug, expectedEug, 5) != 0 || memcmp(pEuh, expectedEuh, 6) != 0) {
        // Fallback pattern scan for future game updates where RVAs shift:
        // Unique switch tail in bs.euh at offset +0xAB from pEuh (and pEug is pEuh - 0x120)
        const uint8_t euhTailSig[] = {
            0x8B, 0x90, 0xC0, 0x00, 0x00, 0x00,
            0x85, 0xD2, 0x74, 0x2D,
            0x83, 0xEA, 0x01, 0x74, 0x1E,
            0x83, 0xEA, 0x01, 0x74, 0x0F
        };
        uint8_t* foundTail = FindPatternInModule(base, euhTailSig, sizeof(euhTailSig));
        if (foundTail) {
            pEuh = foundTail - 0xAB;
            pEug = pEuh - 0x120;
            LogMsg("Pattern scan located bs.eug=%p (RVA 0x%llX), bs.euh=%p (RVA 0x%llX)",
                pEug, (unsigned long long)((uintptr_t)pEug - base),
                pEuh, (unsigned long long)((uintptr_t)pEuh - base));
        }
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

    // Dynamically resolve get_setting_mgr from the call instruction at pEuh + 0xA1
    if (pEuh[0xA1] == 0xE8) {
        int32_t relCall = *(int32_t*)(pEuh + 0xA2);
        uintptr_t targetAddr = (uintptr_t)(pEuh + 0xA6) + (intptr_t)relCall;
        g_get_setting_mgr = (get_setting_mgr_t)targetAddr;
        LogMsg("Resolved get_setting_mgr=%p (RVA 0x%llX)",
            (void*)targetAddr, (unsigned long long)(targetAddr - base));
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

    // Patch bs.eug (5 bytes)
    DWORD oldProt = 0;
    if (!VirtualProtect(pEug, 16, PAGE_EXECUTE_READWRITE, &oldProt)) {
        LogMsg("ERROR: VirtualProtect failed on bs.eug");
        return false;
    }
    int32_t relEug = (int32_t)((intptr_t)relayEug - ((intptr_t)pEug + 5));
    pEug[0] = 0xE9;
    *(int32_t*)(pEug + 1) = relEug;
    VirtualProtect(pEug, 16, oldProt, &oldProt);
    FlushInstructionCache(GetCurrentProcess(), pEug, 16);

    // Patch bs.euh (6 bytes: E9 rel32 + 90 nop)
    if (!VirtualProtect(pEuh, 16, PAGE_EXECUTE_READWRITE, &oldProt)) {
        LogMsg("ERROR: VirtualProtect failed on bs.euh");
        return false;
    }
    int32_t relEuh = (int32_t)((intptr_t)relayEuh - ((intptr_t)pEuh + 5));
    pEuh[0] = 0xE9;
    *(int32_t*)(pEuh + 1) = relEuh;
    pEuh[5] = 0x90;
    VirtualProtect(pEuh, 16, oldProt, &oldProt);
    FlushInstructionCache(GetCurrentProcess(), pEuh, 16);

    LogMsg("Successfully installed hooks! bs.eug=%p, bs.euh=%p, stubPage=%p", pEug, pEuh, stubPage);
    return true;
}

// ============================================================================
// 6. Worker Thread
// ============================================================================
static DWORD WINAPI ModWorkerThread(LPVOID) {
    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeStr(exePath);
    size_t slash = exeStr.find_last_of(L"\\/");
    std::wstring gameDir = (slash != std::wstring::npos) ? exeStr.substr(0, slash) : L".";
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
        bool curF5 = (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
        bool curF6 = (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
        bool curF7 = (GetAsyncKeyState(VK_F7) & 0x8000) != 0;

        if (curF5 && !prevF5) {
            ReloadTranslations();
            MessageBeep(MB_OK);
        }
        if (curF6 && !prevF6) {
            g_modEnabled = !g_modEnabled;
            LogMsg("Mod toggled: %s", g_modEnabled ? "ENABLED" : "DISABLED");
            MessageBeep(g_modEnabled ? MB_OK : MB_ICONASTERISK);
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        InitVersionProxy();
        HANDLE hThread = CreateThread(nullptr, 0, ModWorkerThread, nullptr, 0, nullptr);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}
