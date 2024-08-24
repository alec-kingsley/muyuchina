#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include "controller.h"

/**
 * Fills measurements array with POINTS_TO_COLLECT of measurements.
 *
 * Parameters:
 * measurements - array to fill
 * POINTS_TO_COLLECT - # of points to collect
 * RING - ring index from center
 *
 */
void get_measurements(float *measurements[], const size_t POINTS_TO_COLLECT, const size_t RING) {
	/* time between data collection in seconds */
	const float SEC_BETWEEN_DATA_COLLECT = 0.1;
	const int MICROSEC_IN_SEC = 1000000;

	/* collect data */
	printf("Collecting data...\n");
	for (int i = 0; i < POINTS_TO_COLLECT; i++) {
		(*measurements)[i] = measure_ring_cm(RING);
		usleep(SEC_BETWEEN_DATA_COLLECT * MICROSEC_IN_SEC);
	}
}

/**
 * Returns the median of the array arr of length LEN.
 *
 * Parameters:
 * arr - array to find the median of
 * LEN - length of the array
 *
 */
float get_median(float arr[], const int LEN) {
	/* bubble sort array */
	for (size_t i = 0; i < LEN - 1; i++) {
		for (size_t j = i + 1; j < LEN; j++) {
			if (arr[j] > arr[i]) {
				float temp = arr[j];
				arr[j] = arr[i];
				arr[i] = temp;
			}
		}
	}

	/* return average of middle value(s) */
	return LEN % 2 == 0 ? (arr[LEN / 2 - 1] + arr[LEN / 2]) / 2 : arr[LEN / 2];
}

/**
 * Returns the standard deviation of the array arr of length LEN.
 *
 * Parameters:
 * arr - array to find the standard deviation of
 * LEN - length of the array
 *
 */
float get_stddev(float arr[], const int LEN) {
	/* get mean */
	float mean = 0;
	for (size_t i = 0; i < LEN; i++) {
		mean += arr[i];
	}
	mean /= LEN;
	
	/* get stddev */
	float stddev = 0;
	for (size_t i = 0; i < LEN; i++) {
		stddev += (arr[i] - mean) * (arr[i] - mean) / LEN;
	}
	stddev = sqrt(stddev);
	return stddev;
}

/**
 * Returns the average difference between adjacent elements of array arr
 * of length LEN.
 *
 * Parameters:
 * arr - array to find average distance between adjacent elements of
 * LEN - length of the array.
 *
 */
float get_avg_difference(float arr[], const int LEN) {
	/* calculate average difference */
	float avg = 0;
	for (size_t i = 1; i < LEN; i++) {
		avg += arr[i] - arr[i - 1];
	}
	avg /= LEN - 1;
	return avg;
}

/**
 * Allows user to evaluate if data is acceptable. Returns true iff so.
 *
 * Parameters:
 * medians - median values in a data set
 * stddevs - standard deviations in a data set
 * LEN - length of each of previous arrays (should be equal)
 *
 */
bool evaluate_data(float medians[], float stddevs[], const int LEN) {
	float avg_difference = get_avg_difference(medians, LEN);
	/* for more accuracy, zero distance is set as offset from center */
	size_t offset = LEN / 2;
	float zero_distance = medians[offset] - offset * avg_difference;

	/* print values that will be saved */
	printf("Zero distance: %f\n", zero_distance);
	printf("Average difference: %f\n", avg_difference);

	/* get average standard deviation */
	float avg_stddev = 0;
	for (size_t i = 0; i < LEN; i++) {
		avg_stddev += stddevs[i];
	}
	avg_stddev /= LEN;
	printf("Average standard deviation: %f\n", avg_stddev);

	/* comment on standard deviation */
	printf("Note: This will ");
	printf(
		avg_difference > 3 * avg_stddev ? "almost always work.\n" :
		avg_difference > 2 * avg_stddev ? "usually work.\n" :
		avg_difference > avg_stddev ? "work more often than not.\n" :
		avg_difference > 0.5 * avg_stddev ? "work slightly more than fail.\n" :
		"usually fail.\n"
	);

	printf("Accept? y/n: ");
	int response = -1;
	do {
		response = getchar();
	} while (response == ' ' || response == '\n');
	/* consume whitespace until new line */
	while (getchar() != '\n');

	return response == 'n' || response == 'N';
}

/**
 * Gets data on a certain ring.
 *
 * Replaces:
 * zero_distance, avg_difference
 *
 * Parameters:
 * zero_distance - shortest distance from sensor to ring
 * avg_difference - average difference between two adjacent ring spaces
 * RING - current ring #
 * SUFFIX_CT - # of suffixes on the current ring
 *
 */
