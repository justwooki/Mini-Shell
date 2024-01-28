#ifndef TOKENIZER_H
#define TOKENIZER_H

/**
 * Duplicates a token.
 *
 * Returns an exact copy of a string representing a single token.
 */
char *dup_token(char *token);

/**
 * Tokenize a string.
 *
 * Returns an array of string representing the tokens. The last element is NULL.
 */
char **tokenize(char *line);

/**
 * Copy tokens.
 *
 * Returns an exact copy of the given array of strings representing tokens.
 */
char **copy_tokens(char **tokens);

/**
 * Free memory allocated for tokens.
 *
 * Frees the memory allocated for the tokenized strings and the tokens array itself.
 */
void free_tokens(char **tokens);

/**
 * Removes the first token from tokens.
 *
 * Removes the first string from a list of strings representing tokens.
 */
void remove_first_token(char **tokens);


/**
 * Splits tokens.
 *
 * Splits a token at the first delimiter that shows up into two separate tokens and
 * returns the new token that split from the original.
 */
char** split_tokens(char **tokens, char *delimiters);

/**
 * Join tokens.
 *
 * Combines both string arrays into one and returns that new array.
 */
char** join_tokens(char **tokens1, char **tokens2);

#endif /* ifndef TOKENIZER_H */
