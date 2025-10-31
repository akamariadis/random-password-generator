# random-password-generator
This C++ code generates a random password based on user-selected criteria. It first tries to obtain secure random numbers from the operating system, using Windows CryptoAPI (CryptGenRandom) or /dev/urandom on Unix systems. If that fails, it falls back to a weaker random generator using rand().
