#include "main.h"

#define LEFT_DRIVE_MOTOR 20
#define RIGHT_DRIVE_MOTOR 11
#define VISION_PORT 6
#define RIGHT_INTAKE_PORT 12
#define LEFT_INTAKE_PORT 9

using namespace pros;

Controller vexController(E_CONTROLLER_MASTER);
Motor leftDrive(LEFT_DRIVE_MOTOR);
Motor rightDrive(RIGHT_DRIVE_MOTOR, true);
Motor leftIntake(LEFT_INTAKE_PORT);
Motor rightIntake(RIGHT_INTAKE_PORT, true);
Vision visionSensor(VISION_PORT);

void Chassis();
void Intake(bool intakeForObject = false, int intakeSpeed = 127, int extakeSpeed = 127);
void VisionSensorBlueBall(int objWidth);
void VisionSensorCubeDropOff(int objWidth);
void VisionSensorCenter(int sig_id, int size_id = 0);
void ResetAll();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	leftDrive.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
	rightDrive.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
	rightIntake.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
	leftIntake.set_encoder_units(E_MOTOR_ENCODER_DEGREES);

	lcd::initialize();
	lcd::set_text(0, "Driver Mode");
	vexController.clear();
	delay(50);
	vexController.set_text(0, 0, "Driver Mode");
	visionSensor.clear_led();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

#define PURPLE_CUBE_ID 1
#define GREEN_CUBE_ID 2
#define ORANGE_CUBE_ID 3
#define BLUE_SPHERE_ID 4

bool toggleDriver = true;
bool objIntaked = false;
bool atDropOff = false;
bool atPurple = false;
bool atOrange = false;

void opcontrol() {
	vision_signature_s_t PUR = visionSensor.signature_from_utility(1, 1169, 1891, 1530, 10347, 11797, 11072, 3.000, 0);
	vision_signature_s_t GRE = visionSensor.signature_from_utility(2, -8357, -6569, -7464, -5797, -3309, -4554, 3.000, 0);
	vision_signature_s_t ORG = visionSensor.signature_from_utility(3, 10747, 11843, 11295, -2253, -1833, -2043, 4.000, 0);
	vision_signature_s_t BLU = visionSensor.signature_from_utility(4, -2963, -2159, -2560, 10119, 13043, 11580, 3.000, 0);

	visionSensor.set_signature(PURPLE_CUBE_ID, &PUR);
	visionSensor.set_signature(GREEN_CUBE_ID, &GRE);
	visionSensor.set_signature(ORANGE_CUBE_ID, &ORG);
	visionSensor.set_signature(BLUE_SPHERE_ID, &BLU);

	while(true){
		if (!objIntaked && !atDropOff){
			if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
				if (toggleDriver == false){
					toggleDriver = true;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Driver - Blue");
					lcd::set_text(0, "Driver Mode - Blue Ball");
					for (size_t i{1}; i<8; i++){
						lcd::clear_line(i);
					}
				}else if (toggleDriver == true){
					toggleDriver = false;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Auto Blue");
					lcd::set_text(0, "Auto Blue");
					}
				}

			if (toggleDriver){
				Intake(false, 127, 64);
				Chassis();
			}else{
				VisionSensorCenter(4);
			}
		}else if(objIntaked && !atDropOff){
			if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
				if (toggleDriver == false){
					toggleDriver = true;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Driver - Drop Off");
					lcd::set_text(0, "Driver Mode - Drop Off");
					for (size_t i{1}; i<8; i++){
						lcd::clear_line(i);
					}
				}else if (toggleDriver == true){
					toggleDriver = false;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Auto Drop Off");
					lcd::set_text(0, "Auto Drop Off");
					}
				}

			if (toggleDriver){
				Intake(false, 127, 64);
				Chassis();
			}else{
					VisionSensorCenter(2);
			}
		}else if(atDropOff && !atPurple){
			if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
				if (toggleDriver == false){
					toggleDriver = true;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Driver - Purple");
					lcd::set_text(0, "Driver Mode - Purple Cube");
					for (size_t i{1}; i<8; i++){
						lcd::clear_line(i);
					}
				}else if (toggleDriver == true){
					toggleDriver = false;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Auto Purple Cube");
					lcd::set_text(0, "Auto Purple Cube");
					}
				}

			if (toggleDriver){
				Intake(false, 127, 64);
				Chassis();
			}else{
				VisionSensorCenter(1);
			}
		}else if(atPurple && !atOrange){
			if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
				if (toggleDriver == false){
					toggleDriver = true;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Driver - Orange");
					lcd::set_text(0, "Driver Mode - Orange Cube");
					for (size_t i{1}; i<8; i++){
						lcd::clear_line(i);
					}
				}else if (toggleDriver == true){
					toggleDriver = false;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Auto Orange Cube");
					lcd::set_text(0, "Auto Orange Cube");
					}
				}

			if (toggleDriver){
				Intake(false, 127, 64);
				Chassis();
			}else{
				VisionSensorCenter(3);
			}
		}else{
			leftDrive.move(0);
			rightDrive.move(0);
			break;
		}

		delay(20);
	}
}

