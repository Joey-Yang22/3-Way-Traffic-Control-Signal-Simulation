#include "fsl_device_registers.h"

static int i = 0;
int redCounter = 100000;
int yellowCounter = 4000000;
int greenCounter = 100000;
int rod = 1; //rate of decrease
int leftRod = 50;
int straight = 0x0000;
int leftTurn = 0x0001;
int leftToRight = 0x000;
int rightToLeft = 0x0001;

//int phototh1 = 0;
//int phototh2 = 0;
//int phototh3 = 0;
//int phototh4 = 0;




int rotated = 0;

int leftWait = 50000;




void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}


unsigned short ADC0_read16b(int addy) //0x00 is ADC0DP0, 0x01 is ADC0DP1
{
	ADC0_SC1A = addy; //Write to SC1A to start conversion from ADC_0
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK); // Conversion in progress
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // Until conversion completes
	software_delay(50);
	return ADC0_RA;
}

unsigned short ADC1_read16b(int addy) //0x00 is ADC0DP0, 0x01 is ADC0DP1
{
	ADC1_SC1A = addy; //Write to SC1A to start conversion from ADC_0
	while(ADC1_SC2 & ADC_SC2_ADACT_MASK); // Conversion in progress
	while(!(ADC1_SC1A & ADC_SC1_COCO_MASK)); // Until conversion completes
	software_delay(50);
	return ADC1_RA;
}


