#include <stdlib.h>
#include <stdio.h>
#include "controller.h"
#include "translator.h"


/**
 * Reads in data from FILENAME (expected: ring_data.txt).
 *
 * Replaces:
 * zero_distances, avg_differences
 *
 * Parameters:
 * zero_distances - shortest distance from sensor to ring
 * avg_differences - average difference between two adjacent ring spaces
 * RING_CT - length of previous two arrays (should be equal)
 *
 */
void get_data(float *zero_distances[], float *avg_differences, const size_t RING_CT) {
	const char FILENAME[] = "ring_data.txt";
	FILE *file;

	/* exit if file could not be found */
	if ((file = fopen(FILENAME, "r")) == NULL) {
		printf("Could not find %s. Please run data_collector before this program.\n", FILENAME);
		exit(1);
	}

	/* collect data from file if it exists */
	for (size_t ring = 0; ring < RING_CT; ring++) {
		if (fscanf("%f %f", &((*zero_distances)[ring]), &((*avg_differences)[ring])) != 2) {
			printf("Insufficient data in %s\n", FILENAME);
			exit(1);
		}
	}


	fclose(file);


}

/**
 * Continuously read input from rings and show the word as well as the translation.
 *
 */
int main() {
	/* TODO - Update this to take input as well */
	const char[] ROOT = "wayk'u";
	/* # of rings in use */
	const size_t RING_CT = 6;
	float *zero_distances = calloc(RING_CT, sizeof(float));
	float *avg_differences = calloc(RING_CT, sizeof(float));
	const float MIN_ULTRASONIC_CM = 2.0;
	const float MAX_ULTRASONIC_CM = 400.0;
	/* all suffixes */
	const char *SUFFIXES[][7] = {
		{"ri", "rpa", "rpari", ""},
		{"ku", "mu", "kamu", ""},
		{"chka", "lla", "chkalla", ""},
		{"ni", "nki", "n", "yku", "nchik", "nkichik", "nku"},
		{"ña", "raq", ""},
		{"pis", "taq", "sina", "puni", ""}
	};
	/* # of suffixes per ring */
	const size_t SUFFIX_CTS[] = {4, 4, 4, 7, 3, 5};
	/* indeces within rings */
	size_t ring_idxs[] = {0, 0, 0, 0, 0, 0};
	
	/* setup pins for raspberry pi */
	setup();

	/* get data which should be saved from data collection program */
	get_data(&zero_distances, &avg_differences);

	/* main loop */
	/* TODO? Add way to exit. Maybe not necessary. */
	while (1) {
		/* get rings */
		char *rings[RING_CT];
		for (size_t ring = 0; ring < RING_CT; ring++) {
			float distance_cm = measure_ring_cm(ring);
			if (MIN_ULTRASONIC_CM < distance_cm && distance_cm < MAX_ULTRASONIC_CM) {
				size_t ring_pos = 0;
				while ((ring_pos + 0.5) * avg_differences[ring] < distance_cm - zero_distances[ring]) {
					ring_pos++;
				}
				/* only update if in range */
				if (ring_pos < SUFFIX_CTS[ring]) {
					ring_idxs[ring] = ring_pos;
				}
				rings[ring] = SUFFIXES[ring][ring_idxs[ring]];
			}
		}
		char *translation = translate(ROOT, rings);

		/* print original Quechua word */
		printf("Quechua word: %s", ROOT);
		for (size_t ring = 0; ring < SUFFIX_CT; ring++) {
			printf("%s",rings[ring]);
		}
		printf("\n");

		/* translate and print */
		printf("Translation: %s\n", translation);

        	/* clean up */
        	free(translation);
	}

	/* clean up and exit */
	free(zero_distances);
	free(avg_differences);
	return 0;
}