void measure_ring(float *zero_distance, float *avg_difference, const int RING, const int SUFFIX_CT) {
	/* # of data points collected for each suffix */
	const size_t POINTS_PER_SUFFIX = 30;
	/* this holds the data retrieved */
	float *measurements = calloc(POINTS_PER_SUFFIX, sizeof(float));

	/* these are for data for the user to evaluate */
	float median_measurements[SUFFIX_CT];
	float stddev_measurements[SUFFIX_CT];
	bool repeat_measurements = true;
	while (repeat_measurements) {
		printf("\nBeginning data collection on ring %i.\n", RING);
		for (size_t suffix = 0; suffix < SUFFIX_CT; suffix++) {
			/* prompt user to position correct ring */
			printf("Set ring %i to position %li. Hit enter when ready.", RING, suffix);
			/* pause until user hits enter */
			while (getchar() != '\n');

			/* take measurements */
			get_measurements(&measurements, POINTS_PER_SUFFIX, RING);
			median_measurements[suffix] = get_median(measurements, POINTS_PER_SUFFIX);
			stddev_measurements[suffix] = get_stddev(measurements, POINTS_PER_SUFFIX);
			printf("Done.\n");
		}
		/* repeat measurements if user determines them to be poor */
		printf("\nRing %i Analysis:\n", RING);
		repeat_measurements = evaluate_data(median_measurements, stddev_measurements, SUFFIX_CT);
	}
	
	free(measurements);

	*avg_difference = get_avg_difference(median_measurements, SUFFIX_CT);

	/* for more accuracy, zero distance is set as offset from center */
	size_t offset = SUFFIX_CT / 2;
	*zero_distance = median_measurements[offset] - offset * *avg_difference;
}

/**
 * Saves data to file, where each row contains a zero distance and an average difference,
 * space-separated. Can also save to console if file already exists and user does not wish
 * to overwrite.
 *
 * Parameters:
 * zero_distances - shortest distance from sensor to ring
 * avg_differences - average difference between two adjacent ring spaces
 * LEN - length of previous two arrays (should be equal)
 *
 */
void save_data(float zero_distances[], float avg_differences[], const size_t LEN) {
	FILE *file;
	const char FILENAME[] = "ring_data.txt";
	bool to_console = false;

	/* if file exists, prompt if user wants to overwrite */
	if ((file = fopen(FILENAME, "r")) != NULL) {
		fclose(file);
		printf("%s already exists. Overwrite? y/n: ", FILENAME);
		int response = -1;
		do {
			response = getchar();
		} while (response == ' ' || response == '\n');
		to_console = response == 'N' || response == 'n';
	}

	/* print out data */
	file = to_console ? stdout : fopen(FILENAME, "w");
	for (size_t ring = 0; ring < LEN; ring++) {
		fprintf(file, "%.3f %.3f\n", zero_distances[ring], avg_differences[ring]);
	}
	if (!to_console) {
		printf("Successfully written to %s.\n", FILENAME);
	}
}

/**
 * Gets information about status of rings, saves found data.
 *
 */
int main() {
	/* # of suffixes in each ring, from most to least central */
	const int SUFFIXES_PER_RING[] = {4, 4, 4, 7, 3, 5};
	/* # of rings */
	const size_t RING_CT = sizeof(SUFFIXES_PER_RING) / sizeof(SUFFIXES_PER_RING[0]);

	/* these two values are the "goal values", which will be saved in a file */
	float zero_distances[RING_CT];
	float avg_differences[RING_CT];

	printf("Welcome to the ring data collector. This program expects %li rings, with ", RING_CT);
	for (size_t ring = 0; ring < RING_CT - 1; ring++) {
		printf("%i, ", SUFFIXES_PER_RING[ring]);
	}
	printf("and %i suffixes on each ring going outward. ", SUFFIXES_PER_RING[RING_CT - 1]);
	printf("If this information is incorrect, please exit the program.\n");
	printf("The program will ask to adjust each ring for positions starting at 0. ");
	printf("0 represents the closest part of the ring to the sensor, and each number after is the next farthest.\n");
	/* collect data */
	for (size_t ring = 0; ring < RING_CT; ring++) {
		measure_ring(&zero_distances[ring], &avg_differences[ring], ring, SUFFIXES_PER_RING[ring]);
	}

	/* save data and exit */
	save_data(zero_distances, avg_differences, RING_CT);
	return 0;
}




