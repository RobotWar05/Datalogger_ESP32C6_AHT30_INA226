<a id="top"></a>

# Tổng Quan Dự Án Data Logger ESP32-C6
LINK DATA Google Sheet: https://drive.google.com/drive/folders/1tRtYwRLFDgoCqyweKTJ5jtmrRs-Uq_rW?usp=sharing

## Mục Lục

1. [Dự án là gì](#du-an-la-gi)
2. [Linh kiện sử dụng](#linh-kien-su-dung)
3. [Sơ đồ chân nối](#so-do-chan-noi)
4. [Quy trình hoạt động](#quy-trinh-hoat-dong)
5. [Cấu hình WiFi và Google Sheet](#cau-hinh-wifi-va-google-sheet)
6. [Cấu trúc file code](#cau-truc-file-code)
7. [Cách nạp và kiểm tra](#cach-nap-va-kiem-tra)
8. [Tóm tắt nhanh](#tom-tat-nhanh)

---

<a id="du-an-la-gi"></a>

## 1. Dự án là gì

Dự án là một hệ thống data logger dùng ESP32-C6 để theo dõi:

- Nhiệt độ.
- Độ ẩm.
- Điện áp nguồn/pin.
- Dòng điện tiêu thụ.

Dữ liệu được hiển thị trên:

- OLED 0.91 inch.
- Trang web chạy trên ESP32-C6.
- Google Sheet qua Google Apps Script.

Mục tiêu chính: theo dõi môi trường và tình trạng nguồn từ xa, đồng thời lưu dữ liệu để xem lịch sử.

[Trở về đầu trang](#top)

---

<a id="linh-kien-su-dung"></a>

## 2. Linh kiện sử dụng

| Linh kiện | Vai trò |
|---|---|
| ESP32-C6 | Vi điều khiển trung tâm, chạy WiFi/web/server |
| AHT30 | Đo nhiệt độ và độ ẩm |
| OLED 0.91 SSD1306 | Hiển thị nhanh tại thiết bị |
| INA226 | Đo điện áp và dòng điện |
| Mạch hạ áp buck | Hạ nguồn pin xuống mức cấp cho ESP32-C6 |
| Google Sheet | Lưu dữ liệu lịch sử |
| Google Apps Script | Nhận dữ liệu từ ESP32-C6 và ghi vào Sheet |

[Trở về đầu trang](#top)

---

<a id="so-do-chan-noi"></a>

## 3. Sơ đồ chân nối

AHT30, OLED và INA226 dùng chung bus I2C:

| Thiết bị | Chân thiết bị | ESP32-C6 |
|---|---:|---:|
| AHT30 | VCC | 3V3 |
| AHT30 | GND | GND |
| AHT30 | SDA | GPIO6 |
| AHT30 | SCL | GPIO7 |
| OLED | VCC | 3V3 |
| OLED | GND | GND |
| OLED | SDA | GPIO6 |
| OLED | SCL | GPIO7 |
| INA226 | VCC | 3V3 |
| INA226 | GND | GND |
| INA226 | SDA | GPIO6 |
| INA226 | SCL | GPIO7 |

Đường đo nguồn bằng INA226:

```text
Pin+ -> INA226 IN+ -> INA226 IN- -> Buck VIN+ -> Buck OUT+ -> ESP32-C6 5V
Pin- -> Buck GND -> ESP32-C6 GND
INA226 VBUS -> nối chung với INA226 IN+
INA226 GND -> GND chung
```

Lưu ý: khi vừa cắm USB vừa cấp pin, cần tránh cấp đồng thời nhiều nguồn vào chân 5V nếu chưa chắc mạch nguồn cho phép.

[Trở về đầu trang](#top)

---

<a id="quy-trinh-hoat-dong"></a>

## 4. Quy trình hoạt động

Luồng chính:

```text
ESP32-C6 khởi động
-> Khởi tạo OLED, AHT30, INA226
-> Đọc WiFi đã lưu
-> Nếu có WiFi: kết nối vào mạng
-> Nếu chưa có WiFi: phát AP cấu hình
-> Đọc nhiệt độ, độ ẩm, điện áp, dòng điện
-> Hiển thị OLED
-> Cập nhật web
-> Gửi dữ liệu lên Google Sheet theo chu kỳ
```

Khi chạy bình thường:

- OLED hiển thị ngắn gọn nhiệt độ và độ ẩm.
- Web hiển thị đầy đủ nhiệt độ, độ ẩm, điện áp, dòng điện.
- Google Sheet lưu dữ liệu theo thời gian.

[Trở về đầu trang](#top)

---

<a id="cau-hinh-wifi-va-google-sheet"></a>

## 5. Cấu hình WiFi và Google Sheet

Khi chưa có WiFi lưu trong ESP32-C6:

1. ESP32-C6 phát WiFi tên `HE THONG GIAM SAT`.
2. Kết nối điện thoại/máy tính vào WiFi này.
3. Truy cập `192.168.4.1`.
4. Nhập WiFi, mật khẩu và Google Apps Script ID.
5. Lưu cấu hình, ESP32-C6 sẽ restart.

Khi muốn đổi WiFi:

- Vào web giám sát.
- Mở phần cài đặt.
- Bấm nút reset/đổi WiFi.
- ESP32-C6 xóa cấu hình cũ và quay lại chế độ cấu hình.

[Trở về đầu trang](#top)

---

<a id="cau-truc-file-code"></a>

## 6. Cấu trúc file code

Bản nên dùng để phát triển tiếp:

```text
E:\HeThongNhung\project\main\esp32c6\mainV2
```

Các file chính:

| File | Nhiệm vụ |
|---|---|
| `mainV2.ino` | Điểm bắt đầu chương trình, setup và loop |
| `GlobalConfig.h` | Cấu hình chân, OLED, INA226 và biến chung |
| `SensorManager.cpp` | Đọc AHT30 và INA226 |
| `DisplayManager.cpp` | Quản lý OLED |
| `WifiApp.cpp` | WiFi, web server, API cấu hình |
| `GoogleLogger.cpp` | Gửi dữ liệu lên Google Sheet |
| `DeviceManager.cpp` | Điều phối phần cứng và cập nhật định kỳ |
| `HtmlPages.cpp` | Giao diện web chính |
| `WifiConfigPage.cpp` | Trang cấu hình WiFi ban đầu |

File cần xem trước khi sửa:

- Lỗi cảm biến: `SensorManager.cpp`
- Lỗi OLED: `DisplayManager.cpp`
- Lỗi WiFi/web: `WifiApp.cpp`
- Lỗi Google Sheet: `GoogleLogger.cpp`
- Đổi chân: `GlobalConfig.h`

[Trở về đầu trang](#top)

---

<a id="cach-nap-va-kiem-tra"></a>

## 7. Cách nạp và kiểm tra

1. Mở Arduino IDE.
2. Mở file:

```text
E:\HeThongNhung\project\main\esp32c6\mainV2\mainV2.ino
```

3. Chọn board ESP32-C6.
4. Chọn đúng cổng COM.
5. Upload.
6. Mở Serial Monitor `115200`.

Log nên thấy:

```text
AHT30 OK
INA226 OK on shared I2C GPIO6/GPIO7
INA226 bus=...
GoogleSheet params temp=...
SEND OK: ...
```

[Trở về đầu trang](#top)

---

<a id="tom-tat-nhanh"></a>

## 8. Tóm tắt nhanh

Hệ thống dùng ESP32-C6 làm trung tâm. AHT30 đo nhiệt độ/độ ẩm, INA226 đo điện áp/dòng điện, OLED hiển thị nhanh tại thiết bị, web hiển thị đầy đủ dữ liệu và Google Sheet lưu lịch sử.

`mainV2` là bản nên dùng để tiếp tục phát triển vì code đã tách module rõ hơn, dễ đọc hơn và dễ sửa hơn.

[Trở về đầu trang](#top)

