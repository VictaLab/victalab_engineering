/*
 * Title: main_program
 * Author: Victa Li
 * Purpose: this is a test main program for testing communications with the P89LPC9351 board
 * 			the codes come from the course website, checked by Victa
*/

#include <p891pc9351.h>
// This means we have to have this header file in place


// To me, this seems like a counter to perform/test how fast the controller can tick its clock and makes a pause
// So here it pauses for 1000^100 unit time span. 
 void delay(void) {
 	int j,k;
 	for(j=0; j<100; j++) {
 		for(k=0; k<1000; k++);
 	}
 }

 void main(void) {
 	P2M1 = 0; // maybe defined in the header file
 	p2M2 = 0;

 	while(true) {
 		P2_1 = 0; // maybe defined in the header file
 		delay();
 		P2_1 = 1;
 		delay();
 	}
 }
