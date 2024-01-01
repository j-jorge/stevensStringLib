/**
 * @file stevensStringLib.h
 * @author Jeff Stevens (jeff@bucephalusstudios.com)
 * @brief An easy-to-use C++ library with convenient functions for working with strings.
 * @version 0.1
 * @date 2023-12-21
 * @copyright Copyright (c) Bucephalus Studios 2023
 *
 * Created and released under the MIT license. See the LICENSE file in the repository folder.
 *
 * If you'd like to buy me a coffee or send me a tip for my work on this library, you can do that here: https://ko-fi.com/bucephalus_studios
 *
 * Thank you very, very much for being interested in my code! Hope it serves you well <3
 */

// JJO: The include guards don't need a comment, they are self-explanatory.
//Include guard
// JJO: as per §3.1 from [lex.name], the standard says that each identifier that
// begins with an underscore followed by an uppercase letter is reserver to the
// implementation for any use. This applies to preprocessing tokens too, so you
// should not do that for your include guards.
#ifndef _STEVENSSTRINGLIB_
#define _STEVENSSTRINGLIB_

#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<limits>
#include<sstream>
#include<fstream>
#include<cctype> // JJO: This one seems unnecessary.
#include<locale>
#include<charconv>
#include<map>
#include<unordered_map>


namespace stevensStringLib
{
    // JJO: This is where the required C++ version for your lib is important. If
    // you require C++23 as suggested by the comment in the test file, then this
    // function is useless; the caller should use str.contains(substring). If it
    // is a legacy from a time when your library required a previous version of
    // C++, then I would suggest to add a [[deprecated]] here.
    /**
     * Given a string, determine whether it has an occurrence of the substring somewhere
     * within it.
     *
     *  @param str - The string we are examining to see if it contains the substring.
     *  @param substring - The substring we are trying to see if it is contained in str.
     *
     * JJO: You should not name the type of the return value in the
     * documentation, unless it is not clear from the function
     * signature. Indeed, the information is already present in the code and
     * won't be out of sync if the code changes.
     *
     *  @retval bool - Boolean indicating that input string contains the substring (true) or not (false).
     */
    bool contains(  const std::string & str,
                    const std::string & substring   )
    {
        // JJO: std::string::find returns a std::string::size_type, which is an
        // unsigned integer type, so not an int.
        int result = str.find(substring);
        // JJO: result is necessarily >= 0, as stated above. Anyway, the whole
        // function could be reduced to
        //   return str.find(substring) != std::string::npos.
        if((result >= 0) && (result <= str.length()))
        {
            return true;
        }
        else if(result == std::string::npos)
        {
            return false;
        }
        else
        {
            // JJO: You are sending strings and ints in a std::ostring_stream,
            // you should use operator<<, not operator+:
            // std::cout << "…" << str << ',' << substring << … << result;
            // When you concatenate strings like below you are actually
            // allocating and copying many strings. This can be simpler.
            std::cout << "stevensStringLibrary notification: Use of contains(" + str + "," + substring + ") has returned an unexpected value: " + std::to_string(result);
            return false;
        }
    }


