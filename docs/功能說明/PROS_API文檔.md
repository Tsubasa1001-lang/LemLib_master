# PROS API 完整文檔

VEX V5 PROS 框架感測器與執行器控制 API 參考手冊

---

## 📋 目錄

- [📡 GPS 感測器 API](#-gps-感測器-api)
- [📏 Distance 感測器 API](#-distance-感測器-api)
- [🌈 Optical 感測器 API](#-optical-感測器-api)
- [🔌 ADI (TriPort) API](#-adi-triport-api)
- [📊 AnalogIn - 模擬輸入](#-analogin---模擬輸入)
- [🔊 AnalogOut - 模擬輸出](#-analogout---模擬輸出)
- [💡 DigitalOut - 數位輸出](#-digitalout---數位輸出)
- [🔍 DigitalIn - 數位輸入](#-digitalin---數位輸入)
- [🖥️ LCD (LLEMU) API](#-lcd-llemu-api)
- [📚 快速參考表](#-快速參考表)
- [💡 最佳實踐](#-最佳實踐)

---

## 📡 GPS 感測器 API

### 📖 概述
VEX GPS 感測器提供場地絕對定位、方向和慣性測量功能。

---

### 🎯 創建 GPS 物件

#### 1. 基本創建
```cpp
pros::Gps gps(std::uint8_t port);
```

**參數**:
- `port`: V5 智能埠號 (1-21)

**範例**:
```cpp
// 創建 GPS 感測器 (埠 1)
pros::Gps gps(1);
```

---

#### 2. 帶初始位置創建
```cpp
pros::Gps gps(std::uint8_t port, double xInitial, double yInitial, double headingInitial);
```

**參數**:
- `port`: V5 智能埠號 (1-21)
- `xInitial`: 初始 X 位置 (米) - 場地中心為 (0,0)
- `yInitial`: 初始 Y 位置 (米)
- `headingInitial`: 初始航向 (度) - 0°=北, 90°=東, 180°=南, 270°=西

**範例**:
```cpp
// 創建 GPS 並設置初始位置
pros::Gps gps(1, 1.30, 1.20, 90);  // X=1.3m, Y=1.2m, 航向=90°
```

---

#### 3. 帶偏移量創建
```cpp
pros::Gps gps(std::uint8_t port, double xOffset, double yOffset);
```

**參數**:
- `xOffset`: GPS 相對於機器人旋轉中心的 X 偏移 (米)
- `yOffset`: GPS 相對於機器人旋轉中心的 Y 偏移 (米)

**範例**:
```cpp
// GPS 安裝在機器人中心前方 10cm, 右側 5cm
pros::Gps gps(1, 0.10, -0.05);
```

---

#### 4. 完整初始化創建
```cpp
pros::Gps gps(std::uint8_t port, double xInitial, double yInitial, 
              double headingInitial, double xOffset, double yOffset);
```

**範例**:
```cpp
// 完整初始化: 位置 + 偏移量
pros::Gps gps(1, 1.30, 1.20, 180, 0.10, -0.05);
```

---

### 🎮 GPS 設置方法

#### 1. `initialize_full()` - 完整初始化
```cpp
std::int32_t initialize_full(double xInitial, double yInitial, 
                             double headingInitial, double xOffset, double yOffset);
```

設置 GPS 的初始位置和偏移量。

**返回值**:
- `1`: 成功
- `PROS_ERR`: 失敗

**範例**:
```cpp
pros::Gps gps(1);
gps.initialize_full(1.30, 1.20, 180, 0.10, -0.05);
// 位置: (1.3, 1.2), 航向: 180°, 偏移: (0.1, -0.05)
```

---

#### 2. `set_offset()` - 設置偏移量
```cpp
std::int32_t set_offset(double xOffset, double yOffset);
```

設置 GPS 相對於機器人旋轉中心的偏移量。

**範例**:
```cpp
pros::Gps gps(1);
gps.set_offset(0.10, -0.05);  // GPS 在中心前方 10cm, 右側 5cm
```

---

#### 3. `set_position()` - 設置位置
```cpp
std::int32_t set_position(double xInitial, double yInitial, double headingInitial);
```

設置機器人在場地上的位置和航向。

**參數**:
- `xInitial`: X 位置 (米) - 場地中心為原點
- `yInitial`: Y 位置 (米)
- `headingInitial`: 航向 (度) - 0-360°, 0°為北

**範例**:
```cpp
pros::Gps gps(1);

// 設置到場地中心
gps.set_position(0.0, 0.0, 0);

// 設置到右上角
gps.set_position(1.5, 1.5, 45);
```

---

#### 4. `set_data_rate()` - 設置數據更新率
```cpp
std::int32_t set_data_rate(std::uint32_t rate);
```

設置 GPS IMU 數據的更新週期。

**參數**:
- `rate`: 更新週期 (毫秒), 最小 5ms

**範例**:
```cpp
pros::Gps gps(1);
gps.set_data_rate(5);   // 5ms 更新週期 (200Hz)
gps.set_data_rate(10);  // 10ms 更新週期 (100Hz)
```

---

### 📊 GPS 讀取方法

#### 5. `get_position()` - 獲取位置
```cpp
pros::gps_position_s_t get_position();
```

獲取機器人在場地上的 X, Y 位置。

**返回值**:
```cpp
struct gps_position_s_t {
    double x;  // X 位置 (米)
    double y;  // Y 位置 (米)
};
```

**範例**:
```cpp
pros::Gps gps(1);

pros::gps_position_s_t pos = gps.get_position();
printf("位置: X=%.2f m, Y=%.2f m\n", pos.x, pos.y);

// 在循環中監控位置
while (true) {
    auto pos = gps.get_position();
    pros::lcd::print(0, "X: %.2f  Y: %.2f", pos.x, pos.y);
    pros::delay(50);
}
```

---

#### 6. `get_position_x()` - 獲取 X 位置
```cpp
double get_position_x();
```

**返回值**: X 位置 (米), 失敗返回 `PROS_ERR_F`

**範例**:
```cpp
double x = gps.get_position_x();
printf("X 位置: %.2f m\n", x);
```

---

#### 7. `get_position_y()` - 獲取 Y 位置
```cpp
double get_position_y();
```

**範例**:
```cpp
double y = gps.get_position_y();
printf("Y 位置: %.2f m\n", y);
```

---

#### 8. `get_offset()` - 獲取偏移量
```cpp
pros::gps_position_s_t get_offset();
```

獲取 GPS 相對於機器人旋轉中心的偏移量。

**範例**:
```cpp
pros::gps_position_s_t offset = gps.get_offset();
printf("偏移量: X=%.2f m, Y=%.2f m\n", offset.x, offset.y);
```

---

#### 9. `get_position_and_orientation()` - 獲取位置和方向
```cpp
pros::gps_status_s_t get_position_and_orientation();
```

獲取位置、航向、俯仰和橫滾。

**返回值**:
```cpp
struct gps_status_s_t {
    double x;       // X 位置 (米)
    double y;       // Y 位置 (米)
    double pitch;   // 俯仰角 (度)
    double roll;    // 橫滾角 (度)
    double yaw;     // 偏航角/航向 (度)
};
```

**範例**:
```cpp
pros::gps_status_s_t status = gps.get_position_and_orientation();

printf("位置: X=%.2f, Y=%.2f\n", status.x, status.y);
printf("姿態: Roll=%.1f°, Pitch=%.1f°, Yaw=%.1f°\n", 
       status.roll, status.pitch, status.yaw);

// 監控完整狀態
while (true) {
    auto status = gps.get_position_and_orientation();
    pros::lcd::print(0, "X:%.2f Y:%.2f", status.x, status.y);
    pros::lcd::print(1, "Yaw:%.1f", status.yaw);
    pros::delay(50);
}
```

---

#### 10. `get_error()` - 獲取誤差
```cpp
double get_error();
```

獲取 GPS 位置的可能 RMS (均方根) 誤差。

**返回值**: RMS 誤差 (米), 失敗返回 `PROS_ERR_F`

**範例**:
```cpp
double error = gps.get_error();
if (error < 0.1) {
    printf("GPS 精度良好: %.3f m\n", error);
} else {
    printf("警告: GPS 誤差較大: %.3f m\n", error);
}
```

---

### 💡 GPS 完整使用範例

```cpp
void initialize() {
    // 創建 GPS (埠 1)
    pros::Gps gps(1);
    
    // 設置初始位置 (場地右下角)
    gps.set_position(1.5, -1.5, 0);
    
    // 設置偏移量 (GPS 在中心前方 15cm)
    gps.set_offset(0.15, 0);
    
    // 設置數據更新率
    gps.set_data_rate(10);  // 10ms
    
    pros::delay(100);
}

void autonomous() {
    pros::Gps gps(1);
    
    // 獲取當前位置
    auto pos = gps.get_position();
    printf("起始位置: X=%.2f, Y=%.2f\n", pos.x, pos.y);
    
    // 監控位置和方向
    while (true) {
        auto status = gps.get_position_and_orientation();
        
        // 檢查誤差
        double error = gps.get_error();
        
        printf("位置: (%.2f, %.2f), 航向: %.1f°, 誤差: %.3fm\n",
               status.x, status.y, status.yaw, error);
        
        pros::delay(100);
    }
}

void opcontrol() {
    pros::Gps gps(1);
    
    // 顯示位置在 LCD
    pros::Task display_task([&gps]() {
        while (true) {
            auto pos = gps.get_position();
            auto status = gps.get_position_and_orientation();
            
            pros::lcd::print(0, "GPS Position:");
            pros::lcd::print(1, "X: %.2f m  Y: %.2f m", pos.x, pos.y);
            pros::lcd::print(2, "Heading: %.1f degrees", status.yaw);
            pros::lcd::print(3, "Error: %.3f m", gps.get_error());
            
            pros::delay(50);
        }
    });
    
    // 主控制循環
    while (true) {
        // 您的控制代碼
        pros::delay(20);
    }
}
```

---

## 📏 Distance 感測器 API

### 📖 概述
VEX Distance 感測器使用超聲波測量距離,提供距離、信心度和物體大小資訊。

---

### 🎯 創建 Distance 物件

```cpp
pros::Distance distance(std::uint8_t port);
```

**參數**:
- `port`: V5 智能埠號 (1-21)

**範例**:
```cpp
pros::Distance dist(10);  // 埠 10
```

---

### 📊 Distance 讀取方法

#### 1. `get()` / `get_distance()` - 獲取距離
```cpp
std::int32_t get();
std::int32_t get_distance();  // 與 get() 相同
```

獲取當前測量距離 (毫米)。

**返回值**:
- 距離值 (mm)
- `9999`: 無法檢測到物體
- `PROS_ERR`: 錯誤

**範例**:
```cpp
pros::Distance dist(10);

std::int32_t distance = dist.get();
if (distance == 9999) {
    printf("未檢測到物體\n");
} else if (distance != PROS_ERR) {
    printf("距離: %d mm (%.1f cm)\n", distance, distance / 10.0);
}

// 轉換為不同單位
double cm = distance / 10.0;
double inches = distance / 25.4;
printf("距離: %.1f cm / %.1f in\n", cm, inches);
```

---

#### 2. `get_confidence()` - 獲取信心度
```cpp
std::int32_t get_confidence();
```

獲取距離讀數的信心度。

**返回值**:
- `0-63`: 信心度值
  - `63`: 高信心度
  - `0`: 低信心度
- 僅在距離 > 200mm 時有效

**範例**:
```cpp
std::int32_t confidence = dist.get_confidence();

if (confidence > 50) {
    printf("高信心度: %d\n", confidence);
} else if (confidence > 30) {
    printf("中等信心度: %d\n", confidence);
} else {
    printf("低信心度: %d\n", confidence);
}
```

---

#### 3. `get_object_size()` - 獲取物體大小
```cpp
std::int32_t get_object_size();
```

獲取相對物體大小估計。

**返回值**:
- `0-400`: 大小值
- `-1`: 無法確定物體大小
- 18" × 30" 灰卡在典型照明下約為 75

**範例**:
```cpp
std::int32_t size = dist.get_object_size();

if (size == -1) {
    printf("無法確定物體大小\n");
} else if (size > 100) {
    printf("大物體: %d\n", size);
} else if (size > 50) {
    printf("中等物體: %d\n", size);
} else {
    printf("小物體: %d\n", size);
}
```

---

#### 4. `get_object_velocity()` - 獲取物體速度
```cpp
double get_object_velocity();
```

獲取物體的速度 (m/s)。

**返回值**:
- 速度值 (m/s)
- 正值: 接近
- 負值: 遠離
- `PROS_ERR_F`: 錯誤

**範例**:
```cpp
double velocity = dist.get_object_velocity();

if (velocity > 0.1) {
    printf("物體接近: %.2f m/s\n", velocity);
} else if (velocity < -0.1) {
    printf("物體遠離: %.2f m/s\n", abs(velocity));
} else {
    printf("物體靜止\n");
}
```

---

### 💡 Distance 完整使用範例

```cpp
void opcontrol() {
    pros::Distance dist(10);
    
    while (true) {
        // 獲取所有數據
        std::int32_t distance = dist.get();
        std::int32_t confidence = dist.get_confidence();
        std::int32_t size = dist.get_object_size();
        double velocity = dist.get_object_velocity();
        
        // 顯示在終端
        printf("=== Distance Sensor ===\n");
        
        if (distance == 9999) {
            printf("狀態: 未檢測到物體\n");
        } else {
            printf("距離: %d mm (%.1f cm)\n", distance, distance / 10.0);
            printf("信心度: %d/63\n", confidence);
            
            if (size != -1) {
                printf("物體大小: %d\n", size);
            }
            
            printf("速度: %.2f m/s", velocity);
            if (velocity > 0.05) {
                printf(" (接近)\n");
            } else if (velocity < -0.05) {
                printf(" (遠離)\n");
            } else {
                printf(" (靜止)\n");
            }
        }
        
        // 顯示在 LCD
        if (distance != 9999) {
            pros::lcd::print(0, "Dist: %d mm", distance);
            pros::lcd::print(1, "Confidence: %d", confidence);
            pros::lcd::print(2, "Velocity: %.2f m/s", velocity);
        } else {
            pros::lcd::print(0, "No object detected");
        }
        
        pros::delay(100);
    }
}

// 使用範例: 障礙物檢測
void autonomous() {
    pros::Distance dist(10);
    
    while (true) {
        std::int32_t distance = dist.get();
        std::int32_t confidence = dist.get_confidence();
        
        // 只在高信心度時使用數據
        if (confidence > 40 && distance != 9999) {
            if (distance < 200) {  // 20cm
                printf("警告: 障礙物太近! %d mm\n", distance);
                // 停止機器人
                break;
            } else if (distance < 500) {  // 50cm
                printf("注意: 接近障礙物 %d mm\n", distance);
                // 減速
            }
        }
        
        pros::delay(50);
    }
}
```

---

## 🌈 Optical 感測器 API

### 📖 概述
VEX Optical 感測器可檢測顏色、亮度、接近度和手勢。

---

### 🎯 創建 Optical 物件

```cpp
pros::Optical optical(std::uint8_t port);
```

**範例**:
```cpp
pros::Optical optical(15);  // 埠 15
```

---

### 🎨 顏色檢測方法

#### 1. `get_hue()` - 獲取色相
```cpp
double get_hue();
```

獲取檢測到的顏色色相 (HSV 色彩空間)。

**返回值**:
- `0-359.999`: 色相值
  - `0°`: 紅色
  - `60°`: 黃色
  - `120°`: 綠色
  - `180°`: 青色
  - `240°`: 藍色
  - `300°`: 洋紅色
- `PROS_ERR_F`: 錯誤

**注意**: 啟用手勢檢測時不可用

**範例**:
```cpp
double hue = optical.get_hue();

if (hue < 30 || hue > 330) {
    printf("檢測到紅色\n");
} else if (hue > 90 && hue < 150) {
    printf("檢測到綠色\n");
} else if (hue > 210 && hue < 270) {
    printf("檢測到藍色\n");
}
```

---

#### 2. `get_saturation()` - 獲取飽和度
```cpp
double get_saturation();
```

獲取顏色飽和度。

**返回值**:
- `0.0-1.0`: 飽和度
  - `0.0`: 灰色 (無色)
  - `1.0`: 純色

**範例**:
```cpp
double saturation = optical.get_saturation();

if (saturation > 0.8) {
    printf("鮮豔的顏色: %.2f\n", saturation);
} else if (saturation > 0.4) {
    printf("中等飽和度: %.2f\n", saturation);
} else {
    printf("低飽和度/灰色: %.2f\n", saturation);
}
```

---

#### 3. `get_brightness()` - 獲取亮度
```cpp
double get_brightness();
```

獲取顏色亮度。

**返回值**:
- `0.0-1.0`: 亮度
  - `0.0`: 黑色
  - `1.0`: 白色

**範例**:
```cpp
double brightness = optical.get_brightness();

if (brightness > 0.8) {
    printf("非常亮\n");
} else if (brightness < 0.2) {
    printf("非常暗\n");
}
```

---

#### 4. `get_proximity()` - 獲取接近度
```cpp
std::int32_t get_proximity();
```

獲取物體接近度。

**返回值**:
- `0-255`: 接近度值
  - `255`: 非常接近
  - `0`: 遠離

**範例**:
```cpp
std::int32_t proximity = optical.get_proximity();

if (proximity > 200) {
    printf("物體非常接近\n");
} else if (proximity > 100) {
    printf("物體接近\n");
} else {
    printf("物體遠離\n");
}
```

---

### 🔴 RGB 顏色數據

#### 5. `get_rgb()` - 獲取處理後的 RGB
```cpp
pros::c::optical_rgb_s_t get_rgb();
```

獲取處理後的 RGBC 數據。

**返回值**:
```cpp
struct optical_rgb_s_t {
    double red;         // 紅色通道 (0.0-1.0)
    double green;       // 綠色通道 (0.0-1.0)
    double blue;        // 藍色通道 (0.0-1.0)
    double brightness;  // 亮度 (0.0-1.0)
};
```

**範例**:
```cpp
pros::c::optical_rgb_s_t rgb = optical.get_rgb();

printf("RGB: R=%.2f, G=%.2f, B=%.2f\n", 
       rgb.red, rgb.green, rgb.blue);
printf("亮度: %.2f\n", rgb.brightness);

// 判斷主要顏色
if (rgb.red > rgb.green && rgb.red > rgb.blue) {
    printf("主要顏色: 紅色\n");
} else if (rgb.green > rgb.red && rgb.green > rgb.blue) {
    printf("主要顏色: 綠色\n");
} else if (rgb.blue > rgb.red && rgb.blue > rgb.green) {
    printf("主要顏色: 藍色\n");
}
```

---

#### 6. `get_raw()` - 獲取原始 RGB
```cpp
pros::c::optical_raw_s_t get_raw();
```

獲取未處理的原始 RGBC 數據。

**返回值**:
```cpp
struct optical_raw_s_t {
    uint32_t clear;  // 清除通道
    uint32_t red;    // 紅色通道
    uint32_t green;  // 綠色通道
    uint32_t blue;   // 藍色通道
};
```

**範例**:
```cpp
pros::c::optical_raw_s_t raw = optical.get_raw();

printf("Raw RGBC:\n");
printf("Red: %u\n", raw.red);
printf("Green: %u\n", raw.green);
printf("Blue: %u\n", raw.blue);
printf("Clear: %u\n", raw.clear);
```

---

### ✋ 手勢檢測方法

#### 7. `enable_gesture()` - 啟用手勢
```cpp
std::int32_t enable_gesture();
```

啟用手勢檢測功能。

**注意**: 啟用手勢後,顏色檢測功能將不可用

**返回值**:
- `1`: 成功
- `PROS_ERR`: 失敗

**範例**:
```cpp
optical.enable_gesture();
printf("手勢檢測已啟用\n");
```

---

#### 8. `disable_gesture()` - 禁用手勢
```cpp
std::int32_t disable_gesture();
```

**範例**:
```cpp
optical.disable_gesture();
printf("手勢檢測已禁用\n");
```

---

#### 9. `get_gesture()` - 獲取手勢
```cpp
pros::c::optical_direction_e_t get_gesture();
```

獲取最近檢測到的手勢。

**返回值**:
- `0`: 無手勢
- `1`: 向上 (朝向電纜)
- `2`: 向下
- `3`: 向右
- `4`: 向左
- `PROS_ERR`: 錯誤

**注意**: 手勢在 500ms 後清除

**範例**:
```cpp
optical.enable_gesture();

while (true) {
    auto gesture = optical.get_gesture();
    
    switch (gesture) {
        case 1:
            printf("手勢: 向上\n");
            break;
        case 2:
            printf("手勢: 向下\n");
            break;
        case 3:
            printf("手勢: 向右\n");
            break;
        case 4:
            printf("手勢: 向左\n");
            break;
        case 0:
            // 無手勢
            break;
    }
    
    pros::delay(50);
}
```

---

#### 10. `get_gesture_raw()` - 獲取原始手勢數據
```cpp
pros::c::optical_gesture_s_t get_gesture_raw();
```

獲取詳細的手勢原始數據。

**返回值**:
```cpp
struct optical_gesture_s_t {
    uint8_t udata;   // 向上數據
    uint8_t ddata;   // 向下數據
    uint8_t ldata;   // 向左數據
    uint8_t rdata;   // 向右數據
    uint8_t type;    // 手勢類型
    uint8_t count;   // 手勢計數
    uint32_t time;   // 時間戳
};
```

**範例**:
```cpp
optical.enable_gesture();

while (true) {
    pros::c::optical_gesture_s_t gesture = optical.get_gesture_raw();
    
    printf("手勢原始數據:\n");
    printf("  上: %u, 下: %u, 左: %u, 右: %u\n", 
           gesture.udata, gesture.ddata, 
           gesture.ldata, gesture.rdata);
    printf("  類型: %u, 計數: %u, 時間: %u\n", 
           gesture.type, gesture.count, gesture.time);
    
    pros::delay(100);
}
```

---

### 💡 LED 控制方法

#### 11. `set_led_pwm()` - 設置 LED 亮度
```cpp
std::int32_t set_led_pwm(uint8_t value);
```

設置白色 LED 的 PWM 值。

**參數**:
- `value`: PWM 值 (0-100)
  - `0`: LED 關閉
  - `100`: 最大亮度

**範例**:
```cpp
optical.set_led_pwm(100);  // 全亮
optical.set_led_pwm(50);   // 50% 亮度
optical.set_led_pwm(0);    // 關閉
```

---

#### 12. `get_led_pwm()` - 獲取 LED 亮度
```cpp
std::int32_t get_led_pwm();
```

**範例**:
```cpp
int pwm = optical.get_led_pwm();
printf("LED 亮度: %d%%\n", pwm);
```

---

### ⏱️ 積分時間設置

#### 13. `set_integration_time()` - 設置積分時間
```cpp
std::int32_t set_integration_time(double time);
```

設置感測器的積分時間 (更新率)。

**參數**:
- `time`: 積分時間 (ms)
  - 最小: 3 ms
  - 最大: 712 ms
  - 預設: 100 ms

**範例**:
```cpp
optical.set_integration_time(50);   // 50ms 更新率
optical.set_integration_time(100);  // 預設 100ms
```

---

#### 14. `get_integration_time()` - 獲取積分時間
```cpp
double get_integration_time();
```

**範例**:
```cpp
double time = optical.get_integration_time();
printf("積分時間: %.1f ms\n", time);
```

---

### 💡 Optical 完整使用範例

#### 顏色檢測範例
```cpp
void opcontrol() {
    pros::Optical optical(15);
    
    // 設置 LED 全亮
    optical.set_led_pwm(100);
    
    while (true) {
        // 獲取 HSV 數據
        double hue = optical.get_hue();
        double saturation = optical.get_saturation();
        double brightness = optical.get_brightness();
        int proximity = optical.get_proximity();
        
        // 顯示顏色信息
        printf("=== Optical Sensor ===\n");
        printf("Hue: %.1f°\n", hue);
        printf("Saturation: %.2f\n", saturation);
        printf("Brightness: %.2f\n", brightness);
        printf("Proximity: %d/255\n", proximity);
        
        // 判斷顏色
        if (saturation > 0.5 && proximity > 100) {
            if (hue < 30 || hue > 330) {
                printf("顏色: 紅色\n");
            } else if (hue > 90 && hue < 150) {
                printf("顏色: 綠色\n");
            } else if (hue > 210 && hue < 270) {
                printf("顏色: 藍色\n");
            } else if (hue > 40 && hue < 70) {
                printf("顏色: 黃色\n");
            }
        }
        
        // 獲取 RGB 數據
        auto rgb = optical.get_rgb();
        printf("RGB: R=%.2f G=%.2f B=%.2f\n", 
               rgb.red, rgb.green, rgb.blue);
        
        pros::delay(100);
    }
}
```

#### 手勢檢測範例
```cpp
void opcontrol() {
    pros::Optical optical(15);
    
    // 啟用手勢檢測
    optical.enable_gesture();
    optical.set_led_pwm(50);
    
    printf("手勢檢測已啟用\n");
    printf("在感測器前揮手:\n");
    printf("  ↑ 向上  ↓ 向下  ← 向左  → 向右\n");
    
    while (true) {
        auto gesture = optical.get_gesture();
        
        if (gesture != 0) {
            switch (gesture) {
                case 1:
                    printf("✓ 向上手勢\n");
                    // 執行動作
                    break;
                case 2:
                    printf("✓ 向下手勢\n");
                    break;
                case 3:
                    printf("✓ 向右手勢\n");
                    break;
                case 4:
                    printf("✓ 向左手勢\n");
                    break;
            }
            
            // 獲取詳細數據
            auto raw = optical.get_gesture_raw();
            printf("  原始數據: U=%u D=%u L=%u R=%u\n", 
                   raw.udata, raw.ddata, raw.ldata, raw.rdata);
        }
        
        pros::delay(50);
    }
}
```

#### 球檢測範例 (自主程序)
```cpp
void autonomous() {
    pros::Optical optical(15);
    optical.set_led_pwm(100);
    
    while (true) {
        double hue = optical.get_hue();
        double saturation = optical.get_saturation();
        int proximity = optical.get_proximity();
        
        // 檢測紅球
        if (proximity > 150 && saturation > 0.6) {
            if (hue < 30 || hue > 330) {
                printf("發現紅球! 接近度: %d\n", proximity);
                // 抓取球的動作
                break;
            }
        }
        
        // 檢測藍球
        if (proximity > 150 && saturation > 0.6) {
            if (hue > 210 && hue < 270) {
                printf("發現藍球! 接近度: %d\n", proximity);
                // 排出球的動作
            }
        }
        
        pros::delay(50);
    }
}
```

---

## 🔌 ADI (TriPort) API

### 📖 概述
ADI (Analog/Digital Interface) 提供 8 個三線埠 (A-H 或 1-8),支援多種感測器和執行器類型。

---

### 🎯 ADI 埠類型

```cpp
// 埠類型枚舉
E_ADI_ANALOG_IN      // 模擬輸入
E_ADI_ANALOG_OUT     // 模擬輸出
E_ADI_DIGITAL_IN     // 數位輸入
E_ADI_DIGITAL_OUT    // 數位輸出
E_ADI_LEGACY_GYRO    // 陀螺儀
E_ADI_LEGACY_ACCELEROMETER  // 加速度計
E_ADI_LEGACY_SERVO   // 伺服馬達
E_ADI_LEGACY_PWM     // PWM 輸出
E_ADI_LEGACY_ENCODER // 編碼器
E_ADI_LEGACY_ULTRASONIC  // 超聲波
E_ADI_LEGACY_BUTTON  // 按鈕
E_ADI_LEGACY_POT     // 電位器
E_ADI_LEGACY_LINE_SENSOR  // 線感測器
E_ADI_LEGACY_LIGHT_SENSOR // 光感測器
E_ADI_TYPE_UNDEFINED // 未定義
```

---

### 📍 埠號指定方式

```cpp
// 方法 1: 數字 (1-8)
pros::adi::AnalogIn sensor(1);

// 方法 2: 字母 (小寫)
pros::adi::AnalogIn sensor('a');

// 方法 3: 字母 (大寫)
pros::adi::AnalogIn sensor('A');

// 方法 4: 外部 ADI 擴展器 (智能埠 + ADI 埠)
pros::adi::AnalogIn sensor({{10, 'A'}});  // 智能埠 10, ADI 埠 A
```

---

## 📊 AnalogIn - 模擬輸入

### 🎯 創建 AnalogIn

```cpp
// 直接連接到大腦
pros::adi::AnalogIn sensor(std::uint8_t adi_port);

// 透過 ADI 擴展器
pros::adi::AnalogIn sensor(ext_adi_port_pair_t port_pair);
```

**範例**:
```cpp
// 埠 A
pros::adi::AnalogIn pot('A');

// ADI 擴展器 (智能埠 10, ADI 埠 B)
pros::adi::AnalogIn line_sensor({{10, 'B'}});
```

---

### 📊 AnalogIn 方法

#### 1. `get_value()` - 獲取原始值
```cpp
std::int32_t get_value();
```

讀取 12 位模擬值 (0-4095)。

**返回值**:
- `0-4095`: 模擬值
  - `0`: 接近 0V
  - `4095`: 接近 5V

**範例**:
```cpp
pros::adi::AnalogIn pot('A');

int value = pot.get_value();
printf("原始值: %d\n", value);

// 轉換為電壓
double voltage = value * 5.0 / 4095.0;
printf("電壓: %.2f V\n", voltage);
```

---

#### 2. `calibrate()` - 校準
```cpp
std::int32_t calibrate();
```

校準感測器並返回校準值。採集約 500 個樣本 (1ms 間隔, 0.5s 週期)。

**返回值**: 平均感測器值

**注意**: 校準時感測器值應保持穩定

**範例**:
```cpp
pros::adi::AnalogIn gyro('A');

printf("開始校準,請保持靜止...\n");
int cal_value = gyro.calibrate();
printf("校準完成,基準值: %d\n", cal_value);
```

---

#### 3. `get_value_calibrated()` - 獲取校準值 (12位)
```cpp
std::int32_t get_value_calibrated();
```

獲取相對於校準基準的值。

**返回值**:
- `-4095` 到 `4095`: 相對值

**注意**: 不適合積分運算,使用 `get_value_calibrated_HR()` 代替

**範例**:
```cpp
pros::adi::AnalogIn gyro('A');
gyro.calibrate();

while (true) {
    int value = gyro.get_value_calibrated();
    printf("相對值: %d\n", value);
    pros::delay(50);
}
```

---

#### 4. `get_value_calibrated_HR()` - 獲取高解析度校準值 (16位)
```cpp
std::int32_t get_value_calibrated_HR();
```

獲取 16 位高解析度校準值 (真實值 × 16)。

**返回值**:
- `-16384` 到 `16384`: 高解析度值

**用途**: 適合陀螺儀和加速度計等需要積分的感測器

**範例**:
```cpp
pros::adi::AnalogIn gyro('A');
gyro.calibrate();

int accumulated = 0;

while (true) {
    int value_hr = gyro.get_value_calibrated_HR();
    accumulated += value_hr;
    
    // 轉換回真實值 (除以 16)
    int real_value = accumulated / 16;
    printf("累積值: %d\n", real_value);
    
    pros::delay(10);
}
```

---

### 💡 AnalogIn 使用範例

#### 電位器範例
```cpp
void opcontrol() {
    pros::adi::AnalogIn potentiometer('A');
    
    while (true) {
        int value = potentiometer.get_value();
        
        // 轉換為角度 (假設 270° 範圍)
        double angle = (value / 4095.0) * 270.0;
        printf("電位器: %d  角度: %.1f°\n", value, angle);
        
        // 轉換為百分比
        double percent = (value / 4095.0) * 100.0;
        pros::lcd::print(0, "Position: %.1f%%", percent);
        
        pros::delay(50);
    }
}
```

#### 線追蹤感測器範例
```cpp
void opcontrol() {
    pros::adi::AnalogIn line_left('A');
    pros::adi::AnalogIn line_center('B');
    pros::adi::AnalogIn line_right('C');
    
    // 校準 (在白色表面上)
    printf("校準中 (放在白色表面)...\n");
    pros::delay(500);
    
    int white_left = line_left.calibrate();
    int white_center = line_center.calibrate();
    int white_right = line_right.calibrate();
    
    printf("校準完成\n");
    
    while (true) {
        int left = line_left.get_value_calibrated();
        int center = line_center.get_value_calibrated();
        int right = line_right.get_value_calibrated();
        
        printf("線感測器: L=%d C=%d R=%d\n", left, center, right);
        
        // 檢測黑線 (校準值會很負)
        if (abs(left) > 500) {
            printf("左側偵測到線\n");
        }
        if (abs(center) > 500) {
            printf("中央偵測到線\n");
        }
        if (abs(right) > 500) {
            printf("右側偵測到線\n");
        }
        
        pros::delay(20);
    }
}
```

---

## 🔊 AnalogOut - 模擬輸出

### 🎯 創建 AnalogOut

```cpp
pros::adi::AnalogOut output(std::uint8_t adi_port);
pros::adi::AnalogOut output(ext_adi_port_pair_t port_pair);
```

---

### 📊 AnalogOut 方法

#### `set_value()` - 設置輸出
```cpp
std::int32_t set_value(std::int32_t value);
```

設置模擬輸出電壓。

**參數**:
- `value`: 0-4095
  - `0`: 0V
  - `4095`: 5V

**範例**:
```cpp
pros::adi::AnalogOut output('A');

output.set_value(0);     // 0V
output.set_value(2048);  // 2.5V
output.set_value(4095);  // 5V

// 設置特定電壓
double target_voltage = 3.3;
int value = (target_voltage / 5.0) * 4095;
output.set_value(value);
```

---

## 💡 DigitalOut - 數位輸出

### 🎯 創建 DigitalOut

```cpp
pros::adi::DigitalOut output(std::uint8_t adi_port, bool init_state = LOW);
pros::adi::DigitalOut output(ext_adi_port_pair_t port_pair, bool init_state = LOW);
```

**參數**:
- `init_state`: 初始狀態
  - `LOW` 或 `false`: 低電平 (0V)
  - `HIGH` 或 `true`: 高電平 (5V)

**範例**:
```cpp
// 初始為 LOW
pros::adi::DigitalOut led('A', LOW);

// 初始為 HIGH
pros::adi::DigitalOut solenoid('B', HIGH);
```

---

### 📊 DigitalOut 方法

#### `set_value()` - 設置輸出
```cpp
std::int32_t set_value(std::int32_t value);
```

設置數位輸出狀態。

**參數**:
- `LOW` / `0` / `false`: 低電平
- `HIGH` / `1` / `true`: 高電平

**範例**:
```cpp
pros::adi::DigitalOut led('A');

// 打開
led.set_value(HIGH);
led.set_value(1);
led.set_value(true);

// 關閉
led.set_value(LOW);
led.set_value(0);
led.set_value(false);

// 閃爍
while (true) {
    led.set_value(HIGH);
    pros::delay(500);
    led.set_value(LOW);
    pros::delay(500);
}
```

---

### 💡 DigitalOut 使用範例

#### LED 控制範例
```cpp
void opcontrol() {
    pros::adi::DigitalOut led_red('A');
    pros::adi::DigitalOut led_green('B');
    pros::Controller controller(pros::E_CONTROLLER_MASTER);
    
    while (true) {
        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {
            led_red.set_value(HIGH);
            led_green.set_value(LOW);
        } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
            led_red.set_value(LOW);
            led_green.set_value(HIGH);
        } else {
            led_red.set_value(LOW);
            led_green.set_value(LOW);
        }
        
        pros::delay(20);
    }
}
```

#### 電磁閥控制範例
```cpp
void opcontrol() {
    pros::adi::DigitalOut solenoid('H');
    pros::Controller controller(pros::E_CONTROLLER_MASTER);
    
    bool extended = false;
    
    while (true) {
        // 按下 A 切換狀態
        if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
            extended = !extended;
            solenoid.set_value(extended);
            
            if (extended) {
                printf("電磁閥: 伸出\n");
            } else {
                printf("電磁閥: 縮回\n");
            }
        }
        
        pros::delay(20);
    }
}
```

---

## 🔍 DigitalIn - 數位輸入

### 🎯 創建 DigitalIn

```cpp
pros::adi::DigitalIn input(std::uint8_t adi_port);
pros::adi::DigitalIn input(ext_adi_port_pair_t port_pair);
```

**範例**:
```cpp
pros::adi::DigitalIn limit_switch('A');
pros::adi::DigitalIn bumper_switch('B');
```

---

### 📊 DigitalIn 方法

#### 1. `get_value()` - 獲取值
```cpp
std::int32_t get_value();
```

讀取數位輸入狀態。

**返回值**:
- `HIGH` / `1`: 高電平
- `LOW` / `0`: 低電平

**範例**:
```cpp
pros::adi::DigitalIn button('A');

if (button.get_value() == HIGH) {
    printf("按鈕按下\n");
} else {
    printf("按鈕釋放\n");
}
```

---

#### 2. `get_new_press()` - 檢測上升沿
```cpp
std::int32_t get_new_press();
```

檢測按鈕從 LOW 到 HIGH 的變化 (單次按下)。

**返回值**:
- `1`: 檢測到上升沿
- `0`: 無變化

**注意**: 此函數不是線程安全的

**範例**:
```cpp
pros::adi::DigitalIn button('A');

while (true) {
    if (button.get_new_press()) {
        printf("按鈕剛被按下!\n");
        // 執行動作
    }
    
    pros::delay(20);
}
```

---

### 💡 DigitalIn 使用範例

#### 限位開關範例
```cpp
void autonomous() {
    pros::adi::DigitalIn limit_switch('A');
    pros::Motor intake(5);
    
    // 運行進球機構直到觸發限位開關
    intake.move(127);
    
    while (limit_switch.get_value() == LOW) {
        pros::delay(10);
    }
    
    // 限位開關被觸發,停止
    intake.move(0);
    printf("球已進入,限位開關觸發\n");
}
```

#### 碰撞開關範例
```cpp
void opcontrol() {
    pros::adi::DigitalIn bumper_front('A');
    pros::adi::DigitalIn bumper_back('B');
    pros::Motor left_motor(1);
    pros::Motor right_motor(2);
    
    while (true) {
        // 檢測前方碰撞
        if (bumper_front.get_value() == HIGH) {
            printf("前方碰撞!\n");
            left_motor.move(-50);   // 後退
            right_motor.move(-50);
            pros::delay(500);
            left_motor.move(0);
            right_motor.move(0);
        }
        
        // 檢測後方碰撞
        if (bumper_back.get_value() == HIGH) {
            printf("後方碰撞!\n");
            left_motor.move(50);    // 前進
            right_motor.move(50);
            pros::delay(500);
            left_motor.move(0);
            right_motor.move(0);
        }
        
        pros::delay(20);
    }
}
```

#### 計數器範例
```cpp
void opcontrol() {
    pros::adi::DigitalIn counter_sensor('A');
    int count = 0;
    
    while (true) {
        // 每次按下增加計數
        if (counter_sensor.get_new_press()) {
            count++;
            printf("計數: %d\n", count);
            pros::lcd::print(0, "Count: %d", count);
        }
        
        pros::delay(20);
    }
}
```

---

## 🖥️ LCD (LLEMU) API

> 參考：PROS 官方文件 <https://pros.cs.purdue.edu/v5/api/cpp/llemu.html>

### 📖 概述
`pros::lcd` 提供三按鈕 Legacy LCD 的模擬器介面，可在 Brain 螢幕顯示 8 行文字並讀取左右/中觸控按鈕。PROS 會在 `initialize()` 後建立任務處理輸出，因此在 autonomous 或 opcontrol 中顯示除錯資訊非常便利。

### 🔧 初始化與狀態

| 函式 | 說明 |
|------|------|
| `bool pros::lcd::initialize()` | 啟動 LCD（回傳 `true` 代表成功）。在 LVGL 模板中會載入 GUI 元件，其餘專案則使用文字介面。 |
| `bool pros::lcd::is_initialized()` | 查詢是否已初始化。官方建議在顯示前確認此狀態，以避免 `ENXIO` 錯誤。 |
| `void pros::lcd::shutdown()`* | 關閉 LCD 任務並釋放資源；僅 LVGL 版本提供。 |

```cpp
#include "pros/llemu.hpp"

if (!pros::lcd::is_initialized()) {
    if (!pros::lcd::initialize()) {
        pros::lcd::print(0, "LCD init failed");
    }
}
```

### ✏️ 文字輸出 API

| 函式 | 作用 | 備註 |
|------|------|------|
| `bool set_text(std::int16_t line, std::string text)` | 將整行文字寫入 0–7 行 | `line` 超範圍會回傳 `false` 並設 `EINVAL` |
| `bool clear_line(std::int16_t line)` | 清除指定行 | |
| `bool clear()`* | 清空所有行 | 專用於 LVGL 版本 |
| `template <typename...> bool print(std::int16_t line, const char* fmt, Params... args)` | `printf` 風格輸出 | 會呼叫 `set_text` 實做；常用於顯示數值 |

```cpp
pros::lcd::set_text(0, "Auton Selector");
pros::lcd::print(1, "Pose: X=%.1f", to_in(odom.getPose().x));
pros::lcd::clear_line(7);
```

### 🎛️ 按鈕輸入

| 函式 | 說明 |
|------|------|
| `std::uint8_t read_buttons()` | 回傳位元遮罩，對應 `LCD_BTN_LEFT = 4`、`LCD_BTN_CENTER = 2`、`LCD_BTN_RIGHT = 1`；可同時多按。 |
| `void register_btn1_cb(lcd_btn_cb_fn_t cb)` | 註冊左鍵回呼（LVGL 模板還提供 `register_btn2_cb`/`register_btn3_cb`）。 |

```cpp
while (true) {
    auto buttons = pros::lcd::read_buttons();
    if (buttons & LCD_BTN_LEFT)  pros::lcd::print(2, "Left pressed");
    if (buttons & LCD_BTN_CENTER) pros::lcd::print(3, "Center pressed");
    pros::delay(50);
}
```

### 🧪 綜合範例

```cpp
void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(0, "LemLib Demo");

    pros::lcd::register_btn1_cb([] {
        pros::lcd::print(1, "Left callback fired");
    });
}
```

> \* `shutdown()`、`clear()`、`register_btn2_cb()`、`register_btn3_cb()` 只在安裝 LVGL 介面時可用；若使用純文字 stub，函式可能不存在或為空操作。請依專案模板確認可用性。

---

## 📚 快速參考表

### GPS 感測器

| 功能 | 指令 | 範例 |
|------|------|------|
| 創建 GPS | `Gps(port)` | `pros::Gps gps(1);` |
| 設置位置 | `set_position(x, y, heading)` | `gps.set_position(1.5, 1.5, 90);` |
| 設置偏移 | `set_offset(x, y)` | `gps.set_offset(0.1, 0);` |
| 獲取位置 | `get_position()` | `auto pos = gps.get_position();` |
| 獲取 X | `get_position_x()` | `double x = gps.get_position_x();` |
| 獲取 Y | `get_position_y()` | `double y = gps.get_position_y();` |
| 獲取誤差 | `get_error()` | `double err = gps.get_error();` |

### Distance 感測器

| 功能 | 指令 | 範例 |
|------|------|------|
| 創建 Distance | `Distance(port)` | `pros::Distance dist(10);` |
| 獲取距離 | `get()` 或 `get_distance()` | `int mm = dist.get();` |
| 獲取信心度 | `get_confidence()` | `int conf = dist.get_confidence();` |
| 獲取物體大小 | `get_object_size()` | `int size = dist.get_object_size();` |
| 獲取速度 | `get_object_velocity()` | `double vel = dist.get_object_velocity();` |

### Optical 感測器

| 功能 | 指令 | 範例 |
|------|------|------|
| 創建 Optical | `Optical(port)` | `pros::Optical opt(15);` |
| 獲取色相 | `get_hue()` | `double hue = opt.get_hue();` |
| 獲取飽和度 | `get_saturation()` | `double sat = opt.get_saturation();` |
| 獲取亮度 | `get_brightness()` | `double bright = opt.get_brightness();` |
| 獲取接近度 | `get_proximity()` | `int prox = opt.get_proximity();` |
| 獲取 RGB | `get_rgb()` | `auto rgb = opt.get_rgb();` |
| 設置 LED | `set_led_pwm(value)` | `opt.set_led_pwm(100);` |
| 啟用手勢 | `enable_gesture()` | `opt.enable_gesture();` |
| 獲取手勢 | `get_gesture()` | `auto g = opt.get_gesture();` |

### ADI 埠

| 類型 | 類別 | 範例 |
|------|------|------|
| 模擬輸入 | `adi::AnalogIn` | `pros::adi::AnalogIn pot('A');` |
| 模擬輸出 | `adi::AnalogOut` | `pros::adi::AnalogOut out('B');` |
| 數位輸入 | `adi::DigitalIn` | `pros::adi::DigitalIn btn('C');` |
| 數位輸出 | `adi::DigitalOut` | `pros::adi::DigitalOut led('D');` |

### ADI 常用方法

| 功能 | 指令 | 範例 |
|------|------|------|
| 讀取模擬值 | `get_value()` | `int val = sensor.get_value();` |
| 校準 | `calibrate()` | `sensor.calibrate();` |
| 讀取校準值 | `get_value_calibrated()` | `int cal = sensor.get_value_calibrated();` |
| 設置輸出 | `set_value(value)` | `output.set_value(HIGH);` |
| 檢測按下 | `get_new_press()` | `if (btn.get_new_press())` |

### LCD (LLEMU)

| 功能 | 指令 | 範例 |
|------|------|------|
| 初始化 | `lcd::initialize()` | `pros::lcd::initialize();` |
| 檢查狀態 | `lcd::is_initialized()` | `if (!pros::lcd::is_initialized()) ...` |
| 顯示文字 | `lcd::set_text(line, text)` | `pros::lcd::set_text(0, "Ready");` |
| 格式化輸出 | `lcd::print(line, fmt, ...)` | `pros::lcd::print(1, "X=%.1f", value);` |
| 清除行 | `lcd::clear_line(line)` | `pros::lcd::clear_line(7);` |
| 讀取按鈕 | `lcd::read_buttons()` | `auto btn = pros::lcd::read_buttons();` |
| 註冊按鈕回呼 | `lcd::register_btn1_cb(cb)` | `pros::lcd::register_btn1_cb(onLeft);` |

---

## 💡 最佳實踐

### GPS 使用建議
1. **啟動時校準**: GPS 需要時間初始化,建議在 `initialize()` 中等待
2. **設置偏移**: 準確測量 GPS 到機器人中心的距離
3. **檢查誤差**: 定期檢查 `get_error()`,誤差過大時警告
4. **數據更新率**: 根據需求調整,預設 20ms 已足夠

### Distance 使用建議
1. **檢查信心度**: 只在 `confidence > 40` 時使用數據
2. **處理 9999**: 這表示未檢測到物體,不是錯誤
3. **最小距離**: 感測器在 < 20mm 時可能不準確
4. **最大距離**: 超過 2000mm 信心度會降低

### Optical 使用建議
1. **環境光**: LED 亮度根據環境光調整 (30-100)
2. **距離**: 最佳檢測距離 5-50mm
3. **顏色 vs 手勢**: 不能同時使用,需要切換
4. **積分時間**: 較短的時間 (< 50ms) 提供更快更新,但可能不太穩定

### ADI 使用建議
1. **埠號**: 使用字母 ('A'-'H') 更清晰
2. **校準**: 陀螺儀、加速度計必須校準
3. **線程安全**: `get_new_press()` 不是線程安全的
4. **擴展器**: 使用 `{{smart_port, adi_port}}` 格式

### LCD 使用建議
1. **集中初始化**: 在 `initialize()` 中統一呼叫 `pros::lcd::initialize()`，並以 `is_initialized()` 確認狀態，避免在其他任務中發生 `ENXIO`。
2. **行號範圍**: `set_text()`/`print()` 的行號必須介於 0–7；建議將固定資訊與即時資料分層顯示以免覆蓋。
3. **按鈕去抖**: `read_buttons()` 回傳的是即時狀態，可結合 `pros::delay()` 或自行紀錄時間避免多次觸發。
4. **LVGL 差異**: 若專案採用 LVGL 模板，可使用 `shutdown()`、`clear()` 等額外函式；在純文字 stub 中這些函式可能無效，使用前請確認模板支援。

---

**文檔版本**: 1.0.0  
**最後更新**: 2025年1月8日  
**PROS 版本**: 4.x
