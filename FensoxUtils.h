#pragma once
#include <random>
#include <cctype>
#include <locale>
#include <tuple>
#include <SDL.h>
#include <iostream>
#include <sstream>

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

    // Static function to convert a string to uppercase
    static inline std::string strToUpper(std::string str) {
        // convert string to upper case
        std::for_each(str.begin(), str.end(), [](char& c) {
            c = ::toupper(c);
        });
        return str;
    }

    /* Used to allow std::string to be used in a switch statement. Found this on Stack Overflow.
     * Use: 
     *           switch( hash(str) ) {
     *               case hash("one"): // do something
     *               case hash("two"): // do something
     *           }
    */
    static inline constexpr unsigned int hash(const char* s, int off = 0) {
        return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
    }

    // Takes a string of 4 comma delimited int values and converts them into an SDL_Rect. Returns bool success/failure and an SDL_Rect as a tuple.
    static inline std::tuple<bool, SDL_Rect> getRectFromCDV(std::string strCDV) {
        bool success{ true };
        int intRect[4]{ -1, -1, -1, -1 }; // temp array to hold SDL_Rect values. In a later check if any of these are left as -1 our load in failed.

        // convert string parameter to a stream and loop through grabbing int values and storing in temp array
        if (!strCDV.empty()) {
            std::istringstream stream(strCDV);
            std::string strTmp;
            int count{ 0 };
            while ((std::getline(stream, strTmp, ',')) && (count < 4)) {
                //convert substring we got into an int and catch any errors
                try {
                    intRect[count] = std::stoi(strTmp);
                    ++count;
                } catch (const std::exception& e) {
                    std::cerr << "Failed in FensoxUtils::getRectFromCDV converting str to int.\nError: " << e.what() << std::endl;
                    success = false;
                    break;
                }
            }
        } else {
            std::cerr << "Failed in FensoxUtils::getRectFromCDV: function string parameter is empty." << std::endl;
            success = false;
        }

        // check if all four ints have been set
        if (success) {
            for (int i{ 0 }; i < 4; ++i) {
                if (intRect[i] == -1) {
                    std::cerr << "Failed in FensoxUtils::getRectFromCDV: Paramater string not formatted correctly." << std::endl;
                    success = false;
                }
            }
        }

        // build our SDL_Rect
        SDL_Rect rect{};
        if (success) rect = { intRect[0], intRect[1], intRect[2], intRect[3] };

        return std::tuple<bool, SDL_Rect> {success, rect};
    }

    // Takes in two comma-delimited int values in a string, parses them, and returns them in an SDL_Point object.
    static inline SDL_Point getPointFromCDV(std::string str) {
        // convert passed in string into a stream and read the first value before comma
        int xy[2]{};
        std::string strInt{};
        std::istringstream ss{ std::istringstream{str} };
        int count{ 0 };
        while ((std::getline(ss, strInt, ',')) && (count < 2)) {
            // Attempt to convert substring we got into an int and catch any errors
            try {
                xy[count] = std::stoi(strInt);
                ++count;
            } catch (const std::exception& e) {
                std::cerr << "Failed in FensoxUtils::getPointFromCDV converting str to int.\nError: " << e.what() << std::endl;
                break;
            }
        }

        return SDL_Point{ xy[0], xy[1] };
    }
};