void Chassis(){
	float forwardScale = 0.75;
	float turnScale = 0.5;
	float forward = forwardScale * vexController.get_analog(ANALOG_LEFT_Y);
	float turn = turnScale * vexController.get_analog(ANALOG_RIGHT_X);
	int left = forward + turn;
	int right = forward - turn;
	leftDrive.move(left);
	rightDrive.move(right);

}

void Intake(bool intakeForObject, int intakeSpeed, int extakeSpeed){
	if(intakeForObject || vexController.get_digital(E_CONTROLLER_DIGITAL_R1)){
		leftIntake.move(intakeSpeed);
		rightIntake.move(intakeSpeed);
	}else if(vexController.get_digital(E_CONTROLLER_DIGITAL_L1)){
		leftIntake.move(-extakeSpeed);
		rightIntake.move(-extakeSpeed);
	}else{
		leftIntake.move(0);
		rightIntake.move(0);
	}
}

void VisionSensorBlueBall(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	int topBound = 69;
	int bottomBound = 63;

	lcd::set_text(4, "Obj Width: "+ std::to_string(objWidth));

	if (topBound < objWidth){
		lcd::set_text(3, "Object Intaked");
		ResetAll();
		objIntaked = true;
	}else if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(1, "Intaking");
		Intake(true);
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(1, "Moving Forward At: "+ std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorCubeDropOff(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	int topBound = 57;
	int bottomBound = 47;

	lcd::set_text(4, "Obj Width: "+ std::to_string(objWidth));

	if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(3, "At Drop Off");
		ResetAll();

		int intakeMove = -360;
		rightIntake.tare_position();
		leftIntake.tare_position();
		rightIntake.move_relative(intakeMove, 80);
		leftIntake.move_relative(intakeMove, 80);
		while (!((rightIntake.get_position() < intakeMove + 5) && (rightIntake.get_position() > intakeMove - 5))){
    	pros::delay(2);
  	}
		int motorMove = -360;
		leftDrive.tare_position();
		rightDrive.tare_position();
		leftDrive.move_relative(motorMove, 50);
		rightDrive.move_relative(motorMove, 50);
		while (!((leftDrive.get_position() < motorMove + 5) && (leftDrive.get_position() > motorMove - 5))){
    	pros::delay(2);
  	}
		atDropOff = true;
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(1, "Moving Forward At: " + std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorPurpleCube(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	int topBound = 70;
	int bottomBound = 65;

	lcd::set_text(4, "Obj Width: "+ std::to_string(objWidth));

	if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(3, "At Purple Cube");
		ResetAll();
		atPurple = true;
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(1, "Moving Forward At: " + std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorOrangeCube(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	int topBound = 66;
	int bottomBound = 61;

	lcd::set_text(4, "Obj Width: "+ std::to_string(objWidth));

	if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(3, "At Orange Cube");
		ResetAll();
		atPurple = true;
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(1, "Moving Forward At: " + std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorCenter(int sig_id, int size_id){
	visionSensor.set_zero_point(E_VISION_ZERO_CENTER);
	vision_object_s_t rtn = visionSensor.get_by_sig(0, sig_id);

	if (rtn.signature == sig_id){
		int turnErrorRange = 10;
		int xMiddle = rtn.x_middle_coord;
		float turnSpeed = 13.5 * powf(1.011, abs(xMiddle));

		if (turnErrorRange > xMiddle && xMiddle > -turnErrorRange){
			lcd::set_text(2, "Object Centered");
			int objWidth = rtn.width;
			if(!objIntaked && !atDropOff){
				VisionSensorBlueBall(objWidth);
			}else if(objIntaked && !atDropOff){
				VisionSensorCubeDropOff(objWidth);
			}else if(atDropOff && !atPurple){
				VisionSensorPurpleCube(objWidth);

			}else if(atPurple && !atOrange){
				VisionSensorOrangeCube(objWidth);
			}
		}else if (xMiddle > 0){
			lcd::set_text(1, "Turn speed: " + std::to_string(turnSpeed));
			leftDrive.move(turnSpeed);
			rightDrive.move(-turnSpeed);
		}else if (xMiddle < 0){
			lcd::set_text(1, "Turn speed: " + std::to_string(turnSpeed));
			leftDrive.move(-turnSpeed);
			rightDrive.move(turnSpeed);
		}
	}else if(rtn.signature == 255 && (atDropOff || atPurple)){
		int turnSpeed = 30;
		rightDrive.move(turnSpeed);
		leftDrive.move(-turnSpeed);
	}
}

void ResetAll(){
	toggleDriver = true;
	for (size_t i{0}; i<8; i++)
		lcd::clear_line(i);
	delay(20);
	lcd::set_text(0, "Driver Mode");
	vexController.set_text(0, 0, "Driver Mode");
}
