#define CALIBRATE_K_TIMES_100	((expected2Dec2 - expected2Dec1) * 100 / (actual2Dec2 - actual2Dec1))
#define CALIBRATE_B_TIMES_100 (expected2Dec1 - actual2Dec1 * CALIBRATE_K_TIMES_100 / 100)

int expected2Dec1 = 2400;
int actual2Dec1 = 2200; // without calibrate()
int expected2Dec2 = 3150;
int actual2Dec2 = 3400; // without calibrate()
