#include "lemlib/config.hpp"

#include "lemlog/logger/sinks/terminal.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include <vector>

// 全域終端紀錄器，控制 LemLib 的 log 輸出
logger::Terminal terminal;

// 左/右驅動馬達群組；負號代表該馬達需反轉，360_rpm 為最高速度檔
lemlib::MotorGroup left_motors({-1, 11, -12, 13}, 360_rpm);
lemlib::MotorGroup right_motors({8, 10}, 360_rpm);

// 1 號埠的 V5 慣性感測器
lemlib::V5InertialSensor imu(1);

namespace {
// 兩個追蹤輪設定於匿名命名空間中，僅供此檔案使用
lemlib::TrackingWheel vertical_tracker('E', 'F', true, 2.75_in, 26.5_cm / 2);
lemlib::TrackingWheel horizontal_tracker('G', 'H', false, 2.75_in, -26.5_cm / 2);
} // namespace

// 里程計組合慣性與追蹤輪
lemlib::TrackingWheelOdometry odom({&imu}, {&vertical_tracker}, {&horizontal_tracker});

// 旋轉/平移 PID 預設參數
const lemlib::PID angular_pid(0.05, 0, 0);
const lemlib::PID lateral_pid(0, 0, 0);

// 封裝成函式的姿態取得器，供各種動作演算法抓取
const std::function<units::Pose()> pose_getter = [] { return odom.getPose(); };

// 預設的角度/位移離開條件
const lemlib::ExitConditionGroup<AngleRange> angular_exit_conditions(
    {lemlib::ExitCondition<AngleRange>(1_stDeg, 2_sec)});
const lemlib::ExitConditionGroup<Length> lateral_exit_conditions(
    {lemlib::ExitCondition<Length>(1_in, 500_msec)});

// 底盤寬度（用於計算左右輪差速）
const Length track_width = 26.5_cm;

// IMU 漂移補償係數
const Number drift_compensation = 0;

// 預設的角向/平移 slew 限制，控制加速度平滑度
const Number angular_slew = 1;
const Number lateral_slew = 0;
