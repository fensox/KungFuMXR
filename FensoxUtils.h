#pragma once
#include <random>
#include <cctype>

/*
    A static utility class with many helpful functions.
    i.e. random number generation, string trimming functionality, etc.
    Begun by Fensox June 2020.
*/
class FensoxUtils {
private:
    // Initializes a static random number generator used in some of the utils functions.
	static std::ranlux48 rneGen;

public:
	// Static function to generate a random integer between the range specified in the parameters. Uses ranlux48 algorithm.
    static int getRandInt(int low, int high) {
		std::uniform_int_distribution<int>  uniform_range(low, high);
		return uniform_range(rneGen);
	}

    // Static function to trim whitespace from the left side of the passed in std::string reference.
    static inline void strTrimLeft(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
    }

    // Static function to trim whitespace from the right side of the passed in std::string reference.
    static inline void strTrimRight(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    // Static function to trim whitespace from both right and left ends of the passed in std::string reference.
    static inline void strTrim(std::string& s) {
        strTrimLeft(s);
        strTrimRight(s);
    }
};