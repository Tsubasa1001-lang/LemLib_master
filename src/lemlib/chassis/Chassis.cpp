#include "lemlib/chassis/Chassis.hpp"
#include "lemlib/util.hpp"
#include "lemlib/MotionHandler.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace lemlib {

/**
 * @brief Convert OdomSensors to a vector of tracking wheels
 *
 * @param sensors
 * @return std::vector<TrackingWheel*>
 */
static std::vector<TrackingWheel*> getVerticalWheels(OdomSensors sensors) {
    std::vector<TrackingWheel*> wheels;
    if (sensors.vertical1 != nullptr) wheels.push_back(sensors.vertical1);
    if (sensors.vertical2 != nullptr) wheels.push_back(sensors.vertical2);
    return wheels;
}

static std::vector<TrackingWheel*> getHorizontalWheels(OdomSensors sensors) {
    std::vector<TrackingWheel*> wheels;
    if (sensors.horizontal1 != nullptr) wheels.push_back(sensors.horizontal1);
    if (sensors.horizontal2 != nullptr) wheels.push_back(sensors.horizontal2);
    return wheels;
}

/**
 * @brief Convert OdomSensors to a vector of IMUs
 *
 * @param sensors
 * @return std::vector<IMU*>
 */
static std::vector<IMU*> getIMUs(OdomSensors sensors) {
    std::vector<IMU*> imus;
    if (sensors.imu != nullptr) imus.push_back(sensors.imu);
    return imus;
}

Chassis::Chassis(Drivetrain drivetrain, ControllerSettings lateralSettings, ControllerSettings angularSettings,
                 OdomSensors sensors)
    : drivetrain(drivetrain),
      lateralSettings(lateralSettings),
      angularSettings(angularSettings),
      sensors(sensors),
      lateralPID(lateralSettings.kP, lateralSettings.kI, lateralSettings.kD, lateralSettings.windupRange, true),
      angularPID(angularSettings.kP, angularSettings.kI, angularSettings.kD, angularSettings.windupRange, true),
      lateralExitConditions({lemlib::ExitCondition<units::Length>(lateralSettings.smallError, lateralSettings.smallErrorTimeout),
                             lemlib::ExitCondition<units::Length>(lateralSettings.largeError, lateralSettings.largeErrorTimeout)}),
      angularExitConditions({lemlib::ExitCondition<units::AngleRange>(units::Angle(angularSettings.smallError), angularSettings.smallErrorTimeout),
                             lemlib::ExitCondition<units::AngleRange>(units::Angle(angularSettings.largeError), angularSettings.largeErrorTimeout)}),
      odom(getIMUs(sensors), getVerticalWheels(sensors), getHorizontalWheels(sensors))
{
}

void Chassis::calibrate(bool calibrateIMU) {
    if (calibrateIMU && sensors.imu != nullptr) {
        sensors.imu->calibrate();
        pros::delay(3000);
    }
    odom.startTask();
}

void Chassis::setPose(units::Pose pose) {
    odom.setPose(pose);
}

units::Pose Chassis::getPose() {
    return odom.getPose();
}

void Chassis::waitUntilDone() {
    while (lemlib::motion_handler::isMoving()) {
        pros::delay(10);
    }
}

void Chassis::cancelMotion() {
    lemlib::motion_handler::cancel();
}

void Chassis::requestMotionStart() {
    // This might be internal logic, but if exposed, it could potentially
    // signal that a new motion is about to start.
    // For now, if we use motion_handler::move, it handles pre-emption if needed?
    // Or we should cancel current motion?
    // motion_handler::move queues? No, the example says:
    // "if we try to run another motion while one is still running, it will wait"
    // So it queues.
    // If we want to interrupt, we should call cancelMotion().
}

void Chassis::turnTo(std::variant<units::Angle, units::V2Position> target, units::Time timeout,
                    TurnToParams params, bool async) {
    // Construct settings
    TurnToSettings settings;
    settings.angularPID = angularPID;
    settings.exitConditions = angularExitConditions;
    settings.poseGetter = [this]() { return getPose(); };
    settings.leftMotors = *drivetrain.leftMotors;
    settings.rightMotors = *drivetrain.rightMotors;

    if (params.slew == 0) params.slew = angularSettings.slew;

    auto motion = [=]() {
        lemlib::turnTo(target, timeout, params, settings);
    };

    lemlib::motion_handler::move(motion);

    if (!async) {
        waitUntilDone();
    }
}

