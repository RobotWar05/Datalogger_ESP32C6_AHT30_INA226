# So Sánh Nhanh `main` Và `mainV2`
Hai bản:

- `main`: bản gốc đã chạy ổn.
- `mainV2`: bản đã tách module để dễ phát triển hơn.

## 1. Bảng so sánh:

| Tiêu chí | `main` | `mainV2` | Nhận xét |
|---|---|---|---|
| Cấu trúc code | Gộp nhiều chức năng | Tách module rõ hơn | `mainV2` tốt hơn |
| File chính | `main.ino` | `mainV2.ino` | Cả hai đều đúng |
| Đọc AHT30/INA226 | Trong `DeviceManager` | Trong `SensorManager` | `mainV2` dễ debug hơn |
| OLED | Nằm rải trong `DeviceManager` và `WifiApp` | Gom vào `DisplayManager` | `mainV2` dễ sửa hơn |
| Google Sheet | Nằm trong `WifiApp` | Tách sang `GoogleLogger` | `mainV2` rõ hơn |
| Vòng `loop()` | Gửi Google trước rồi mới update sensor | Update sensor trước rồi mới gửi Google | `mainV2` hợp lý hơn |
| Dữ liệu lỗi INA226 | Có thể giữ số cũ | Đưa về `NAN` | `mainV2` an toàn hơn |
| Dễ mở rộng | Trung bình | Tốt | `mainV2` phù hợp lâu dài |
| Dễ hiểu với người mới | Dễ hơn vì ít file | Cần đọc theo module | `main` dễ mở nhanh hơn |
| Phù hợp xuất bản | Trung bình | Tốt | Nên chọn `mainV2` |

---

## 2. Khác biệt quan trọng nhất

### `main`

Các phần chính:

- `DeviceManager.cpp`: đọc cảm biến, đọc INA226, OLED, relay.
- `WifiApp.cpp`: WiFi, web server, reset, Google Sheet.

Ưu điểm:

- Ít file.
- Dễ nạp thử.
- Dễ xem nhanh toàn bộ dự án.

Nhược điểm:

- Một file ôm nhiều việc.
- Khó bảo trì khi thêm chức năng.
- Google Sheet nằm chung với WiFi/web nên chưa sạch kiến trúc.

### `mainV2`

Các phần chính:

- `SensorManager.cpp`: AHT30 và INA226.
- `DisplayManager.cpp`: OLED.
- `GoogleLogger.cpp`: Google Sheet.
- `WifiApp.cpp`: WiFi và web server.
- `DeviceManager.cpp`: điều phối phần cứng.

Ưu điểm:

- Chia chức năng rõ.
- Dễ tìm lỗi theo từng nhóm.
- Dễ giải thích trong báo cáo.
- Dễ phát triển tiếp.

Nhược điểm:

- Nhiều file hơn.
- Người mới cần đọc README để hiểu luồng.

---

## 3. Đánh giá theo mục đích sử dụng

| Mục đích | Nên dùng |
|---|---|
| Test nhanh bản cũ | `main` |
| Làm báo cáo/môn học | `mainV2` |
| Xuất bản code sạch hơn | `mainV2` |
| Mở rộng thêm relay/cảnh báo/pin % | `mainV2` |
| Backup logic cũ | `main` |

