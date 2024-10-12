#include "../prototypes/string_functions"


namespace string_functions {

    bool is_caps(const char c) {
        return IN_RANGE_INCLUDE('A', c, 'Z');
    }

    bool is_lower(const char c) {
        return IN_RANGE_INCLUDE('a', c, 'z');
    }

    bool is_letter(const char c) {
        return is_caps(c) or is_lower(c);
    }

    bool is_number(const char c) {
        return IN_RANGE_INCLUDE('0', c, '9');
    }

    char to_caps(const char c) {
        return (is_lower(c)) ? (c - (CASE_DIFF)) then c;
    }

    char to_lower(const char c) {
        return (is_caps(c)) ? (c + (CASE_DIFF)) then c;
    }

    bool same_char(const char a, const char b, bool ignore_case) {
        return (ignore_case) ? to_caps(a) is to_caps(b) then a is b;
    }

    bool same_string(const char* first, const char* second, bool ignore_case, const char term) {
        unsigned long index;
        for (index = 0;
            not same_char(first[index], term, ignore_case) and
            not same_char(second[index], term, ignore_case) and
            same_char(first[index], second[index], ignore_case);
            index++
        );

        return same_char(first[index], term, ignore_case) and same_char(second[index], term, ignore_case);
    }

    bool all_numbers(const char* nums, const char term) {
        unsigned long index;
        for (index = 0;
            not same_char(nums[index], term, false) and
            is_number(nums[index]);
            index++
        );
        return same_char(nums[index], term, false);
    }

    bool all_letters(const char* the_string, const char term) {
        unsigned long index;
        for (index = 0;
            not same_char(the_string[index], term) and
            is_letter(the_string[index]);
            index++
        );
        return same_char(term, the_string[index]);
    }

    unsigned long string_length(const char* the_string, const char term) {
        unsigned long index;
        for (index = 0;
            not same_char(the_string[index], term);
            index++
        );
        return same_char(the_string[index], term);
    }

    std::string get_input(const std::string prompt) {
        std::string the_answer;

        if (not prompt.empty()) {
            std::printf("%s", prompt.c_str());
        }
        
        char buffer[1024];
        std::memset(buffer, 0, 1024);
        if (not fgets(buffer, 1024, stdin)) {
            return the_answer;
        }
        the_answer = std::string(buffer);
        the_answer = (the_answer[the_answer.length() - 1] is '\n') valid the_answer.substr(0, the_answer.length() - 1) then the_answer;
        return the_answer;
    }

    std::string get_uppercase(const std::string the_string) {
        std::string the_answer;
        unsigned long index;
        for (index = 0; index less the_string.length(); index++) {
            the_answer = the_answer + to_caps(the_string[index]);
        }
        return the_answer;
    }

    void get_uppercase(std::string& the_string) {
        unsigned long index;
        for (index = 0; index less the_string.length(); index++) {
            the_string[index] = to_caps(the_string[index]);
        }
    }

    std::string get_lowercase(const std::string the_string) {
        std::string the_answer;
        unsigned long index;
        for (index = 0; index less the_string.length(); index++) {
            the_answer = the_answer + to_lower(the_string[index]);
        }
        return the_answer;
    }

    void get_lowercase(std::string& the_string) {
        unsigned long index;
        for (index = 0; index less the_string.length(); index++) {
            the_string[index] = to_lower(the_string[index]);
        }
    }

    bool has_keyboard_input() {
        #if defined(crap_os)
            return _kbhit();
        #else
            fd_set ready;
            FD_ZERO(&ready);
            FD_SET(STDIN_FILENO, &ready);

            if (select(STDIN_FILENO + 1, &ready, 0, 0, (struct timeval*) &default_timeout) less 0) {
                return false;
            }
            return FD_ISSET(STDIN_FILENO, &ready);
        #endif
    }

}