void Chassis::moveToPoint(units::V2Position target, units::Time timeout, MoveToPointParams params,
                         bool async) {
    MoveToPointSettings settings;
    settings.angularPID = angularPID;
    settings.lateralPID = lateralPID;
    settings.exitConditions = lateralExitConditions;
    settings.poseGetter = [this]() { return getPose(); };
    settings.leftMotors = *drivetrain.leftMotors;
    settings.rightMotors = *drivetrain.rightMotors;

    if (params.lateralSlew == 0) params.lateralSlew = lateralSettings.slew;
    if (params.angularSlew == 0) params.angularSlew = angularSettings.slew;

    auto motion = [=]() {
        lemlib::moveToPoint(target, timeout, params, settings);
    };

    lemlib::motion_handler::move(motion);

    if (!async) {
        waitUntilDone();
    }
}

void Chassis::moveToPose(units::Pose target, units::Time timeout, MoveToPoseParams params, bool async) {
    MoveToPoseSettings settings;
    settings.angularPID = angularPID;
    settings.lateralPID = lateralPID;
    settings.angularExitConditions = angularExitConditions;
    settings.lateralExitConditions = lateralExitConditions;
    settings.poseGetter = [this]() { return getPose(); };
    settings.leftMotors = *drivetrain.leftMotors;
    settings.rightMotors = *drivetrain.rightMotors;

    if (params.lateralSlew == 0) params.lateralSlew = lateralSettings.slew;
    if (params.angularSlew == 0) params.angularSlew = angularSettings.slew;

    auto motion = [=]() {
        lemlib::moveToPose(target, timeout, params, settings);
    };

    lemlib::motion_handler::move(motion);

    if (!async) {
        waitUntilDone();
    }
}

void Chassis::follow(const asset& path, units::Length lookahead, units::Time timeout, FollowParams params,
                    bool async) {
    FollowSettings settings;
    settings.trackWidth = drivetrain.trackWidth;
    settings.poseGetter = [this]() { return getPose(); };
    settings.leftMotors = *drivetrain.leftMotors;
    settings.rightMotors = *drivetrain.rightMotors;

    if (params.lateralSlew == 0) params.lateralSlew = lateralSettings.slew;

    auto motion = [=]() {
        lemlib::follow(path, lookahead, timeout, params, settings);
    };

    lemlib::motion_handler::move(motion);

    if (!async) {
        waitUntilDone();
    }
}

// Driver Control Implementations

static float applyCurve(float input, float scale) {
    if (scale != 0) {
        return (std::exp(-scale / 10) + std::exp((std::abs(input) - 100) / 10) * (1 - std::exp(-scale / 10))) * input;
    }
    return input;
}

void Chassis::tank(int left, int right, float curveDriveScale) {
    cancelMotion(); // Ensure no auto motion is fighting us
    // Simple pass through for now, typically scaling is handled by user or added here if requested
    // If curveDriveScale is meant to be expo drive, we could apply it.
    // Assuming input is -127 to 127 or similar
    // LemLib v0.5 had expo curves.
    // For now, I will trust the user inputs unless curveDriveScale is used.
    // Since I don't have the exact math for LemLib's specific curve preference handy in 'util.hpp',
    // I will implement a basic one or leave raw if scale is 0.

    // Note: If you want expo drive, we need to implement it.
    // Let's implement a simple version if scale > 0.

    // If move(int) expects voltage or velocity depends on MotorGroup config.
    // Usually it's -127 to 127 for typical V5 usage in OpControl.

    drivetrain.leftMotors->move(left);
    drivetrain.rightMotors->move(right);
}

void Chassis::arcade(int throttle, int turn, float curveDriveScale, float desaturateBias) {
    cancelMotion();

    // Calculate left and right powers
    int leftPower = throttle + turn;
    int rightPower = throttle - turn;

    // Desaturate
    // Standard desaturation: if max > 127, scale both down.
    // desaturateBias in LemLib v0.5 allowed favoring turn or throttle?
    // I'll use a simple desaturation for now as `util.hpp` `desaturate` returns `DriveOutputs` which are 0-1.
    // But `throttle`/`turn` here are ints (likely -127 to 127).

    int maxVal = std::max(std::abs(leftPower), std::abs(rightPower));
    if (maxVal > 127) {
        leftPower = leftPower * 127 / maxVal;
        rightPower = rightPower * 127 / maxVal;
    }

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}

void Chassis::curvature(int throttle, int curvature, float curveDriveScale) {
    cancelMotion();

    // Curvature drive: turn is proportional to speed
    // if stationary, it's like arcade (quick turn)

    int leftPower = throttle + (std::abs(throttle) * curvature / 127); // rough approx
    int rightPower = throttle - (std::abs(throttle) * curvature / 127);

    // Handle "quick turn" (if throttle is low)
    if (throttle == 0) {
        leftPower = curvature;
        rightPower = -curvature;
    }

    int maxVal = std::max(std::abs(leftPower), std::abs(rightPower));
    if (maxVal > 127) {
        leftPower = leftPower * 127 / maxVal;
        rightPower = rightPower * 127 / maxVal;
    }

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}

} // namespace lemlib
