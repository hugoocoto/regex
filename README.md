# Regular Expression Lib

## About

Poorly performing implementation of a regular expression subset.

## Implemented subset

| Metacharacter | Description                                                             |
| :------------ | :---------------------------------------------------------------------- |
| `^`           | Match the starting position of the string                               |
| `[ ]`         | Matches a single char that is contained within the brackets             |
| `[^ ]`        | Matches a single character that is not contained within the brackets    |
| `$`           | Matches the ending position of the string                               |
| `( )`         | Defines a marked subexpression                                          |
| `*`           | Matches the preceding element zero or more times                        |
| `{m, n}`      | Matches the preceding element at least _m_ and not more than _n_ times  |
| `?`           | Matches the preceding element zero or one time                          |
| `+`           | Matches the preceding element one or more times                         |
| `\|`          | Match either the expression before or the expression after the operator |

## No yet implemented 

| Metacharacter | Description                                                                           |
| :------------ | :------------------------------------------------------------------------------------ |
| `\n`          | Matches what the nth marked subexpression matched, where n is a digit between 0 and 9 |

## Why not use `regex.h`?
Use it. It would perform better.


