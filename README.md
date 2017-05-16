This project is a multi-year Burning Man countdown.  When properly set, it will count
down the number of days until the man burns, and on the final day will switch to a
dancing animation.  The next day, the count resets and it begins counting down for
the next year.

Current draw is approximately 2 uA to 5 uA depending on the number of segments active.
Battery life on one CR2032 cell is about 5 years.

| Part | Description                | Price (qty 1) | Price (qty 10) |
|------|----------------------------|---------------|----------------|
| U1	 | Freescale MC9S08LL16CLH	| $2.95         |	$2.77          |
| LCD	 | Varionics VI-321-DP-RC-S	| $4.40	    | $3.28          |
| XTAL | CM51932768DZFT			| $1.20	    | $0.46          |
| BATT | Panasonic CR-2032L/F1N	| $1.10	    | $1.00          |
| C1-C6 | 0.1uF 0603 MLCC           |               |                |

Programming is via a TC2030 programming header with a non-standard pinout.  Sorry -
I only designed this for my own use and went with what I had available.

To set the starting time, you need to make sure that the interval[] array starts
with the current year's interval.  As provided, the code starts with 2013-2014.
You'll also need to set START_DAYS and START_MINS.

The code is fairly simple, but no great effort has been made to clean it up.
tod_ISD() runs once every minute and the current time is recorded in two places
in RAM.  If the MCU is rebooted (due to an SEU caused by a cosmic ray, for example)
counting will resume with the last recorded time if the backup copies are valid.

When the number of days remaining changes, the main loop updates the LCD pins
and shuts down until the next TOD interrupt.
