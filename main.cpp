#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>
#include <algorithm>
#include <stdexcept>

using namespace std;

#ifdef _WIN32
  #define NOMINMAX
  #include <windows.h>
  #include <wincrypt.h>
#endif

bool get_entropy_uint32(uint32_t &out) {
#ifdef _WIN32
    HCRYPTPROV prov = 0;
    if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        return false;
    }
    BOOL ok = CryptGenRandom(prov, sizeof(out), reinterpret_cast<BYTE*>(&out));
    CryptReleaseContext(prov, 0);
    return ok == TRUE;
#else
    ifstream urandom("/dev/urandom", ios::in | ios::binary);
    if (!urandom) return false;
    urandom.read(reinterpret_cast<char*>(&out), sizeof(out));
    return urandom.good();
#endif
}

uint32_t fallback_prng() {
    static bool seeded = false;
    if (!seeded) {
        auto now = chrono::high_resolution_clock::now().time_since_epoch().count();
        unsigned seed = static_cast<unsigned>(now ^ (uintptr_t)&now);
        srand(seed);
        seeded = true;
    }
    uint32_t a = static_cast<uint32_t>(rand() & 0xFFFF);
    uint32_t b = static_cast<uint32_t>(rand() & 0xFFFF);
    return (a << 16) | b;
}

uint32_t random_uint32() {
    uint32_t v;
    if (get_entropy_uint32(v)) return v;
    return fallback_prng();
}

uint32_t uniform_uint32(uint32_t range) {
    if (range == 0) throw invalid_argument("range must be > 0");
    const uint64_t max_uint32 = static_cast<uint64_t>(numeric_limits<uint32_t>::max());
    uint64_t limit = (max_uint32 / range) * range;
    while (true) {
        uint32_t r = random_uint32();
        if (static_cast<uint64_t>(r) < limit) {
            return static_cast<uint32_t>(r % range);
        }
    }
}

string generate_password(
    size_t length,
    bool use_lower = true,
    bool use_upper = true,
    bool use_digits = true,
    bool use_symbols = true,
    bool avoid_ambiguous = false
)

{
    const string lower = "abcdefghijklmnopqrstuvwxyz";
    const string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const string digits = "0123456789";
    const string symbols = "!@#$%^&*()-_+=[]{}<>?/|:;.,";
    const string ambiguous = "Il1O0`'\" ,;:.|\\/";
    vector<string> pools;
    if (use_lower) pools.push_back(lower);
    if (use_upper) pools.push_back(upper);
    if (use_digits) pools.push_back(digits);
    if (use_symbols) pools.push_back(symbols);
    if (pools.empty()) throw invalid_argument("At least one character class must be selected");
    string alphabet;
    for (const auto &p : pools) alphabet += p;
    if (avoid_ambiguous) {
        string filtered;
        for (char c : alphabet) {
            if (ambiguous.find(c) == string::npos) filtered.push_back(c);
        }
        alphabet.swap(filtered);
        for (auto it = pools.begin(); it != pools.end();) {
            string filteredPool;
            for (char c: *it) if (ambiguous.find(c) == string::npos) filteredPool.push_back(c);
            if (filteredPool.empty()) it = pools.erase(it);
            else { *it = move(filteredPool); ++it; }
        }
        if (alphabet.empty()) throw runtime_error("No characters available after removing ambiguous characters.");
    }
    if (length < pools.size()) {
        throw invalid_argument("Length too short to include at least one of each requested character class.");
    }
    string pw; pw.resize(length);
    size_t pos = 0;
    for (const auto &p : pools) {
        uint32_t idx = uniform_uint32(static_cast<uint32_t>(p.size()));
        pw[pos++] = p[idx];
    }
    for (; pos < length; ++pos) {
        uint32_t idx = uniform_uint32(static_cast<uint32_t>(alphabet.size()));
        pw[pos] = alphabet[idx];
    }

    for (size_t i = length - 1; i > 0; --i) {
        uint32_t j = uniform_uint32(static_cast<uint32_t>(i + 1));
        swap(pw[i], pw[j]);
    }
    return pw;
}

int main(int argc, char** argv) {
    unsigned SIZEOFPASSWORD;
    cin >> SIZEOFPASSWORD;
    try {
        size_t length = SIZEOFPASSWORD;
        bool lower = true, upper = true, digits = true, symbols = true, avoid_amb = true;
        for (int i = 1; i < argc; ++i) {
            string a = argv[i];
            if (a == "-n" && i+1 < argc) { length = stoul(argv[++i]); }
            else if (a == "--no-symbols") symbols = false;
            else if (a == "--no-upper") upper = false;
            else if (a == "--no-lower") lower = false;
            else if (a == "--no-digits") digits = false;
            else if (a == "--allow-ambiguous") avoid_amb = false;
            else if (a == "-h" || a == "--help") {
                cout << "Usage: pwgen [-n LENGTH] [--no-upper] [--no-lower] [--no-digits] [--no-symbols] [--allow-ambiguous]" << endl;
                return 0;
            }
        }
        string pass = generate_password(length, lower, upper, digits, symbols, avoid_amb);
        cout << pass << endl;
        return 0;
    } catch (const std::exception &ex) {
        cerr << "Error: " << ex.what() << endl;
        return 2;
    }
}