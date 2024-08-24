#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "que_to_eng.h"

/**
 i* Generates a random set of suffixes.
 *
 * Array contents: (all but 3 could also be empty)
 * 	0: ri, rpa, rpari
 *n 	1: ku, mu, kamu
 * 	2: chka, lla, chkalla
 * 	3: ni, nki, n, yku, nchik, nkichik, nku
 * 	4: ña, raq
 * 	5: pis, taq, sina, puni
 *
 */
const char **get_random_suffixes() {
	/* all possible suffixes */
	char *options[][7] = {
		{"ri", "rpa", "rpari", ""},
		{"ku", "mu", "kamu", ""},
		{"chka", "lla", "chkalla", ""},
		{"ni", "nki", "n", "yku", "nchik", "nkichik", "nku"},
		{"ña", "raq", ""},
		{"pis", "taq", "sina", "puni", ""}
	};

	/* I was gonna use an array but this is cooler */
	const int OPTION_LENS = 0b101011111100100100;

	const size_t SUFFIX_CT = 6;
	size_t i, j;
	const char **result = calloc(SUFFIX_CT, sizeof(char *));

	for (i = 0; i < SUFFIX_CT; i++) {
		j = rand() % (OPTION_LENS >> 3*i & 7);
		result[i] = options[i][j];
	}

	return result;
}

/**
 * -------------------QUECHUA TO ENGLISH TRANSLATOR-------------------
 *  Generate random suffix combinations to test against the program.
 *   ___                  _
 *  / _ \ _   _  ___  ___| |__  _   _  __ _
 * | | | | | | |/ _ \/ __| '_ \| | | |/ _` |
 * | |_| | |_| |  __/ (__| | | | |_| | (_| |
 *  \__\_\\__,_|\___|\___|_| |_|\__,_|\__,_|
 *  _
 * | |_ ___
 * | __/ _ \
 * | || |_| |
 *  \__\___/
 *  _____             _ _     _
 * | ____|_ __   __ _| |_|___| |__
 * |  _| | '_ \ / _` | | / __| '_ \
 * | |___| | | | (_| | | \__ \ | | |
 * |_____|_| |_|\__, |_|_|___/_| |_|
 *              |___/
 *
 * Author: Alec Kingsley
 *
 */
int main(int argc, char *argv[]) {
	/* set random seed */
	srand(time(NULL));
	const char *roots[] = {"t'usu", "puklla", "wayk'u", "awa", "llamk'a"};
	const char **suffixes = get_random_suffixes();
	size_t i;
	const int SUFFIX_CT = 6;
	const char *root = roots[rand() % (sizeof(roots) / sizeof(roots[0]))];
	char *translation = translate(root, suffixes);

	/* print original Quechua word */
	printf("Quechua word: %s", root);
	for (i = 0; i < SUFFIX_CT; i++) {
		printf("%s",suffixes[i]);
	}
	printf("\n");

	/* translate and print */
	printf("Translation: %s\n", translation);

	/* clean up and leave */
	free(suffixes);
	free(translation);
	return 0;
}