    /**
     * Separates a string by a separator substring. Returns a vector of strings that
     * were separated by the separator substring.
     *
     * Example:
     *
     * std::vector<std::string> result = separate( "John,Gina,Sebastian,Nick", ",")
     *
     * //Value of result is: {"John","Gina","Sebastian","Nick"}
     *
     *  @param str - The string we intend to separate with this function.
     *  @param separator - The substring of str we intend to separate it by.
     *  @param omitEmptyStrings - If true, do not include empty strings in the returned vector.
     *
     *  @retval vector<std::string> - A vector of substrings of the original string that have been split up by all occurrences of the separator parameter
     */
    // JJO: If you use C++20 or later, separator should be a
    // std::string_view. Also, without calling into question the need for
    // this specific interface, I'm pretty sure that a version working on
    // std::string_view would quickly find its use.
    std::vector<std::string> separate(  const std::string & str,
                                        const std::string & separator = ",",
                                        // JJO: passing integrals as const&
                                        // arguments is inefficient, there will
                                        // be an indirection in the emitted
                                        // assembly to get its value. If you had
                                        // passed this argument by value it
                                        // would have also fit in a register and
                                        // its value would have been immediately
                                        // available.
                                        const bool & omitEmptyStrings = true)
    {
        // JJO: These initializations do nothing more than the default
        // constructor, they should be removed to lighten the code.
        std::vector<std::string> separatedStrings = {};
        std::string word = "";

        // JJO: It's a good idea to have a separate implementation for the
        // case where the separator is a single character. To push a bit
        // further you should put this in another function that the caller
        // could use directly.

        //If we have a single character separator, we can use a more efficient method
        if(separator.length() == 1)
        {
            // JJO: this tokenizer is short, for sure, but it is also
            // inefficient. Just with the std::istringstream you already have a
            // full copy of str, then another copy for each part by
            // std::getline, then another copy with the call to push_back! If
            // you use C++23, you should use std::ispanstream. Anyway, I would
            // suggest to implement the algorithm using a loop calling
            // str.find(separator[0], …) repeatedly.

            //Taken from https://stackoverflow.com/a/5757851
            std::istringstream split(str);
            for (std::string each; std::getline(split, each, separator[0]); separatedStrings.push_back(each));
            //Check to see if the last element needs a separator character trimmed off (if there exists a trailing separator character)
            if(!separatedStrings.empty())
            {
                // JJO: How can we have a separator[0] in the separatedStrings?
                // The calls to std::getline above would have removed all of
                // them.
                if(separatedStrings.back().back() == separator[0])
                {
                    separatedStrings.back().pop_back();
                }
            }
        }
        else
        {
            //Iterate through the input string and piece together the strings we want to separate
            for(int i = 0; i < str.length(); i++)
            {
                // JJO: Just as above, this implementation is quite
                // inefficient. You should start with a loop calling
                // str.find(separator, …) repeatedly, and copy the segments
                // jumped over by each call. Unless you have MBs of text between
                // two separators, I would be surprised if it would not help :)
                word += str[i];
                //Every time we add a character to the word, we check to see if it contains the separator
                // JJO: Can the characters from word[0] to word[i-1] change
                // between two iterations of the loop? Nope. If the separator is
                // in word then it is at the very end, so you should compare
                // with this range only.
                if(contains(word, separator))
                {
                    //Extract the separator from the word
                    // JJO: you already did word.find(separator) in the call to
                    // contains above. It's unfortunate to do it again
                    // here. Moreover, you know where the separator is: at the
                    // end of word.
                    word.erase(word.find(separator), separator.length());
                    //Push the word into the vector
                    separatedStrings.push_back(word);
                    //Clear the word
                    word.clear();
                }
            }
            //If we had no separators at all, or if the string doesn't end in a separator, we push the remaining
            //word to the vector.
            separatedStrings.push_back(word);
        }

        //If we are omitting empty strings in our result, erase all of that we found
        if(omitEmptyStrings)
        {
            separatedStrings.erase(std::remove(separatedStrings.begin(), separatedStrings.end(), ""), separatedStrings.end());
        }

        return separatedStrings;
    }


    /**
     * @brief Alias of separate()
     */
    // JJO: separate is a nice function name, just by its name I can guess what
    // it does. sep is a bad function name. Nobody needs aliases like this, it
    // obfuscates the code.
    std::vector<std::string> sep(   const std::string & str,
                                    const std::string & separator = ",",
                                    const bool & omitEmptyStrings = true    )
    {
        return separate(str, separator, omitEmptyStrings);
    }


    /**
     *  Returns a string with the first letter capitalized. If the string is empty, then we just return the empty string.
     *
     *  @param str - The string we want to capitalize the first letter of.
     *
     *  @retval std::string - The string str with the first letter capitalized.
     */
    std::string cap1stChar(std::string input)
    {
        if(input.length() > 0)
        {
            input[0] = std::toupper(input[0]);
        }
        return input;
    }