void greenToRed (int th1, int th2, int th3, int th4) {                       //int straight (1) = 0x0000; leftTurn (2) = 0x0001;  leftToRight (3) = 0x000; rightToLeft (4)= 0x0001;
	int temp = 0;
	int photo1 = 0;
	int photo2 = 0;
	int photo3 = 0;
	int photo4 = 0;

	int leftrod;
	int leftGreen = leftWait;
	int leftYellow = yellowCounter;
	int leftRed = redCounter;

	int leftTurnActive = 0;

	photo1 = ADC0_read16b(straight);
	photo2 = ADC0_read16b(leftTurn);
	photo3 = ADC1_read16b(leftToRight);
	photo4 = ADC1_read16b(rightToLeft);

	int tempRod = rod;



	temp = greenCounter; // add photoresistor values to change the rateofdecrease
	GPIOD_PDOR = 0x00000004;
	GPIOD_PDOR |= 0x00000010;
	GPIOC_PDOR = 0x00000004;

	GPIOB_PDOR |= 0x00000004; //signal for arduino to indicate which lane



	if((photo2 < th2 && !leftTurnActive) && rotated == 0)
	{
		//left turn light on
		GPIOC_PDOR &= ~0x00000008; // Turn off PortC3 (red for left turn signal)

		GPIOC_PDOR |= 0x00000020; // PortC5 (left turn green)

		GPIOC_PDOR |= 0x00000001; // Turn on PortC0 (red for opposite lane)
		GPIOC_PDOR &= ~0x00000004; // Turn off PortC2 (green for opposite lane)
		rotated = 0;
		leftTurnActive = 1; // Set flag
		GPIOB_PDOR |= 0x00000008; //signal for arduino to indicate left turn
	}

	if(!(leftTurnActive))
	{
		// Left turn red light (PortC0) is always on during green phase
		GPIOC_PDOR |= 0x00000008;
	}


	while (temp > 0) {
		photo1 = ADC0_read16b(straight);
		photo2 = ADC0_read16b(leftTurn);
		photo3 = ADC1_read16b(leftToRight);
		photo4 = ADC1_read16b(rightToLeft);

		//checking if left turn should be turned off
		if((photo2 >= th2 && leftTurnActive) || leftGreen < 0)
		{
			leftYellow = yellowCounter;
			GPIOC_PDOR &= ~0x00000020; // Turn off PortC5 (left turn green)
			GPIOC_PDOR |= 0x00000010; // PortC4 (left turn yellow)
			while(leftYellow > 0){
				leftYellow = leftYellow - rod;
			}
			GPIOC_PDOR &= ~0x00000010; // Turn off PortC4 (left turn yellow)
			GPIOC_PDOR |= 0x00000008; // Turn on PortC3 (red for left turn signal)
			GPIOB_PDOR &= ~(1 << 3); //signal for arduino to indicate left turn
			software_delay(3000000);
			GPIOC_PDOR &= ~0x00000001; // Turn off PortC0 (red for opposite lane)
			GPIOC_PDOR |= 0x00000004; // Turn on PortC2 (green for opposite lane)
			leftGreen = leftWait;
			leftTurnActive = 0; // Reset flag
			//GPIOB_PDOR &= ~(1 << 3); //signal for arduino to indicate left turn
		}

		//checks if there are no cars in current lane, and another car in the other lane
		if (photo1 < th1 && photo3 >= th3 && photo4 >= th4 && photo2 >= th2) { // add left turn
			tempRod = 30;
		}
		else {
			tempRod = rod;
		}

		//counting left turn first
		if(leftTurnActive)
		{
			leftGreen = leftGreen - rod;
		}

		//start counting down regular cycle once left turn is done
		if(!(leftTurnActive))
		{
			temp = temp - tempRod;
		}
	}


	//yellow
	temp = yellowCounter;
	tempRod = rod;


	GPIOD_PDOR = 0x00000002;
	GPIOD_PDOR |= 0x00000010;
	GPIOC_PDOR = 0x00000002;

	if(leftTurnActive){
		GPIOC_PDOR &= ~0x00000020; // Turn off PortC5 (left turn green)
		GPIOC_PDOR |= 0x00000010; // PortC4 (left turn yellow)
		GPIOC_PDOR &= ~0x00000001;  //turn off Port C0 (straight red)
		GPIOC_PDOR |= 0x00000001; // PortC0
	}
	else
	{
		GPIOC_PDOR |= 0x00000008; // PortC5 (left turn red)
	}

	while (temp > 0) {

		temp = temp - tempRod;
	}


	//red
	temp = redCounter;
	tempRod = rod;

	GPIOB_PDOR &= ~(1 << 2);   //signal for arduino to indicate which lane

	GPIOD_PDOR = 0x000000011;
	GPIOC_PDOR = 0x00000001;
	GPIOC_PDOR |= 0x00000008; // PortC5 (left turn red)

	software_delay(3000000);
	GPIOD_PDOR = 0x000000001;
	GPIOD_PDOR |= 0x00000040;
	GPIOC_PDOR |= 0x00000008; // PortC5 (left turn red)




	while (temp > 0) {
		photo1 = ADC0_read16b(straight);
		photo2 = ADC0_read16b(leftTurn);
		photo3 = ADC1_read16b(leftToRight);
		photo4 = ADC1_read16b(rightToLeft);

		if (((photo3 <= th3) || (photo4 <= th4)) && (photo1 >= th1)) {
			tempRod = 30;
		}
		else {
			tempRod = rod;
		}
		temp = temp - tempRod;
	}



	//second rotation

	leftTurnActive = 0; // Flag to track if left turn is active

	temp = 0;
	photo1 = 0;
	photo2 = 0;
	photo3 = 0;
	photo4 = 0;

	//green
	tempRod = rod;
	temp = greenCounter; // add photoresistor values to change the rateofdecrease
	GPIOC_PDOR |= 0x00000008; //left turn red light
	GPIOD_PDOR = 0x00000040;
	GPIOD_PDOR |= 0x00000001;
	GPIOC_PDOR = 0x00000001;

	GPIOC_PDOR |= 0x00000008; // PortC5 (left turn red)


	while (temp > 0) {
		photo1 = ADC0_read16b(straight);
		photo2 = ADC0_read16b(leftTurn);
		photo3 = ADC1_read16b(leftToRight);
		photo4 = ADC1_read16b(rightToLeft);

		if (((photo3 <= th3) || (photo4 <= th4)) && (photo1 >= th1)) {
			tempRod = 30;
		}
		else {
			tempRod = rod;
		}
		temp = temp - tempRod;
	}

	//yellow
	temp = yellowCounter;
	tempRod = rod;
	leftTurnActive = 0; // Flag to track if left turn is active


	GPIOD_PDOR = 0x00000020;
	GPIOD_PDOR |= 0x00000001;

	while (temp > 0) {
		temp = temp - tempRod;
	}

	//red
	temp = redCounter;
	tempRod = rod;

	GPIOD_PDOR = 0x00000011;


	GPIOB_PDOR |= 0x00000004; //signal for arduino to indicate which lane
	software_delay(3000000);

	GPIOC_PDOR = 0x00000004;
	GPIOD_PDOR = 0x000000010;
	GPIOD_PDOR |= 0x00000004;




	photo1 = ADC0_read16b(straight);
	photo2 = ADC0_read16b(leftTurn);
	photo3 = ADC1_read16b(leftToRight);
	photo4 = ADC1_read16b(rightToLeft);


	if(photo2 < th2 && !leftTurnActive)
	{
		//left turn light on
		GPIOC_PDOR &= ~0x00000008; // Turn off PortC3 (red for left turn signal)

		GPIOC_PDOR |= 0x00000020; // PortC5 (left turn green)

		GPIOC_PDOR |= 0x00000001; // Turn on PortC0 (red for opposite lane)
		GPIOC_PDOR &= ~0x00000004; // Turn off PortC2 (green for opposite lane)
		leftTurnActive = 1; // Set flag
		GPIOB_PDOR |= 0x00000008; //signal for arduino to indicate left turn
	}

	if(!(leftTurnActive))
	{
		// Left turn red light (PortC0) is always on during green phase
		GPIOC_PDOR |= 0x00000008;
	}

	while (temp > 0)
	{
		photo1 = ADC0_read16b(straight);
		photo2 = ADC0_read16b(leftTurn);
		photo3 = ADC1_read16b(leftToRight);
		photo4 = ADC1_read16b(rightToLeft);


		//checking if left turn should be turned off
		if((photo2 >= th2 && leftTurnActive) || leftGreen < 0)
		{
			leftYellow = yellowCounter;
			GPIOC_PDOR &= ~0x00000020; // Turn off PortC5 (left turn green)
			GPIOC_PDOR |= 0x00000010; // PortC4 (left turn yellow)
			while(leftYellow > 0){
				leftYellow = leftYellow - rod;
			}
			GPIOC_PDOR &= ~0x00000010; // Turn off PortC4 (left turn yellow)
			GPIOC_PDOR |= 0x00000008; // Turn on PortC3 (red for left turn signal)
			GPIOB_PDOR &= ~(1 << 3); //signal for arduino to indicate left turn


			software_delay(3000000);

			GPIOC_PDOR &= ~0x00000001; // Turn off PortC0 (red for opposite lane)
			GPIOC_PDOR |= 0x00000004; // Turn on PortC2 (green for opposite lane)
			leftGreen = leftWait;
			leftTurnActive = 0; // Reset flag
			//GPIOB_PDOR &= ~(1 << 3); //signal for arduino to indicate left turn
		}


		if (photo1 < th1 && photo3 >= th3 && photo4 >= th4 && photo2 >= th2) //add left turn
		{
			tempRod = 30;
		}
		else
		{
			tempRod = rod;
		}

		//counting left turn first
		if(leftTurnActive)
		{
			leftGreen = leftGreen - rod;
		}
		//start counting down regular cycle once left turn is done
		if(!(leftTurnActive))
		{
			temp = temp - tempRod;
		}
	}
	rotated = 1;
}

	int main(void)
	{
		SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
		ADC0_CFG1 = 0x0C;
		ADC0_SC1A = 0x1F; //disables module

		SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
		ADC1_CFG1 = 0x0C;
		ADC1_SC1A = 0x1F; //disables module


	    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; // Port D clock enable
	    PORTD_GPCLR = 0x00FF0100; //Configuring GPIO for Port B
	    GPIOD_PDDR |= 0x000000FF;
	    GPIOD_PDOR = 0x0000FFFF; //Sets Port D as outputs

		SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
		PORTC_GPCLR = 0x01BF0100;
		GPIOC_PDDR |= 0x000001BF;
		GPIOC_PDOR = 0x00000000;

		SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
		PORTB_GPCLR = 0x00FF0100;
		GPIOB_PDDR |= 0x000000FF;
		GPIOB_PDOR = 0x00000000;





		int input = 0;
		//int th = 0x1000;
		int phototh1 = ADC0_read16b(0x00) - 0x100;
		int phototh2 = ADC0_read16b(0x01) - 0x400;
		int phototh3 = ADC1_read16b(0x00) - 0x80;
		int phototh4 = ADC1_read16b(0x01) - 0x80;
		while (1) {

			//greenToRed(phototh1, phototh2, phototh3, phototh4);

			  int pulse_ticks = 1315 + ((angle * (2625 - 1315)) / 180);
			  FTM3_C6V = pulse_ticks;
			  //FTM3_C6SC = 0x28; // Make sure PWM active high is set again



		}
		/* Never leave main */
		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////
	// EOF
	////////////////////////////////////////////////////////////////////////////////
	//code so far

