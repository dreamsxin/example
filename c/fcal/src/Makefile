all:
	gcc -O2 -c fdatefunc.c
	gcc test_sun_to_lunar.c fdatefunc.o -O2 -o sun_to_lunar
	gcc test_lunar_to_sun.c fdatefunc.o -O2 -o lunar_to_sun
	gcc lunar.c fdatefunc.o -O2 -o lunar
	strip sun_to_lunar lunar_to_sun lunar

clean:
	rm *.o sun_to_lunar lunar_to_sun lunar
