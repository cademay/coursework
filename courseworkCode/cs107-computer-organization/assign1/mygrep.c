/*
 * File: mygrep.c
 * Student: Cade May
 * -----------------------
 *
 * The 'mygrep' program implements a simplified version of the 'grep' command.
 */

#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LEN 1024

/* 
 * Emphasis: terminal codes are used to control the font characteristics of
 * of the output characters. We will use inverse video to show emphasis.
 * Terminal codes are a bit goopy and nothing worth investigating deeply, 
 * all you need to do is output this OPEN/CLOSE around the characters you want
 * emphasized. See given code for print_with_emphasis() as an example.
 */
#define OPEN_EMPHASIS "\e[7m"
#define CLOSE_EMPHASIS "\e[0m"

/*
 *this recursive method receives an input string, a pattern string, a pattern length, and a pointer to an int
 *which it modifies remotely with respect to the * metacharacter if necessary. this method returns 0 to accept when length == 0
 *it checks for the ^ metacharacter and rejects if the first characters are different
 *it checks for the $ metacharacter and rejects if the input isn't at the null terminator
 *it checks if the next character is a *, and if it is, it recurses only on the input to allow for more characters. here, it 
 *backtracks if necessary in order to account for potential edge cases in the relation of the pattern
 */
static int regex_match(const char *input, const char *pattern, int length, int *patternShiftPtr)
{
	const int accept = 0;
	const int advanceSlider = 1;
	const int autoReject = 2;
	const int acceptDollarSign = 3;

	if (length == 0) return accept;

	char pat = pattern[0];
	char inp = input[0];

	if (pat == '^') {
		pattern++;
		length--;
		pat = pattern[0];
		if (pat != inp && pat != '.') return autoReject;
	}

	if (pat == '$') {
		if (inp == '\0') {
			return acceptDollarSign;
		}
		else {
			return autoReject;
		}
	}

	if (length > 1) {

		char possibleStar = pattern[1];
		if (possibleStar == '*') {

			if ((inp == pat || pat == '.') && inp != '\0') {
				int reserveShift = *patternShiftPtr;
				*patternShiftPtr = *patternShiftPtr + 1;
				int result = regex_match(input + 1, pattern, length, patternShiftPtr);
				if (result == accept) return accept;
				else {
					//if it ends up backtracking, this line sets the pattern shift back to what it was before
					//then it subtracts to in order to account for the impending advancement over the "x*" term
					*patternShiftPtr = reserveShift - 2;
					return regex_match(input, pattern + 2,length - 2, patternShiftPtr); 
				}
			}
			else {
				*patternShiftPtr = *patternShiftPtr - 2;
				return regex_match(input, pattern + 2,length - 2, patternShiftPtr);
			}
		}
	}

	if (inp == '\0') {
		*patternShiftPtr = 0;
		return advanceSlider;
	}
	if (pat != '.' && inp != pat) {
		*patternShiftPtr = 0;	
		return advanceSlider;
	}
	else {
		return regex_match(input + 1, pattern + 1, length - 1, patternShiftPtr);
	}
}


/*
 *this method receives an input string, a pattern string, and an pointer to an end string that it updates remotely
 *this method iterates over each character of input, and at each character from 0-n it checks to see what regex_match returns
 *when called on the pattern string and the input. if regex match returns 0, that means it found a match, so the for loop accepts
 *if regex_match returns 1, starting the match check at that particular character didnt work so it continues
 *if it returns 2, this method rejects the input. if it returns 3, it accepts the input. 
 *when it accepts,returns a string pointer back to print_match, indicating the location where the match begins.
 *it also remotely modifies print_match's end variable, and sets it to a string pointer at the location of pattern length
 *past the beginning of the match. 
 */
