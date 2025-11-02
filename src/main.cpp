#include "main.h"
#include "lemlib/config.hpp"
#include "lemlib/motions/follow.hpp"
#include "lemlib/motions/turnTo.hpp"
#include "pros/llemu.hpp"
#include "units/units.hpp"

// 使用修改後的 ASSET 來引用 example.txt
ASSET(example_txt);

void initialize() {
    terminal.setLoggingLevel(logger::Level::DEBUG);
    pros::lcd::initialize();

    imu.calibrate();
    pros::delay(3200);
    odom.startTask();
    pros::delay(100);
    pros::Task([] {
        while (true) {
            auto p = odom.getPose();
            pros::lcd::print(0, "X: %f", to_in(p.x));
            pros::lcd::print(1, "Y: %f", to_in(p.y));
            pros::lcd::print(2, "Theta: %f", to_cDeg(p.orientation));
            pros::delay(10);
        }
    });
    lemlib::turnTo(90_cDeg, 100_sec);
}

void disabled() {}

void autonomous() {
    // 設置起始位置 - 使用大括號初始化 Pose
    odom.setPose({0_in, 0_in, 0_cDeg});
    pros::delay(100);
    
    // 範例1: 直接使用 example.txt 檔案
    pros::lcd::print(3, "Following example.txt");
    pros::lcd::print(4, "Direct file access...");
    
    // 使用重新定義的 ASSET 來直接引用檔案
    lemlib::follow(example_txt, 8_in, 25_sec, {}, {});
    
    // 等待並重置位置
    pros::delay(2000);
    odom.setPose({0_in, 0_in, 0_cDeg});
    
    // 範例2: 使用不同的前瞻距離
    pros::lcd::print(5, "Different lookahead...");
    lemlib::follow(example_txt, 6_in, 20_sec, {}, {});
    
    // 範例3: 倒車執行路徑
    pros::delay(2000);
    // 設置在路徑終點開始倒車
    odom.setPose({27.369_in, 46.871_in, 0_cDeg});
    pros::lcd::print(6, "Reverse example.txt...");
    
    lemlib::FollowParams reverseParams;
    reverseParams.reversed = true;
    lemlib::follow(example_txt, 10_in, 25_sec, reverseParams, {});
    
    // 範例4: 使用更精確的跟隨
    pros::delay(2000);
    odom.setPose({0_in, 0_in, 0_cDeg});
    pros::lcd::print(7, "Precise follow...");
    
    // 使用更小的前瞻距離來獲得更精確的路徑跟隨
    lemlib::follow(example_txt, 4_in, 30_sec, {}, {});
    
    pros::lcd::print(0, "Example.txt complete!");
}

void opcontrol() {}
