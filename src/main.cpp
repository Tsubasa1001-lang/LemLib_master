#include "main.h"
#include "lemlib/config.hpp"
#include "pros/llemu.hpp"
#include "units/units.hpp"

// 使用修改後的 ASSET 來引用 example.txt
ASSET(example_txt);

void initialize() {
    terminal.setLoggingLevel(logger::Level::DEBUG);
    pros::lcd::initialize();

    // Use chassis for calibration
    chassis.calibrate();

    // Odom task is started inside chassis.calibrate() in my implementation for now,
    // or we can call it explicitly if needed, but I put it in calibrate.
    // Wait, in my Chassis.cpp I added odom.startTask() to calibrate().
    // So we don't need to call it again.

    pros::delay(100);
    pros::Task([] {
        while (true) {
            auto p = chassis.getPose(); // Use chassis.getPose()
            pros::lcd::print(0, "X: %f", to_in(p.x));
            pros::lcd::print(1, "Y: %f", to_in(p.y));
            pros::lcd::print(2, "Theta: %f", to_cDeg(p.orientation));
            pros::delay(10);
        }
    });

    // Use chassis.turnTo instead of lemlib::turnTo
    chassis.turnTo(90_cDeg, 100_sec);
}

void disabled() {}

void autonomous() {
    // 設置起始位置
    chassis.setPose({0_in, 0_in, 0_cDeg});
    pros::delay(100);
    
    // 範例1: 直接使用 example.txt 檔案
    pros::lcd::print(3, "Following example.txt");
    pros::lcd::print(4, "Direct file access...");
    
    chassis.follow(example_txt, 8_in, 25_sec);
    
    // 等待並重置位置
    pros::delay(2000);
    chassis.setPose({0_in, 0_in, 0_cDeg});
    
    // 範例2: 使用不同的前瞻距離
    pros::lcd::print(5, "Different lookahead...");
    chassis.follow(example_txt, 6_in, 20_sec);
    
    // 範例3: 倒車執行路徑
    pros::delay(2000);
    // 設置在路徑終點開始倒車
    chassis.setPose({27.369_in, 46.871_in, 0_cDeg});
    pros::lcd::print(6, "Reverse example.txt...");
    
    lemlib::FollowParams reverseParams;
    reverseParams.reversed = true;
    chassis.follow(example_txt, 10_in, 25_sec, reverseParams);
    
    // 範例4: 使用更精確的跟隨
    pros::delay(2000);
    chassis.setPose({0_in, 0_in, 0_cDeg});
    pros::lcd::print(7, "Precise follow...");
    
    // 使用更小的前瞻距離來獲得更精確的路徑跟隨
    chassis.follow(example_txt, 4_in, 30_sec);
    
    pros::lcd::print(0, "Example.txt complete!");
}

void opcontrol() {
    // Basic arcade control example
    while (true) {
        int throttle = 0; // Get from controller
        int turn = 0; // Get from controller
        chassis.arcade(throttle, turn);
        pros::delay(10);
    }
}
