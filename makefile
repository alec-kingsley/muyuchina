CFLAGS = -Wall

# data collection program ; necessary before running driver
data_collector: data_collector.c controller.c controller.h
	gcc $(CFLAGS) -lwiringPi -o $@ data_collector.c controller.c

# main program
driver: driver.c que_to_eng.c controller.c que_to_eng.h controller.h
	gcc $(CFLAGS) -lwiringPi -o $@ driver.c que_to_eng.c controller.c

# testing program ; shows random word / translation
tester: tester.c que_to_eng.c que_to_eng.h
	gcc $(CFLAGS) -o $@ tester.c que_to_eng.c

clean: rm -f *.o driver tester data_collector


