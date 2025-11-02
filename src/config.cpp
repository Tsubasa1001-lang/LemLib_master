#include "lemlib/config.hpp"

#include "lemlog/logger/sinks/terminal.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include <vector>

logger::Terminal terminal;

lemlib::MotorGroup left_motors({-1, 11, -12, 13}, 360_rpm);
lemlib::MotorGroup right_motors({8, 10}, 360_rpm);

lemlib::V5InertialSensor imu(1);

namespace {
lemlib::TrackingWheel vertical_tracker('E', 'F', true, 2.75_in, 26.5_cm / 2);
lemlib::TrackingWheel horizontal_tracker('G', 'H', false, 2.75_in, -26.5_cm / 2);
} // namespace

lemlib::TrackingWheelOdometry odom({&imu}, {&vertical_tracker}, {&horizontal_tracker});

const lemlib::PID angular_pid(0.05, 0, 0);
const lemlib::PID lateral_pid(0, 0, 0);

const std::function<units::Pose()> pose_getter = [] { return odom.getPose(); };

const lemlib::ExitConditionGroup<AngleRange> angular_exit_conditions(
    {lemlib::ExitCondition<AngleRange>(1_stDeg, 2_sec)});
const lemlib::ExitConditionGroup<Length> lateral_exit_conditions(
    {lemlib::ExitCondition<Length>(1_in, 500_msec)});

const Length track_width = 26.5_cm;

const Number drift_compensation = 0;

const Number angular_slew = 1;
const Number lateral_slew = 0;
