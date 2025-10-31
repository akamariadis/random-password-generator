# RANDOM-PASSWORD-GENERATOR

The code featured in this repository, builds an alphabet from requested character classes, including lower case and upper characters, digits as well as symbols. It guarantees at least one character from each chosen class, fills the rest randomly, shuffles the result, and prints a password of the requested length. Random values are taken from the OS CSPRNG when available and from a weaker rand() - based fallback otherwise.
