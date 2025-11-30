#pragma once

#include "lemlib/config.hpp"
#include "lemlib/motions/turnTo.hpp"
#include "lemlib/motions/moveToPoint.hpp"
#include "lemlib/motions/moveToPose.hpp"
#include "lemlib/motions/follow.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "lemlib/PID.hpp"

namespace lemlib {

/**
 * @brief Struct containing all the sensors used for odometry
 *
 */
struct OdomSensors {
        TrackingWheel* vertical1;
        TrackingWheel* vertical2;
        TrackingWheel* horizontal1;
        TrackingWheel* horizontal2;
        V5InertialSensor* imu;
};

/**
 * @brief Struct containing constants for a drivetrain
 *
 */
struct Drivetrain {
        MotorGroup* leftMotors;
        MotorGroup* rightMotors;
        Length trackWidth;
        Length wheelDiameter;
        Number rpm;
        Number horizontalDrift;
};

/**
 * @brief Struct containing constants for a controller
 *
 */
struct ControllerSettings {
        Number kP;
        Number kI;
        Number kD;
        Number windupRange;
        Number smallError;
        Number smallErrorTimeout;
        Number largeError;
        Number largeErrorTimeout;
        Number slew;
};

/**
 * @brief Chassis class
 *
 */
class Chassis {
    public:
        /**
         * @brief Construct a new Chassis object
         *
         * @param drivetrain drivetrain to use
         * @param lateralSettings settings for the lateral PID
         * @param angularSettings settings for the angular PID
         * @param sensors sensors to use for odometry
         * @param driveCurve drive curve to use. Defaults to nullptr
         */
        Chassis(Drivetrain drivetrain, ControllerSettings lateralSettings, ControllerSettings angularSettings,
                OdomSensors sensors);

        /**
         * @brief Calibrate the sensors
         *
         * @param calibrateIMU whether to calibrate the IMU. Defaults to true
         */
        void calibrate(bool calibrateIMU = true);

        /**
         * @brief Set the Pose object
         *
         * @param pose new pose
         */
        void setPose(units::Pose pose);

        /**
         * @brief Get the Pose object
         *
         * @return units::Pose
         */
        units::Pose getPose();

        /**
         * @brief Turn to face a target
         *
         * @param target target to turn to
         * @param timeout maximum time to run the motion for
         * @param params parameters for the motion
         * @param async whether to run the motion asynchronously. Defaults to true
         */
        void turnTo(std::variant<units::Angle, units::V2Position> target, units::Time timeout,
                    TurnToParams params = {}, bool async = true);

        /**
         * @brief Move to a point
         *
         * @param target target to move to
         * @param timeout maximum time to run the motion for
         * @param params parameters for the motion
         * @param async whether to run the motion asynchronously. Defaults to true
         */
        void moveToPoint(units::V2Position target, units::Time timeout, MoveToPointParams params = {},
                         bool async = true);

        /**
         * @brief Move to a pose
         *
         * @param target target to move to
         * @param timeout maximum time to run the motion for
         * @param params parameters for the motion
         * @param async whether to run the motion asynchronously. Defaults to true
         */
        void moveToPose(units::Pose target, units::Time timeout, MoveToPoseParams params = {}, bool async = true);

        /**
         * @brief Follow a path
         *
         * @param path path to follow
         * @param lookahead lookahead distance
         * @param timeout maximum time to run the motion for
         * @param params parameters for the motion
         * @param async whether to run the motion asynchronously. Defaults to true
         */
        void follow(const asset& path, units::Length lookahead, units::Time timeout, FollowParams params = {},
                    bool async = true);

        /**
         * @brief Control the robot with a tank drive
         *
         * @param left speed of the left side of the drivetrain
         * @param right speed of the right side of the drivetrain
         * @param curveDriveScale how much to scale the curve. Defaults to 0
         */
        void tank(int left, int right, float curveDriveScale = 0.0);

        /**
         * @brief Control the robot with an arcade drive
         *
         * @param throttle speed to move forward
         * @param turn speed to turn
         * @param curveDriveScale how much to scale the curve. Defaults to 0
         * @param desaturateBias how much to bias desaturation. Defaults to 0.5
         */
        void arcade(int throttle, int turn, float curveDriveScale = 0.0, float desaturateBias = 0.5);

        /**
         * @brief Control the robot with a curvature drive
         *
         * @param throttle speed to move forward
         * @param curvature curvature to turn
         * @param curveDriveScale how much to scale the curve. Defaults to 0
         */
        void curvature(int throttle, int curvature, float curveDriveScale = 0.0);

        /**
         * @brief Wait for the current motion to finish
         *
         */
        void waitUntilDone();

        /**
         * @brief Cancel the current motion
         *
         */
        void cancelMotion();

        /**
         * @brief Request the current motion to cancel
         *
         */
        void requestMotionStart();

    protected:
        Drivetrain drivetrain;
        ControllerSettings lateralSettings;
        ControllerSettings angularSettings;
        OdomSensors sensors;

        PID lateralPID;
        PID angularPID;
        ExitConditionGroup<units::Length> lateralExitConditions;
        ExitConditionGroup<units::AngleRange> angularExitConditions;
        TrackingWheelOdometry odom;
};

} // namespace lemlib
