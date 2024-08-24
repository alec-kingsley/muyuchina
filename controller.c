#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
 
/**
 *  WIRING PI PIN LAYOUT
 *  ------------------
 *  | 	3.3v	5v     |
 *  |   WP 8	5v     |
 *  | 	WP 9	GND    |
 *  | 	WP 7	WP 15  |
 *  |  	 GND	WP 16  |
 *  | 	WP 0	WP 1   |
 *  | 	WP 2	GND    |
 *  | 	WP 3	WP 4   |
 *  |		3.3v	WP 5   |
 *  |  WP 12	GND    |
 *  |  WP 13	WP 6   |
 *  |  WP 14	WP 10  |
 *  |  	 GND	WP 11  |
 *  |  WP 30	WP 31  |
 *  |  WP 21	GND    |
 *  |  WP 22	WP 26  |
 *  |  WP 23	GND    |
 *  |  WP 24  WP 27  |
 *  |  WP 25	WP 28  |
 *  |    GND	WP 29  |
 *  ------------------
 *
 */
const int TRIGGER_PINS[] = {4};
const int ECHO_PINS[] = {5};
const int RING_CT = sizeof(ECHO_PINS) / sizeof(ECHO_PINS[0]);

/**
 * Sets up pins for usage.
 *
 * All trigger pins should be for output, while echo pins should
 * be for input.
 *
 */
void setup() {
	/* set up wiring pi */
	if (wiringPiSetup() < 0) {
		printf("Failed to setup wiringpi\n");
		return 1;
	}
	/* set pins */
	for (size_t ring = 0; ring < RING_CT; ring++) {
		pinMode(TRIGGER_PINS[ring], OUTPUT);
		pinMode(ECHO_PINS[ring], INPUT);
	}
}

/**
 * Measures the distance at a certain ring in cm.
 * Returns -1 if out of range.
 *
 * Parameter:
 * RING - the ring number, where 0 <= RING < RING_CT
 *
 */
float measure_ring_cm(const size_t RING) {
	/* known constants */
	const int SOUND_SPEED_CM_PER_SEC = 34320;
	const int MAX_ULTRASONIC_RANGE_CM = 400;
 
	/* generate 10us pulse */
	digitalWrite(TRIGGER_PINS[RING], HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER_PINS[RING], LOW);
 
	/* get elapsed time in seconds */
	float start = 0.0, end = 0.0, elapsed_sec;
	while (!digitalRead(ECHO_PINS[RING])) {
		start = (float) clock();
	}
	while (digitalRead(ECHO_PINS[RING])) {
		end = (float) clock();
		/* if out of range, return -1 */
		if ((end - start) / CLOCKS_PER_SEC * SOUND_SPEED_CM_PER_SEC / 2 > MAX_ULTRASONIC_RANGE_CM) {
			return -1;
		}
	}
	elapsed_sec = (end - start) / CLOCKS_PER_SEC;
 
	/* has to travel both ways, so divide by 2 */
	return elapsed_sec * SOUND_SPEED_CM_PER_SEC / 2;
}