    /**
     * Returns a string with all characters in uppercase if possible.
     *
     * Credit to this article: https://www.geeksforgeeks.org/conversion-whole-string-uppercase-lowercase-using-stl-c/
     *
     * @param  str - The string we would like to make all uppercase.
     *
     * @retval string - The parameter str, but all in uppercase!
     *
    `*/
    // JJO: This function takes its argument by reference, it modifies the
    // argument passed by the caller, but it also copies the argument! Something
    // is wrong here. Either the argument is modified, and the function should
    // return void, or else it is expected to return the string in upper case
    // and then the argument should be const&.
    std::string toUpper(std::string & str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char x) { return std::toupper(x); });
        return str;
    }


    /**
     * Detects if a string is in the form of a valid C++ integer.
     *
     * @param str - A string we are checking to see if it represents an integer.
     *
     * @retval bool - true if the string str represents an integer, false otherwise.
    */
    bool isInteger( const std::string & str )
    {
        for (int charIndex = 0; charIndex < str.length(); charIndex++)
        {
            //The first index has the possibility of having a negative sign
            if(charIndex == 0)
            {
                if(str[charIndex] == '-')
                {
                    continue;
                }
            }
            //Check to see if the character is a numerical digit
            if (!isdigit(str[charIndex]))
            {
                return false;
            }
        }

        //Now that we're sure we have a string that contains only digits and possibly a negative sign, we check
        //to see if converting the string to an integer will cause errors

        // JJO: This initialization is useless, std::from_chars will overwrite
        // it on success, and it does not matter on error.
        int value = 0;
        // JJO: What is the point of the loop above if you call std::from_chars?
        // The latter will do the check done in this loop.
        const auto res = std::from_chars(   str.data(),
                                            str.data() + str.size(),
                                            value );
        if (res.ec == std::errc::invalid_argument)
        {
            // JJO: This comment adds nothing that is not already said by the
            // condition above.

            //Invalid argument for std::from_chars()
            return false;
        }
        else if (res.ec == std::errc::result_out_of_range)
        {
            // JJO: Same, this comment adds nothing that is not already said by
            // the condition above.

            //Overflow/underflow has occurred
            return false;
        }

        //Otherwise, return true
        return true;
    }


    /**
     * Detects if a string is in the form of a valid c++ floating point number.
     *
     * @param str - A string we are checking to see if it represents a floating point number.
     *
     * @retval bool - true if the string str represents a floating point number. False otherwise.
    */
    bool isFloat( const std::string & str )
    {
        //Let's keep track of if we've seen the decimal point or not
        bool seenDecimalPoint = false;
        char point = std::use_facet< std::numpunct<char> >(std::cout.getloc()).decimal_point();
        for (int charIndex = 0; charIndex < str.length(); charIndex++)
        {
            //The first index has the possibility of having a negative sign
            if(charIndex == 0)
            {
                if(str[charIndex] == '-')
                {
                    continue;
                }
            }
            if(!seenDecimalPoint)
            {
                if(str[charIndex] == point)
                {
                    seenDecimalPoint = true;
                    continue;
                }
            }
            //Check to see if the character is a numerical digit
            if(!isdigit(str[charIndex]))
            {
                return false;
            }
        }

        // JJO: This is a loose description of a floating point number. How 1 is
        // not a valid floating point number? It even has an exact
        // representation as a float! Also, what about something like 1.e03?

        //If we've looked at the whole string and not found a decimal point, then we couldn't be looking at a floating point number.
        if(!seenDecimalPoint)
        {
            return false;
        }

        // JJO: Same comments as for isInteger: the work done by the loop above
        // is done by the call to std::from_chars below, thus the loop is
        // superfluous. Even if you want to ensure that there is a dot in the
        // string, I think it should be verified after the call to
        // std::from_chars.

        //Now that we're sure we have digits with a decimal point, and possibly a negative sign in front, we convert the string to
        //a floating point number using std::from_chars to see if we run into any problems.
        double value = 0;
        const auto format = std::chars_format::general;
        const auto res = std::from_chars(   str.data(),
                                            str.data() + str.size(),
                                            value,
                                            format  );
        if (res.ec == std::errc::invalid_argument)
        {
            //Invalid argument
            return false;
        }
        else if (res.ec == std::errc::result_out_of_range)
        {
            //Overflow/unverflow has occurred
            return false;
        }

        //Otherwise, return true
        return true;
    }



    /**
     *  Detects whether or not the user input is in the form of an integer or floating point number.
     *  Does not work for mathematical expressions!
     *
     * JJO: it does not work for mathematical expressions, nor for numbers
     * written in letters, nor with Roman numbers, nor… Well, there is no need
     * to tell what are the non-supported cases. Just describe the function with
     * something like "… floating point number, written with digits only."
     *
     *  @param str - The string we are checking to see if it represents a number.
     *
     *  @retval bool - True if the string represents a number. False if otherwise.
     *
     */
    bool isNumber(  const std::string & str   )
    {
        return ( isInteger(str) || isFloat(str) );
    }


    /**
     * Takes in a string and checks to see if it is a representation of the word "true" or "0".
     * In those cases, return a true bool. In all other cases, return false.
     *
     * JJO: The description is incorrect. The function return true if the
     * argument is a representation of "true" or **if it would not result in the
     * number zero when converted to integer**.
     *
     * @param str - A string we are converting to a bool.
     *
     * @retval bool - True if str is a form of the word true or 0, and false otherwise.
    */
    // JJO: And suddenly: snake_case. All the functions up to here use
    // camelCase, you should stay consistent in your interfaces (and use
    // camel_case, obviously :)).
    // Also, why is the argument passed as non-const reference?
    bool string_to_bool( std::string & str )
    {
        // JJO: A function like string_to_bool should not modify its argument :(
        if( toUpper(str) == "TRUE")
        {
            return true;
        }
        else if( isNumber(str) )
        {
            // JJO: The call to isNumber as already scanned the string, **and**
            // has converted it into an integer or float via
            // std::from_chars. Here we are doing a third scan of the string.
            // It's a lot! Moreover, the string "0.1" would pass the test
            // "isNumber" and the call to std::stoi would return 0 for it. The
            // string would thus be considered equivalent to false, yet it is
            // not zero. Is it expected?
            return !(std::stoi(str) == 0);
        }
        return false;
    }


    /**
     * Converts a boolean value to a string value.
     *
     * Taken from OJ's answer here: https://stackoverflow.com/questions/29383/converting-bool-to-text-in-c
     *
     * @param input - The boolean value we wish to convert to a string.
     *
     * @retval std::string - The boolean value represented as a string.
     */
    // JJO: As stated before, the argument should be passed by value. There are
    // more occurrences of this in the file, I will not point them all.
    std::string bool_to_string( const bool & input  )
    {
        return input ? "true" : "false";
    }


    /**
     * Given a string and integer X, erase (trim) X characters from both the beginning and end of the string.
     *
     * @param str - A string we would like to trim the characters from.
     * @param charsToTrim - The number of characters to trim from the beginning and end of the string.
     *
     * @retval std::string - A modified version of the string str, with a number characters from both the beginning and end of the
     *                  string trimmed off equal to charsToTrim.
     */
    std::string trim(   const std::string & str,
                        const int & charsToTrim   )
    {
        //We don't accept negative numbers of characters to trim

        // JJO: If you don't accept negative number, use an unsigned type for
        // the argument.
        if(charsToTrim < 0)
        {
            //TODO: Work out how to do error handling
            return str;
        }
        //If we have a charsToTrim value greater than half the the length of the input string, we return an empty string
        if(charsToTrim >= (str.length()/2))
        {
            return "";
        }

        return str.substr(charsToTrim, (str.length() - (charsToTrim * 2)));
    }


    /**
     * Removes all tabs, spaces, newlines, and anything else from a string that is defined as whitespace in the current locale.
     *
     * Learn what is defined as whitespace: https://en.cppreference.com/w/cpp/string/byte/isspace
     * Credit to Michael Steller: https://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
     *
     * @param str - The string from which we wish to remove all the whitespace from.
     *
     * @retval std::string - The input string but with all of the whitespace removed
     */
    std::string removeWhitespace( std::string str )
    {
        str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char x) { return std::isspace(x); }), str.end());
        return str;
    }


    /**
     * TODO: Make one mapify function for std::map and std::unordered_map. This can be done with templates, somehow. Need to do research.
     * TODO: Problem!!! Data could potentially contain separator strings. We need to prevent this, possibly by requiring JSON formatting?
     *
     * Given an input string str that can represent a map<std::string,std::string, take two separator strings and
     * separate the pairs from eachother, and the keys and values from each other. Then
     * insert each key value pair into a std::map<std::string,std::string> object.
     *
     * @param str - The string we would like to convert into a map<std::string,std::string>.
     * @param keyValueSeparator - The string in str using to separate keys from values.
     * @param pairSeparator - The string in str we are using separate pairs.
     * @param ignoreWhitespace - Bool indicating if we should remove any whitespace from str before creating the map.
     *
     * @retval T<std::string,std::string> - A map object of string-string key-value pairs.
    */
    // JJO: The three std::string parameters should be passed as const&. Here
    // you are requiring a copy from the caller's arguments.
    std::map<std::string,std::string> mapifyString( std::string str,
                                                    std::string keyValueSeparator,
                                                    std::string pairSeparator,
                                                    bool ignoreWhitespace = true    )
    {
        //Create the map that we'll be returning later

        // JJO: The initialization is useless, the default constructor does the
        // job. There are more occurrences of this in the file, I won't comment
        // them all.
        std::map<std::string,std::string> mappedString = {};

        //Get rid of any whitespace if necessary (TODO: see if we can just remove whitespace between words-equals signs, and words-commas)
        if(ignoreWhitespace)
        {
            // JJO: I don't buy this feature. If the caller wants to remove the
            // whitespaces, they should call removeWhitespace themselves on the
            // argument before calling mapifyString.
            str = removeWhitespace(str);
        }

        //Separate the pairs
        std::vector<std::string> keysAndValues = separate(str,pairSeparator);

        //Separate the keys and values
        std::vector<std::string> keyAndValue;
        for(int i = 0; i < keysAndValues.size(); i++)
        {
            keyAndValue = separate(keysAndValues[i], keyValueSeparator);
            // JJO: This if should be omitted, the case is already handled by
            // the following conditions.
            if(keyAndValue.size() == 0)
            {
                //No keys or values found, skip
                continue;
            }
            else if(keyAndValue.size() == 1)
            {
                //Insert a key with an empty value
                mappedString[keyAndValue[0]] = "";
            }
            else if(keyAndValue.size() >= 2)
            {
                //Insert the key value pair into the map (only the first two elements)
                mappedString[keyAndValue[0]] = keyAndValue[1];
            }
        }

        return mappedString;
    }


    /**
     * Given an input string str that can represent a map, take two separator strings and
     * separate the pairs from eachother, and they keys and values from each other. Then
     * insert each key value pair into a std::map<std::string,std::string> object.
     *
     * JJO: The description is incorrect, the key-value pairs are inserted into
     * a std::unordered_map.
     *
    */
    std::unordered_map<std::string,std::string> unordered_mapifyString( std::string str,
                                                                        std::string keyValueSeparator = ":",
                                                                        std::string pairSeparator  = ",",
                                                                        bool ignoreWhitespace = true    )
    {
        //Create the map that we'll be returning later
        std::unordered_map<std::string,std::string> unordered_mappedString = {};

        //Get rid of any whitespace if necessary (TODO: see if we can just remove whitespace between words-equals signs, and words-commas)
        if(ignoreWhitespace)
        {
            str = removeWhitespace(str);
        }

        //Separate the pairs
        std::vector<std::string> keysAndValues = separate(str,pairSeparator);

        //Separate the keys and values
        std::vector<std::string> keyAndValue;
        for(int i = 0; i < keysAndValues.size(); i++)
        {
            keyAndValue = separate(keysAndValues[i], keyValueSeparator);
            if(keyAndValue.size() == 0)
            {
                //No keys or values found, skip
                continue;
            }
            else if(keyAndValue.size() == 1)
            {
                //Insert a key with an empty value
                unordered_mappedString[keyAndValue[0]] = "";
            }
            else if(keyAndValue.size() >= 2)
            {
                //Insert the key value pair into the map (only the first two elements)
                unordered_mappedString[keyAndValue[0]] = keyAndValue[1];
            }
        }

        return unordered_mappedString;
    }


    /**
     * Given an unordered_map of strings, turn it into a string of keys and values paired together separated by delimiting characters.
     *
     * Parameters:
     *  unordered_map<std::string,std::string> umap - The unordered map with string keys and values which we intend to turn into a string.
     *  std::string keyValueSeparator - The string that separates keys from their values in the returned string.
     *  std::string pairSeparator - The string that separates key-value pairs in the returned string.
     *
     * Returns:
     *  std::string - The all contents of the unordered map turned into a string list of separated key-value pairs.
    */
    // JJO: Arguments should be const&. There are other occurrences of this in
    // the file, I won't point them all.
    std::string stringifyUnordered_map( std::unordered_map<std::string,std::string> umap,
                                        std::string keyValueSeparator = ":",
                                        std::string pairSeparator =     "," )
    {
        std::string stringifiedUmap = "";
        //Iterate through the unordered map, appending each pair to the string

        // JJO: Loop with for (const auto& …) since you don't modify the key nor
        // the value.
        for(auto & [key,value] : umap )
        {
            if(!stringifiedUmap.empty())
            {
                stringifiedUmap += pairSeparator;
            }
            stringifiedUmap += key + keyValueSeparator + value;
        }
        return stringifiedUmap;
    }


    /**
     * Given a string, count how many lines are in that string and return the integer count.
     * Solution from: https://stackoverflow.com/questions/3482064/counting-the-number-of-lines-in-a-text-file
     *
     * TODO: Concerned with line counting problems presented here: https://stackoverflow.com/a/843484/16511184
     *
     * Parameter:
     *  std::string str - The string which we wish to count the number of lines of.
     *
     * Returns:
     *  int - The integer count of the number of lines that the string str has.
    */
    int countLines(std::string str)
    {
        // JJO: too many copies of the string in this algorithm: one copy to
        // pass the argument, another one to pass it to the std::istringstream,
        // then a segmented copy for each call to std::getline. It's a lot!
        // Especially since the function can be reduced to
        // return std::count(str.begin(), str.end(), '\n').
        int number_of_lines = 0;
        std::string line;
        std::istringstream ss(str);

        while (std::getline(ss, line))
        {
            ++number_of_lines;
        }
        return number_of_lines;
    }


    /**
     * Given the path to a file, count how many lines are in the file and return the integer count.
     *
     * Parameter:
     *  std::string filePath - The path to the file we want to count the number of lines of.
     *
     * Returns:
     *  int - The integer number of lines that the file contains.
    */
    int countFileLines(const std::string & filePath)
    {
        //Create an input stream from the file we are trying to print
        std::ifstream input_file(filePath);
        if (!input_file.is_open())
        {
            //Error if we cannot successfully print the file
            throw std::invalid_argument("Error, could not find file: " + filePath);
        }
        // JJO: Here you have a stream (the input_file), then you turn it into a
        // std::string, then you call countLines which… turns the string into a
        // stream! At the very least, you should have worked with the stream you
        // have at hand, but again: std::count.

        //Store the text content of the file in a string
        std::string fileContent = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

        return countLines(fileContent);
    }


    /**
     * Given a string and integer describing the total number of characters that can exist in a line of text,
     * wrap the text by adding newlines between words so it may fit within a certain width.
     *
     * Parameters:
     *  std::string str - The string which we wish to wrap to a certain width.
     *  int wrapWidth - The width in number of characters we wish to wrap.
     *
     * Returns:
     *  std::string - A modified version of the parameter str, with newlines added to it so that it fits within
     *                a certain character width.
     *
    */
    // JJO: The wrapWidth parameter should be unsigned, or at least tested for
    // positivity. A negative argument would lead to bad things here.
    std::string wrapToWidth(    std::string str,
                                int wrapWidth   )
    {
        std::istringstream in(str);
        std::string line;
        std::string word;
        std::string output = "";
        int lineCutOffIndex = 0;
        int numberOfLines = countLines(str);
        int currLineNum = 0;

        while(getline(in,line))
        {
            lineCutOffIndex = 0;
            while(true)
            {
                //Check to see if we need to wrap this line around
                if(line.length() > wrapWidth)
                {
                    //Find the last space in the string that fits on the line

                    // JJO: Use the char version, since you search a character:
                    // line.rfind(' ', wrapWidth)
                    lineCutOffIndex = line.rfind(" ", wrapWidth);
                    //If we can't find a space in the string and the string is too long, we just cut the line and wrap to the next line

                    // JJO: it seems that you could have used one code for both
                    // branches:
                    //   std::string::size_type break_index =
                    //     std::min(wrapWidth, line.rfind(' ', wrapWidth));
                    // Then use break_index to split the string.
                    if(lineCutOffIndex == std::string::npos)
                    {
                        //Add as much of the line as we can to the output and then add a newline

                        // JJO: Save a few temporary strings and dynamic
                        // allocations by using std::string::append. e.g:
                        // output.append(line, 0, wrapWidth);
                        // output += '\n';

                        output += line.substr(0, wrapWidth) + "\n";
                        //Continue looping until the rest of the line is added to the output

                        // JJO: Save a few temporary strings and dynamic
                        // allocations by working directly on line:
                        // line.erase(0, wrapWidth). Or maybe just keep track of
                        // a cursor in line and don't modify liny at all :)
                        line = line.substr(wrapWidth+1);
                    }
                    //If we find a space...
                    else
                    {
                        //Add the part of the string before the cut off point to the output
                        output += line.substr(0, lineCutOffIndex) + "\n";
                        //Set the line equal to the cut off portion of the string and continue looping until the rest of the line is added to the output
                        line = line.substr(lineCutOffIndex+1);
                    }
                }
                else
                {
                    output += line;
                    break;
                }
            }
            currLineNum++;
            //Add a new line when printing the next line from the string, or print a newline if we only have a newline character
            // JJO: Can't we handle that without the expensive call to
            // countLines? I guess we can tell if the current line is the last
            // one by testing in.eof(), and we can keep a count of the number of
            // lines locally.
            if((currLineNum < numberOfLines) || ((line.length()) == 0 && (numberOfLines == 1)))
            {
                output += "\n";
            }
        }

        return output;
    }


    /**
     * JJO: an an -> and an.
     *
     * Given a string an an integer representing an index, return a single character from the string by the process of circular
     * indexing.
     *
     * Circular indexing is performed by indexing over the string from left to right. Once the index exceeds the length of the string, we begin
     * indexing again from the lefthand side of the string and repeat the process until we stop at the final index. We return the character at
     * that final index.
     *
     * Example: circularIndex("Hello world!", 13) returns 'e', as we loop around to 'H' at 12 and index one space further to reach 'e'.
     *
     * JJO: The parameters are already listed in the signature, no need to list
     * them again in a comment. Or else put a descriptive comment :)
     *
     * Parameters:
     *  std::string str
     *  int circ_i
     *
     * Returns:
     *  char - A character found in str that has been circularly indexed to at position circ_i
     *
     *
     * TODO: Create aliases ci and circ_i
    */
    char circularIndex( std::string str,
                        int circ_i )
    {
        //If we have an empty string, we can't index to anything
        // JJO: Yes, the program will crash with an empty string. Also, what if
        // circ_i is negative?
        // if(str.empty())
        // {
        //     throw std::invalid_argument("str cannot be empty for circularIndex()");
        // }

        return str[circ_i % str.length()];
    }


    /**
     * Given a string str, erase the last n characters of the string.
     *
     * Taken from: https://thispointer.com/remove-last-n-characters-from-a-string-in-c/
     *
     * Parameters:
     *  std::string str - The string we want to erase characters from the end of.
     *  int n - The number of characters we want to erase
     *
     * Returns:
     *  std::string - The string str having n characters erased from the end of it.
    */
    std::string eraseCharsFromEnd(  std::string str,
                                    int n   )
    {
        //Can't erase from empty string
        // JJO: This should be if (str.size() >= n)
        if(str.empty())
        {
            return "";
        }

        str.erase(str.size() - n);
        return str;
    }


    /**
     * Given a string str, find all occurrences of a substring within it. Returns a vector of all of the indices that the substring
     * occurs at within the string str.
     *
     * Based off of: https://stackoverflow.com/a/4034809
     *
     * Parameters:
     *  std::string str - The string we are searching for the substring in.
     *  std::string substr - The substring we are looking for within string str.
     *
     * Returns:
     *  std::vector<size_t> - A vector containing all indices in increasing order that the substr occurs at.
    */
    std::vector<size_t> findAll(    std::string str,
                                    std::string substr  )
    {
        std::vector<size_t> positions;

        size_t pos = str.find(substr, 0);
        while(pos != std::string::npos)
        {
            positions.push_back(pos);
            pos = str.find(substr,pos+1);
        }

        return positions;
    }


    /**
     * Given a locale, return all of the whitespace characters for that locale in a string.
     *
     * Credit: https://stackoverflow.com/a/36311304/16511184
     *
     * Parameter:
     *  const std::locale & loc - The locale which we want to obtain a string of all whitespace characters for.
     *
     * Returns:
     *  std::string - A string of all whitespace characters defined in the given locale.
    */
    std::string getWhitespaceString(const std::locale & loc)
    {
        std::string whitespace;
        for (char ch = std::numeric_limits<char>::min(); ch < std::numeric_limits<char>::max(); ch++)
            if (std::isspace(ch, loc))
                whitespace += ch;
        // to avoid infinte loop check char max outside the for loop.
        if (std::isspace(std::numeric_limits<char>::max(), std::locale(loc)))
            whitespace += std::numeric_limits<char>::max();
        return whitespace;
    }


    /**
     * Remove all leading and trailing whitespace from a string (spaces, tabs, newlines, etc.), then return it.
     *
     * Based on: https://stackoverflow.com/a/1798170
     *
     * Parameter:
     *  std::string str - The string to remove all of the leading and trailing whitespaces from.
     *
     * Returns:
     *  std::string - str modified by removing all of its leading and trailing whitespaces.
    */
    std::string trimWhitespace( std::string str )
    {
        //Get a string of all of the whitespace characters in the current locale
        std::string whitespace = getWhitespaceString(std::locale(""));

        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
        {
            return ""; // no content
        }

        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }


    //trimAllOf


    /**
     * Given a character c, convert it to a std::string.
     *
     * Parameter:
     *  char c - The character we'd like to convert to a string.
     *
     * Returns:
     *  std::string - A string representing the character c.
    */
    std::string char_to_string( const char & c )
    {
        std::string s(1, c);
        return s;
    }


    /**
     * Predicate function for eraseNonNumericChars. It performs a logical not operation on the result of the isdigit() function.
     *
     * Parameter:
     *  const char & c - The character we'd like to check to see if it is not a digit.
     *
     * Returns:
     *  bool - True if c is not a digit, false otherwise.
    */
    bool isNotDigit(    const char & c  )
    {
        return !(isdigit(c));
    }


    /**
     * Erases all non-numeric characters from a string and returns it.
     *
     * Parameter:
     *  std::string str - The string to release all non-numeric chars from.
     *
     * Returns:
     *  The string parameter we passed in, but all of its non-numeric characters removed from it.
    */
    std::string eraseNonNumericChars( std::string str )
    {
        // JJO: I don't see the point of isNotDigit, a lambda would be perfect here.
        str.erase(std::remove_if(str.begin(), str.end(), isNotDigit), str.end());
        return str;
    }


    /**
     * Reverses the order of a string's characters using std::reverse().
     *
     * Parameter:
     *  std::string & str - The string we would like to reverse.
     *
     * Returns:
     *  std::string - The reversed string.
    */
    std::string reverse( std::string str )
    {
        std::reverse(str.begin(),str.end());
        return str;
    }


    /**
     * Checks to see if a std::string is a palindrime or not (the reversed order of characters equals the original order of characters).
     * Note well that character case, spacing, and punctuation present in classic English palindromes like "A man, a plan, a canal, panama"
     * prevent them from being valid C++ palindromes with this function, as the reverse order of this exact string is "amanap ,lanac a ,nalp a ,nam A" is
     * not equivalent to the original order of characters.
     *
     * Credit: https://stackoverflow.com/a/8362657/16511184
     *
     * Parameter:
     *  std::string str - The string we would like to check.
     *
     * Returns:
     *  bool - true if str is a palindrome, false otherwise.
    */
    bool isPalindrome( const std::string & str )
    {
        return std::equal(str.begin(), str.begin() + str.size()/2, str.rbegin());
    }


    //replace


    //eraseAll


    //scramble


    //negativeIndex
    //with aliases ni / neg_i

}
#endif