static const char *search(const char *input, const char *pattern, const char **endp)
{
	const int accept = 0;
	const int advanceSlider = 1;
	const int autoReject = 2;
	const int acceptDollarSign = 3;

	//accounts for the possible increase in the movement of the pointers with respect to the * character's operation	
	int patternShift = 0;

	int pattern_len = strlen(pattern);

	bool caretPresent = false;
	if (pattern[0] == '^') caretPresent = true;

	for (int i = 0; input[i] != '\0'; i++) {
		int result = regex_match(input + i, pattern, pattern_len, &patternShift);

		if (result == accept) {
			*endp = input + i + pattern_len + patternShift;
			return input + i;		
		}

		if (result == advanceSlider) {
			continue;
		}

		if (result == autoReject) {
			*endp = NULL;
			return NULL;
		}

		//when result == acceptDollar sign, this means it just accepted a string with a $ metacharacter
		//as a result, it needs to check if a carat was present as well. if it was, it needs to modify the length of the match by -2	
		if (result == acceptDollarSign) {
			if (caretPresent){
				*endp = input + i + pattern_len - 2 + patternShift;
			}
			else {
				*endp = input + i + pattern_len  + patternShift;
			}
			return input + i;
		}
	}

	*endp = NULL;
	return NULL;
}

/*
 *This method receives a file string, a start string, and an end string.
 *It performs two basic calculations on the strings to identify the proper dimensions,
 *and then it uses those dimension to print nbefore characters that come before the highlighted 
 *part, then it opens emphasis and prints nmatched characters from the part it should highlight,
 * and then closes the emphasis, successfully highlighting the proper content of the match
 */
static void print_with_emphasis(const char *cur, const char *start, const char *end)
{
	int nbefore = start - cur;
	int nmatched = end - start;
	printf("%.*s%s%.*s%s", nbefore, cur, OPEN_EMPHASIS, nmatched, start, CLOSE_EMPHASIS);
}

/*
 *this method receives a fileLine, a pattern, and a filename
 *this method first sends the fineLine, the pattern, and the address of end to a search method
 *the search method will return the location of a match from pattern:fileLine if one exists
 *the search method also remotely sets the value of the end variable through a ptr dereference
 *if there is a match found by search, search will return something that isn't null, so
 *the if statement in this method will run, calling printWithEmphasis, providing it with
 *this information: the line, the start of the match, and the end of the match
 *this continues in a while loop until the start pointer is null
 */
static void print_match(const char *line, const char *pattern, const char *filename)
{

	bool valid = false;
	bool searching = true;
	while (searching) {

		const char *end = NULL;
		const char *start = search(line, pattern, &end);

		if (start != NULL) {
			if (filename != NULL) printf("%s: ", filename);
			print_with_emphasis(line, start, end);
			line = end;
			valid = true;
		} else {
			if (valid == true) {
				printf("%s\n",line);
				break;
			}
			else {
				break;
			}
		}
	}
}

/*
 *grep_file takes in a file and then runs the contents through a for loop
 *the for loop changes the \n newline command on each line to a terminating \0 character if necessary
 *then it calls print_match on each line, sending print_match the line string, the pattern, and the filename
 */
static void grep_file(FILE *fp, const char *pattern, const char *filename)
{
	char line[MAX_LINE_LEN];

	while (fgets(line, sizeof(line), fp) != NULL) {
		// truncate trailing newline if present (most lines will have one, last line might not)
		if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
		print_match(line, pattern, filename);
	}
}

/*
 *main determines the size of the userInput, and responds accordingly
 *if inputSize < 2, error. if inputSize = 2, main calls grep_file with stdin (standard input)
 *if inputSize > 2, for each item after the first(pattern), it assumes the item is a file and 
 *attempts to open it, then send the filePointer fp to grep_file
 *the question mark in the function call makes it so if the user only enters one file, 
 *then a file name is not sent with the call because it wouldn't be necessary
 */

int main(int argc, char *argv[])
{
	if (argc < 2) error(1, 0, "Usage: grep PATTERN [FILE]...");
	const char *pattern = argv[1];

	if (argc == 2) 
		grep_file(stdin, pattern, NULL);
	else {
		for (int i = 2; i < argc; i++) {
			FILE *fp = fopen(argv[i], "r");
			if (fp == NULL) error(1, 0, "%s: no such file", argv[i]);
			grep_file(fp, pattern, argc > 3 ? argv[i] : NULL);
			fclose(fp);
		}
	}
	return 0;
}

