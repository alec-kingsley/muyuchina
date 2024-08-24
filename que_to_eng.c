#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Return true iff pre is a prefix of str.
 *
 * Parameters:
 * str - main string
 * pre - possible prefix of main string
 *
 */
bool is_prefix(const char *str, const char *pre) {
	return !strncmp(pre, str, strlen(pre));
}

/**
 * Return true iff suf is a suffix of str.
 *
 * Parameters:
 * str - main string
 * suf - possible suffix of main string
 *
 */
bool is_suffix(const char *str, const char *suf) {
	size_t str_len = strlen(str);
	size_t suf_len = strlen(suf);

	if (suf_len > str_len) {
        	return false;
    	}

	return !strcmp(str + str_len - suf_len, suf);
}

/**
 * Returns the English subject from the Quechua suffix.
 * 
 * Parameter:
 * suffix - Quechua subject suffix. Options:
 * 	"ni", "nki", "n", "yku", "nchik", "nkichik", "nku"
 * 	note that in this program, suffixes which accept an object are not allowed.
 *
 */
const char *get_sub(const char *suffix) {
	const size_t SUBJECT_CT = 7;
	const char *suffixes[] = {"ni", "nki", "n", "yku", "nchik", "nkichik", "nku"};
	const char *subjects[] = {"I", "you", "he/she", "we (but not you)", "we (including you)", "you all", "they"};
	for (int i = 0; i < SUBJECT_CT; i++) {
		if (!strcmp(suffixes[i], suffix)) {
			return subjects[i];
		}
	}
	/* this only happens if an error has occured */
	return NULL;
}

/**
 * Conjugate verb according to person, whether it's progressive, and whether
 * movement is involved.
 *
 * Frees:
 * adverb
 *
 * Parameters:
 * verb - standard dictionary form of English verb
 * sub - subject of sentence. Options: "I", "we (including you)", "we (but not you)", "you", "you all", "he/she", "they"
 * progressive - whether it should be progressive
 * movement - whether movement is involved
 * adverb - adverb to be attached to verb
 * pleasure - whether the action is enjoyed rathr than done
 *
 */
char *conjugate(const char *verb, const char *sub, const bool progressive, const bool movement, char *adverb, const bool pleasure, const bool about_to) {
	const size_t MAX_TO_BE_LEN = 4;
	const size_t MAX_TO_GO_LEN = 5;
	const size_t MAX_CONJUGATION_LEN = 100;
	
	/* return value */
	char *conjugation = calloc(MAX_CONJUGATION_LEN + 1, 1);
	
	/* conjugate "to go", "to be", and find possessive variant of subject */
	char *poss = !strcmp(sub, "I") ? "my " :
		is_prefix(sub, "you") ? "your " :
		is_prefix(sub, "we") ? "our " :
		"their ";
	char to_be[strlen(adverb) + MAX_TO_BE_LEN];
	char to_go[MAX_TO_GO_LEN + 1];
	sprintf(to_be,"%s",
		!strcmp(sub, "I") ? "am " :
		!strcmp(sub, "he/she") ? "is " :
		"are ");
	sprintf(to_go,"%s", !strcmp(sub, "he/she") ? "goes " : "go ");

	/* conjugate verb */
	if (pleasure) {
		sprintf(conjugation, "%s%s", adverb, !strcmp(sub, "he/she") ? "likes to " : "like to ");
	} else if (about_to) {
		sprintf(conjugation, "%s%sabout to ", adverb, to_be);
	}
	bool infinitive = pleasure || about_to;
	if (infinitive) {
		sprintf(to_be, "be ");
		sprintf(to_go, "go ");
		adverb[0] = '\0';
	}
	if (movement) {
		if (progressive) {
			sprintf(conjugation + strlen(conjugation), "%s%son %sway to %s", to_be, adverb, poss, verb);
		} else {
			sprintf(conjugation + strlen(conjugation), "%s%sto %s", adverb, to_go, verb);
		}
	} else {
		const char *infinitives[] = {"dance", "play", "cook", "weave", "work"};
		const char *progressives[] = {"dancing", "playing", "cooking", "weaving", "working"};
		const char *thirdpersons[] = {"dances", "plays", "cooks", "weaves", "works"};
		bool found_verb = false;
		for (size_t i = 0; !found_verb && i < sizeof(infinitives) / sizeof(infinitives[0]); i++) {
			if (!strcmp(verb, infinitives[i])) {
				if (progressive) {
					sprintf(conjugation + strlen(conjugation), "%s%s%s", to_be, adverb, progressives[i]);
				} else if (infinitive || strcmp(sub, "he/she")) {
					sprintf(conjugation + strlen(conjugation), "%s%s", adverb, infinitives[i]);
				} else {
					sprintf(conjugation + strlen(conjugation), "%s%s", adverb, thirdpersons[i]);
				}
				found_verb = true;
			}
		}
		if (!found_verb) {
			exit(1);
		}
	}

	free(adverb);
	return conjugation;

}

