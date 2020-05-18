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

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
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

bool objIntaked = false;
bool atDropOff = false;

void opcontrol() {
	vision_signature_s_t PUR = visionSensor.signature_from_utility(1, 901, 1821, 1362, 10293, 12743, 11518, 3.000, 0);
	vision_signature_s_t GRE = visionSensor.signature_from_utility(2, -7299, -6057, -6678, -4197, -2977, -3588, 3.000, 0);
	vision_signature_s_t ORG = visionSensor.signature_from_utility(3, 6433, 9433, 7934, -2875, -2409, -2642, 3.900, 0);
	vision_signature_s_t BLU = visionSensor.signature_from_utility(4, -3205, -2095, -2650, 6807, 10725, 8766, 2.300, 0);

	visionSensor.set_signature(PURPLE_CUBE_ID, &PUR);
	visionSensor.set_signature(GREEN_CUBE_ID, &GRE);
	visionSensor.set_signature(ORANGE_CUBE_ID, &ORG);
	visionSensor.set_signature(BLUE_SPHERE_ID, &BLU);

	bool toggleDriver = true;

	while(true){
		if (!objIntaked && !atDropOff){
			if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
				if (toggleDriver == false){
					toggleDriver = true;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Driver Mode");
					lcd::set_text(0, "Driver Mode");
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
					vexController.set_text(0, 0, "Driver Mode");
					lcd::set_text(0, "Driver Mode");
					for (size_t i{1}; i<8; i++){
						lcd::clear_line(i);
					}
				}else if (toggleDriver == true){
					toggleDriver = false;
					vexController.clear();
					delay(50);
					vexController.set_text(0, 0, "Drop-off");
					lcd::set_text(0, "Drop-off");
					}
				}

			if (toggleDriver){
				Intake(false, 127, 64);
				Chassis();
			}else{
					VisionSensorCenter(2);
			}
		delay(20);
		}
	}

	// leftIntake.move(0);
	// rightIntake.move(0);
	// rightDrive.move(0);
	// leftDrive.move(0);
	// for (size_t i{0}; i<8; i++){
	// 	lcd::clear_line(i);
	// delay(20);


	// 3. Intake the ball out slowly
	// 4. Back away at a medium speed from drop off zone
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

	lcd::set_text(3, "Obj Width: "+ std::to_string(objWidth));

	if (topBound < objWidth){
		lcd::set_text(2, "Object Intaked");
		objIntaked = true;
	}else if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(2, "Intaking");
		Intake(true);
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(2, "Moving Forward At: "+ std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorCubeDropOff(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	int topBound = 102;
	int bottomBound = 68;

	lcd::set_text(3, "Obj Width: "+ std::to_string(objWidth));

	if (bottomBound <= objWidth && objWidth <= topBound){
		lcd::set_text(2, "At Drop Off");
		atDropOff = true;
	}else if (5 < objWidth && objWidth < bottomBound){
		lcd::set_text(2, "Moving Forward At: " + std::to_string(forwardSpeed));
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
		float turnSpeed = 10 * powf(1.012, abs(xMiddle));

		if (turnErrorRange > xMiddle && xMiddle > -turnErrorRange){
			lcd::set_text(1, "Object Centered");
			int objWidth = rtn.width;
			if(!objIntaked){
				VisionSensorBlueBall(objWidth);
			}else{
				VisionSensorCubeDropOff(objWidth);
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
	}
}
