#pragma config(Sensor, in1,    TurnGyro,       sensorGyro)
#pragma config(Sensor, dgtl7,  LeftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  RightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl11, ShooterEncoder, sensorQuadEncoder)
#pragma config(Motor,  port1,           Intake,        tmotorVex393TurboSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           LeftShooter1,  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           LeftShooter2,  tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           LeftShooter3,  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           LeftDrive,     tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           RightDrive,    tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           RightShooter1, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           RightShooter2, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           RightShooter3, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          Index,         tmotorVex393HighSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "TBH_Library.c"

//++++++++++++++++++++PID Stuff++++++++++++++++++++

//pid values
const float pVal = 1;
//float iVal = 0;
//float dVal = 0;

//PID variables
int error = 0;
//int sumError = 0;
//int slope = 0;
//int lastError = 0;

int pChange = 0;
//int iChange = 0;
//int dChange = 0;
int tChange = 0;

//RPM Calculation Variables
int lastVal = 0;
int val = 0;
int diff = 0;
int targetDis;
float currentDis;

float cmPreTic = 0.07;

task pid(){											//PID task
	while(true){		//pid overwrite

			//++++++++++++++++++++Current dis++++++++++++++++++++
			val = SensorValue[LeftEncoder];

			currentDis = val * cmPreTic;
			writeDebugStreamLine("current dis: %d", currentDis);
			//-----------------End Current dis ------------------

			//get error
			error = targetDis - currentDis;
			writeDebugStreamLine("error : %d", error);

			//sumError = sumError + error;

			//p calculations
			pChange = error * pVal;

			writeDebugStreamLine("p change : %d", pChange);

			//i calculations
			//iChange = sumError * iVal;

			//d calculations
			//slope = error - lastError;
			//dChange = slope * dVal;

			//total pid changes
			tChange = pChange; //+ iChange + dChange;

			writeDebugStreamLine("t change : %d \n\r", tChange);

			//update motor

			motor[LeftDrive] = tChange;

			//lastError = error;
			lastVal = val;
			wait1Msec(50);
	}//while loop
}//task

task main()
{

	// Start the flywheel control task
	startTask( FwControlTask );
	startTask( pid );

	bool AutoFeed = false;
	int AutoIndex = 0;
	int IntakeCycles = 0;

	// Main user control loop
	while(1)
	{
		// Different speeds set by buttons
		if( vexRT[ Btn8R ] == 1 )
			FwVelocitySet( 170, 0.47 );
		if( vexRT[ Btn8U ] == 1 )
			FwVelocitySet( 145, 0.47 );
		if( vexRT[ Btn8L ] == 1 )
			FwVelocitySet( 125, 0.39 );
		if( vexRT[ Btn8D ] == 1 )
			FwVelocitySet( 00, 0 );

		//////// DRIVE ////////
		//if(abs(vexRT[Ch3]) > 10 || abs(vexRT[Ch4]) > 10 ||
		//	abs(vexRT[Ch1]) > 10 || abs(vexRT[Ch2]) > 10){
		//	motor[LeftDrive]  = ((vexRT[Ch2] + vexRT[Ch1])/2 - (vexRT[Ch3] - vexRT[Ch4]));
		//	motor[RightDrive]   = ((vexRT[Ch2] - vexRT[Ch1])/2 - (vexRT[Ch3] + vexRT[Ch4]));
		//}
		//else{
		//	motor[LeftDrive]  = 0;
		//	motor[RightDrive]  = 0;
		//}

		/////// INTAKE ////////

			targetDis = vexRT[ch2];

		if(vexRT[Btn6U] == 1){
			motor[Intake]   = 127;
		}
		else if(vexRT[Btn6D] == 1){
			if (IntakeCycles < 500){
				motor[Intake] = 0;
			}
			IntakeCycles++;
			if (IntakeCycles > 500){
				motor[Intake]   = -127;
			}
		}
		else{
			if(IntakeCycles > 250){
				motor[Intake] = 0;
			}
			IntakeCycles = 0;
		}

		/////// INDEXER ////////
		if(vexRT[Btn5U] == 1){
			motor[Index]   = 127;
		}
		else if(vexRT[Btn5D] == 1){
			motor[Index] = -127;
			AutoFeed = false;
		}
		else {
			motor[Index] = AutoIndex;
		}

		///////////// AUTO FEED CODE /////////////////
		if (vexRT[Btn7U] == 1) AutoFeed = true;
		else if (vexRT[Btn7D] == 1) AutoFeed = false;

		if (AutoFeed == true){
			AutoIndex = 127;
		}//AutoFeed
		else {
			AutoIndex = 0;
		}
		// Don't hog the cpu :)
		wait1Msec(10);
	}
}
