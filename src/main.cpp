#include "main.h"

// Ports
#define LEFT_DRIVE_MOTOR 11
#define RIGHT_DRIVE_MOTOR 20
#define VISION_PORT 6
#define RIGHT_INTAKE_PORT 3
#define LEFT_INTAKE_PORT 10

// Namespace
using namespace pros;

// Create controller, motors and sensors
Controller vexController(E_CONTROLLER_MASTER);
Motor leftDrive(LEFT_DRIVE_MOTOR);
Motor rightDrive(RIGHT_DRIVE_MOTOR, true);
Motor leftIntake(LEFT_INTAKE_PORT);
Motor rightIntake(RIGHT_INTAKE_PORT);
Vision visionSensor(VISION_PORT);


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

// Vision Sensor ID's
#define PURPLE_CUBE_ID 1
#define GREEN_CUBE_ID 2
#define ORANGE_CUBE_ID 3
#define BLUE_SPHERE_ID 4

void Chassis(){
	int forward = vexController.get_analog(ANALOG_LEFT_Y);
	int turn = vexController.get_analog(ANALOG_RIGHT_X);
	int left = forward + turn;
	int right = forward - turn;
	leftDrive.move(left);
	rightDrive.move(right);
}

void Intake(int intakeSpeed = 127){
	if(vexController.get_digital(E_CONTROLLER_DIGITAL_R1)){
		leftIntake.move(-intakeSpeed);
		rightIntake.move(intakeSpeed);
	}else if(vexController.get_digital(E_CONTROLLER_DIGITAL_L1)){
		leftIntake.move(intakeSpeed);
		rightIntake.move(-intakeSpeed);
	}else{
		leftIntake.move(0);
		rightIntake.move(0);
	}
}

void VisionSensorMove(int objWidth){
	float forwardSpeed = 10 * powf(1.005, 316 - abs(objWidth));
	// float backSpeed = -(10 * powf(1.015, 316 - abs(objWidth))+ 10);
	int moveError = 13;

	lcd::set_text(3, "Obj Width: "+ std::to_string(objWidth));

	if (objWidth >= 200 - moveError && objWidth <= 200 + moveError) { //243>=objWidth>=227
		lcd::set_text(2, "Located At Object");
	// }else if(objWidth < 320 && objWidth > 280 + moveError){ // 320>objWidth>293
	// 	lcd::set_text(2, "Reversing at: " + std::to_string(backSpeed));
	// 	leftDrive.move(backSpeed);
	// 	rightDrive.move(backSpeed);
	}else if (objWidth > 0 && objWidth < 200 - moveError){ // 277 > objWidth >> 0
		lcd::set_text(2, "Moving forward at: "+ std::to_string(forwardSpeed));
		leftDrive.move(forwardSpeed);
		rightDrive.move(forwardSpeed);
	}
}

void VisionSensorCenter(int sig_id, int size_id = 0){
	visionSensor.set_zero_point(E_VISION_ZERO_CENTER);
	vision_object_s_t rtn = visionSensor.get_by_sig(0, sig_id);

	if (rtn.signature == sig_id){
		int turnErrorRange = 10;
		int xMiddle = rtn.x_middle_coord;
		float turnSpeed = 10 * powf(1.012, abs(xMiddle));

		if (turnErrorRange > xMiddle && xMiddle > -turnErrorRange){
			lcd::set_text(1, "Object Centered");
			int objWidth = rtn.width;
			VisionSensorMove(objWidth);
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

void opcontrol() {
	vision_signature_s_t PUR = visionSensor.signature_from_utility(1, 901, 1821, 1362, 10293, 12743, 11518, 3.000, 0);
	vision_signature_s_t GRE = visionSensor.signature_from_utility(2, -7299, -6057, -6678, -4197, -2977, -3588, 3.000, 0);
	vision_signature_s_t ORG = visionSensor.signature_from_utility(3, 6433, 9433, 7934, -2875, -2409, -2642, 3.900, 0);
	vision_signature_s_t BLU = visionSensor.signature_from_utility(4, -3205, -2095, -2650, 6807, 10725, 8766, 2.300, 0);

	visionSensor.set_signature(PURPLE_CUBE_ID, &PUR);
	visionSensor.set_signature(GREEN_CUBE_ID, &GRE);
	visionSensor.set_signature(ORANGE_CUBE_ID, &ORG);
	visionSensor.set_signature(BLUE_SPHERE_ID, &BLU);

	// Misc
	bool togglePurple = false;
	bool toggleGreen = false;
	bool toggleOrange = false;
	bool toggleBlue = false;

	while(true){
	// Toggle driver mode or different color modes
		if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)){
			if (toggleBlue || togglePurple || toggleGreen || toggleOrange){
				togglePurple = toggleGreen = toggleOrange = toggleBlue = false;
				vexController.clear();
				delay(50);
				vexController.set_text(0, 0, "Driver Mode");
				lcd::set_text(0, "Driver Mode");
				for (size_t i{1}; i<8; i++){
					lcd::clear_line(i);
				}
			}
		}else if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			if (togglePurple == false){
				togglePurple = true;
				toggleGreen = toggleOrange = toggleBlue = false;
				vexController.clear();
				delay(50);
				vexController.set_text(0, 0, "Auto Purple");
			}
		}else if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){
			if (toggleGreen == false){
				togglePurple = toggleOrange = toggleBlue = false;
				toggleGreen = true;
				vexController.clear();
				delay(50);
				vexController.set_text(0, 0, "Auto Green");
			}
		}else if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_Y)){
			if (toggleOrange == false){
				togglePurple = toggleGreen = toggleBlue = false;
				toggleOrange = true;
				vexController.clear();
				delay(50);
				vexController.set_text(0, 0, "Auto Orange");
			}
		}else if (vexController.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){
			if (toggleBlue == false){
				togglePurple = toggleGreen = toggleOrange = false;
				toggleBlue = true;
				vexController.clear();
				delay(50);
				vexController.set_text(0, 0, "Auto Blue");
			}
		}

		Intake();
		if (!(toggleBlue || togglePurple || toggleGreen || toggleOrange)){
			Chassis();
		}else if (togglePurple) {
			VisionSensorCenter(1);
		}else if (toggleGreen) {
			VisionSensorCenter(2);
		}else if (toggleOrange) {
			VisionSensorCenter(3);
		}else if (toggleBlue) {
			VisionSensorCenter(4);
		}

		delay(20);
	}
}