/**
 * Translates a given root in Quechua with its suffixes to English.
 *
 * Parameters:
 * root - A Quechua root word (t'usuy, pukllay, wayk'uy, away, llamk'ay)
 * suffixes - An array of suffixes with the following options: (or none for all except 3)
 * 	0: ri, rpa, rpari
 * 	1: ku, mu, kamu
 * 	2: chka, lla, chkalla
 * 	3: ni, nki, n, yku, nchik, nkichik, nku
 * 	4: ña, raq
 * 	5: pis, taq, sina, puni
 *
 */
char *translate(const char *root, const char *suffixes[]) {
	const size_t MAX_TRANSLATION_LEN = 250;
	const size_t MAX_ADVERB_LEN = 25;
	char *translation = calloc(MAX_TRANSLATION_LEN + 1, 1);
	char *verb = !strcmp(root, "t'usu") ? "dance" :
		!strcmp(root, "puklla") ? "play" :
		!strcmp(root, "wayk'u") ? "cook" :
		!strcmp(root, "awa") ? "weave" :
		!strcmp(root, "llamk'a") ? "work" :
		"unkwnown";
	const char *sub = get_sub(suffixes[3]);
	const bool has_ku = !strcmp(suffixes[1], "ku") || !strcmp(suffixes[1], "kamu"),
		has_mu = is_suffix(suffixes[1], "mu"),
		has_lla = is_suffix(suffixes[2], "lla"),
		has_raq = !strcmp(suffixes[4], "raq"),
		has_chka = is_prefix(suffixes[2], "chka"),
		has_puni = !strcmp(suffixes[5], "puni"),
		has_pis = !strcmp(suffixes[5], "pis"),
		has_ri = is_suffix(suffixes[0], "ri"),
		has_na = !strcmp(suffixes[4], "ña"),
		has_taq = !strcmp(suffixes[5], "taq");
	bool is_valid = true, is_upset = false, about_to = false;
	char *adverb = calloc(MAX_ADVERB_LEN + 1, 1);

	/* handle invalid combinations */
	if (has_lla && has_puni && has_ri) {
		/* this would combine "like to" with "about to" which doesn't make sense */
		is_valid = false;
	}
	if (has_na && has_ri) {
		/* this would combine "like to" with "already" which doesn't make sense */
		is_valid = false;
	}

	/* TODO - Update these. Simply don't know how to translate. */
	if (has_lla && has_taq && has_na) {
		is_valid = false;
	}
	if (has_lla && has_raq && has_taq) {
		is_valid = false;
	}
	
	/* handle special suffix combinations (pt 1) */
	if (has_puni && !has_lla) {
		sprintf(translation, "of course ");
	} else if (!strcmp(suffixes[5], "sina")) {
		sprintf(translation, "most likely, ");
	} else if (has_taq && !has_lla) {
		sprintf(translation, "but ");
	} else if (has_pis && !has_lla) {
		sprintf(adverb, "also ");
	} else if (has_pis && has_lla) {
		sprintf(translation, "nonetheless ");
	}
	if (has_na && !has_taq && !has_lla) {
		sprintf(adverb + strlen(adverb), "still ");
		is_upset= true;
	}
	if (has_lla && !has_na) {
		if (has_raq) {
			sprintf(adverb + strlen(adverb), "still ");
		} else {
			sprintf(adverb + strlen(adverb), "only ");
		}
		if (has_puni) {
			about_to = true;
		}

	}
	if (has_na && has_taq) {
		is_upset = has_lla;
		about_to = true;
	}

	/* add pronoun and conjugate verb */
	sprintf(translation + strlen(translation), "%s ", sub);
	char *conjugation = conjugate(verb, sub, has_chka, has_mu, adverb, has_ri, about_to);
	sprintf(translation + strlen(translation), "%s", conjugation);
	free(conjugation);
	
	/* handle special suffix combinations (pt 2) */
	if (is_prefix(suffixes[0], "rpa")) {
		sprintf(translation + strlen(translation), " quickly");
	}
	if (has_ku) {
		sprintf(translation + strlen(translation), " alone");
	}
	if (has_raq && !has_lla) {
		sprintf(translation + strlen(translation), " first");
	}
	if ((has_lla || has_na) && has_taq) {
		sprintf(translation + strlen(translation), " again");
	}
	if (has_lla && has_na) {
		sprintf(translation + strlen(translation), " instead");
	}

	/* add ! to signal anger */
	if (is_upset) {
		sprintf(translation + strlen(translation), "!");
	} else {
		sprintf(translation + strlen(translation), ".");
	}

	/* capitalize start of translation */
	if ('a' <= translation[0] && translation[0] <= 'z') {
		translation[0] -= 'a' - 'A';
	}
	if (!is_valid) {
		sprintf(translation, "Invalid input");
	}

	return translation;
}

