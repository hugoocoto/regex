#include <stdio.h>

#include "regex.h"

#define HIDE_PASSED 1
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

static void
test(Regex regex, char *str)
{
        static int done = 0;
        static int passed = 0;
        done++;
        if (!regex_match(regex, str)) {
                printf(RED "Test [%d/%d] Fail: \"%s\" don't match regex \"%s\"\n" RESET, done, passed, str, regex_repr(regex));
        } else {
                passed++;
#if !defined(HIDE_PASSED) || !HIDE_PASSED
                printf(GREEN "Test [%d/%d] Ok\n" RESET, done, passed);
#endif
        }
}

static void
test_not(Regex regex, char *str)
{
        static int done = 0;
        static int passed = 0;
        done++;
        if (regex_match(regex, str)) {
                printf(RED "Test [%d/%d] Fail: \"%s\" match regex \"%s\"\n" RESET, done, passed, str, regex_repr(regex));
        } else {
                passed++;
#if !defined(HIDE_PASSED) || !HIDE_PASSED
                printf(GREEN "Test [%d/%d] Ok\n" RESET, done, passed);
#endif
        }
}

int
main()
{
        /* 01 */ test(regex_compile("a"), "a");
        /* 02 */ test(regex_compile("^a"), "a");
        /* 03 */ test(regex_compile("^$"), "");
        /* 04 */ test(regex_compile("a$"), "a");
        /* 05 */ test(regex_compile("[ab]"), "a");
        /* 06 */ test(regex_compile("[ab]"), "b");
        /* 07 */ test(regex_compile("[^ab]"), "c");
        /* 08 */ test(regex_compile("[^ab]"), "d");
        /* 09 */ test(regex_compile("a[ab]c"), "abc");
        /* 10 */ test(regex_compile("(a)"), "a");
        /* 11 */ test(regex_compile("a(a)"), "aa");
        /* 12 */ test(regex_compile("a(a)b"), "aab");
        /* 13 */ test(regex_compile("."), "a");
        /* 14 */ test(regex_compile("a."), "ab");
        /* 15 */ test(regex_compile(".a"), "ba");
        /* 16 */ test(regex_compile("^a*$"), "a");
        /* 17 */ test(regex_compile("a*$"), "a");
        /* 18 */ test(regex_compile("^a*"), "a");
        /* 19 */ test(regex_compile("a*"), "a");
        /* 20 */ test(regex_compile("a*"), "aaaaaa");
        /* 21 */ test(regex_compile("a*"), "");
        /* 22 */ test(regex_compile("a{0,}"), "");
        /* 23 */ test(regex_compile("a[ab]*c"), "abc");
        /* 24 */ test(regex_compile("a[ab]*c"), "aabbc");
        /* 25 */ test(regex_compile("a[ab]*c"), "ac");
        /* 26 */ test(regex_compile("a[bc]*d$"), "abcbcd");
        /* 27 */ test(regex_compile("a**[bc]*d$"), "abd");
        /* 28 */ test(regex_compile("a([bc])***d$"), "ad");
        /* 29 */ test(regex_compile("a[bc]*d$"), "abbbbbbbd");
        /* 30 */ test(regex_compile("a[bc\\]]"), "a]");
        /* 31 */ test(regex_compile("a{1,2}"), "a");
        /* 32 */ test(regex_compile("a{1,2}"), "aa");
        /* 33 */ test(regex_compile("a{,2}"), "aa");
        /* 34 */ test(regex_compile("a{,2}"), "a");
        /* 35 */ test(regex_compile("a{,2}"), "");
        /* 36 */ test(regex_compile("a{1,}"), "a");
        /* 37 */ test(regex_compile("a{1,}"), "aaa");
        /* 38 */ test(regex_compile("[a-z]"), "b");
        /* 39 */ test(regex_compile("[a-z]"), "a");
        /* 40 */ test(regex_compile("[a-z]"), "z");
        /* 41 */ test(regex_compile("[0-9]"), "3");
        /* 42 */ test(regex_compile("[A-Za-z0-9._%+-]"), "3");
        /* 43 */ test(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "hugo.coto@rai.usc.es");
        /* 44 */ test(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "hugocoto100305@gmail.com");
        /* 45 */ test(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "hugocoto100305+1@gmail.com");
        /* 46 */ test(regex_compile(".at"), "hat");
        /* 47 */ test(regex_compile(".at"), "cat");
        /* 48 */ test(regex_compile(".at"), "bat");
        /* 49 */ test(regex_compile(".at"), "4at");
        /* 50 */ test(regex_compile(".at"), "#at");
        /* 51 */ test(regex_compile(".at"), " at");
        /* 52 */ test(regex_compile("[hc]at"), "hat");
        /* 53 */ test(regex_compile("[hc]at"), "cat");
        /* 54 */ test(regex_compile("[^b]at"), "cat");
        /* 55 */ test(regex_compile("[^bc]at"), "hat");
        /* 56 */ test(regex_compile("\\[.\\]"), "[a]");
        /* 57 */ test(regex_compile("s.*"), "saw");
        /* 58 */ test(regex_compile("a?"), "");
        /* 59 */ test(regex_compile("a?"), "a");
        /* 60 */ test(regex_compile("ab?c"), "abc");
        /* 61 */ test(regex_compile("ab?c"), "ac");
        /* 62 */ test(regex_compile("a+"), "a");
        /* 63 */ test(regex_compile("a+"), "aaa");
        /* 64 */ test(regex_compile("ab+c"), "abc");
        /* 65 */ test(regex_compile("ab+c"), "abbbc");
        /* 66 */ test(regex_compile("a|b"), "a");
        /* 67 */ test(regex_compile("a|b"), "b");
        /* 68 */ test(regex_compile("[ab]|c"), "a");
        /* 69 */ test(regex_compile("[ab]|c"), "b");
        /* 70 */ test(regex_compile("[ab]|c"), "c");
        /* 71 */ test(regex_compile("a([ab]|c)*c"), "abcabc");
        /* 72 */ test(regex_compile("a([ab]|c)*c"), "ac");
        /* 73 */ test(regex_compile("^a+$"), "a");
        /* 74 */ test(regex_compile("^a+$"), "aa");
        /* 75 */ test(regex_compile("^a+$"), "aaa");
        /* 76 */ test(regex_compile("^a+$"), "aaaaa");
        /* 77 */ test(regex_compile("^a+c$"), "ac");
        /* 78 */ test(regex_compile("^a+c$"), "aac");
        /* 79 */ test(regex_compile("^a+c$"), "aaac");
        /* 80 */ test(regex_compile("^a+c$"), "aaaac");
        /* 81 */ test(regex_compile("^ba+c$"), "bac");
        /* 82 */ test(regex_compile("^ba+c$"), "baac");
        /* 83 */ test(regex_compile("^ba+c$"), "baaac");
        /* 84 */ test(regex_compile("^ba+c$"), "baaaac");
        /* 85 */ test(regex_compile("a{,}"), "");
        /* 86 */ test(regex_compile("a{0,0}"), "");
        /* 87 */ test(regex_compile("a{,0}"), "");
        /* 88 */ test(regex_compile("a{0,}"), "");
        /* 89 */ test(regex_compile("a{0,1}"), "");
        /* 90 */ test(regex_compile("a{0,1}"), "a");
        /* 91 */ test(regex_compile("^a{1,1}$"), "a");

        /* 01 */ test_not(regex_compile("a"), "");
        /* 02 */ test_not(regex_compile("a"), "b");
        /* 03 */ test_not(regex_compile("^a"), " a");
        /* 04 */ test_not(regex_compile("^$"), "a");
        /* 05 */ test_not(regex_compile("a$"), "a ");
        /* 06 */ test_not(regex_compile("[ab]"), "c");
        /* 07 */ test_not(regex_compile("^[ab]$"), "ab");
        /* 08 */ test_not(regex_compile("^[^ab]$"), "a");
        /* 09 */ test_not(regex_compile("^[^ab]$"), "b");
        /* 10 */ test_not(regex_compile("^a[ab]c$"), "ac");
        /* 11 */ test_not(regex_compile("^(a)"), "");
        /* 12 */ test_not(regex_compile("^a(a)"), "a");
        /* 13 */ test_not(regex_compile("^a(a)b$"), "ab");
        /* 14 */ test_not(regex_compile("^.$"), "aa");
        /* 15 */ test_not(regex_compile("^.$"), "");
        /* 16 */ test_not(regex_compile("^a.$"), "a");
        /* 17 */ test_not(regex_compile("^.a$"), "a");
        /* 18 */ test_not(regex_compile("^.a$"), "aaa");
        /* 19 */ test_not(regex_compile("^a[ab]*c$"), "aabcc");
        /* 20 */ test_not(regex_compile("^a[ab]*c$"), "bc");
        /* 21 */ test_not(regex_compile("^a([bc])***d$"), "aabcd");
        /* 22 */ test_not(regex_compile("^a[]$"), "a]");
        /* 23 */ test_not(regex_compile("^a{1,2}$"), "");
        /* 24 */ test_not(regex_compile("^a{1,2}$"), "aaa");
        /* 25 */ test_not(regex_compile("^a{,2}$"), "aaa");
        /* 26 */ test_not(regex_compile("^a{1,}$"), "");
        /* 27 */ test_not(regex_compile("^a{2,6}$"), "a");
        /* 28 */ test_not(regex_compile("^[a-z]$"), "A");
        /* 29 */ test_not(regex_compile("^[a-z]$"), "Z");
        /* 30 */ test_not(regex_compile("^[a-z]$"), "0");
        /* 31 */ test_not(regex_compile("^[0-9]$"), "");
        /* 32 */ test_not(regex_compile("^[A-Za-z0-9._%+-]$"), "@");
        /* 33 */ test_not(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "@rai.usc.es");
        /* 34 */ test_not(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "hugocoto100305@.com");
        /* 35 */ test_not(regex_compile("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"), "hugocoto100305@gmail");
        /* 36 */ test_not(regex_compile("^\\[.\\]$"), "[]");
        /* 37 */ test_not(regex_compile("^a?$"), "b");
        /* 38 */ test_not(regex_compile("^a?$"), "aa");
        /* 39 */ test_not(regex_compile("^ab?c$"), "abbc");
        /* 40 */ test_not(regex_compile("^a+$"), "");
        /* 41 */ test_not(regex_compile("^ab+c$"), "ac");
        /* 42 */ test_not(regex_compile("^a|b$"), "c");
        /* 43 */ test_not(regex_compile("^a|b$"), "ab");
        /* 44 */ test_not(regex_compile("^[ab]|c$"), "ac");
        /* 45 */ test_not(regex_compile("^a([ab]|c)*c$"), "abcdc");
        /* 46 */ test_not(regex_compile("^a+$"), "");
        /* 47 */ test_not(regex_compile("^a+c$"), "c");
        /* 48 */ test_not(regex_compile("^a"), " a");
        /* 49 */ test_not(regex_compile("a$"), "a ");
        /* 50 */ test_not(regex_compile("^a$"), " a ");
        /* 51 */ test_not(regex_compile("^ba+c$"), "bc");
        /* 52 */ test_not(regex_compile("^a{0,1}$"), "aa");
        /* 53 */ test_not(regex_compile("^a{1,1}$"), "aa");
        /* 54 */ test_not(regex_compile("^a{1,1}$"), "");

        return 0;
}
