flowchart TD
    A["ESP32-C6 khởi động"] --> B["Khởi tạo OLED, AHT30, INA226"]
    B --> C["Kiểm tra WiFi đã lưu"]
    C -->|Có WiFi| D["Kết nối vào mạng"]
    C -->|Chưa có WiFi| E["Phát WiFi cấu hình HE THONG GIAM SAT"]
    E --> F["Người dùng nhập WiFi và Google Apps Script ID"]
    F --> D
    D --> G["Đọc nhiệt độ, độ ẩm, điện áp, dòng điện"]
    G --> H["Hiển thị OLED và web"]
    H --> I["Gửi dữ liệu lên Google Sheet"]
    I --> G