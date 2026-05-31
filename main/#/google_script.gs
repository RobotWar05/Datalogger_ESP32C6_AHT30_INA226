function doGet(e) {
  var temp = toNumber_(e.parameter.temp);
  var hum = toNumber_(e.parameter.hum);

  var voltage = firstNumber_([
    e.parameter.volt,
    e.parameter.voltage,
    e.parameter.v,
    e.parameter.busVoltage
  ]);

  var current = firstNumber_([
    e.parameter.curr,
    e.parameter.current,
    e.parameter.i,
    e.parameter.amp
  ]);

  var high = toNumber_(e.parameter.high);
  var low = toNumber_(e.parameter.low);
  if (isNaN(high)) high = 32.0;
  if (isNaN(low)) low = 18.0;

  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  ensureHeader_(sheet);

  var now = new Date();
  var dateText = Utilities.formatDate(now, Session.getScriptTimeZone(), "dd/MM/yyyy");
  var timeText = Utilities.formatDate(now, Session.getScriptTimeZone(), "HH:mm:ss");
  var stt = sheet.getLastRow();

  sheet.appendRow([
    stt,
    dateText,
    timeText,
    roundOrBlank_(temp, 2),
    roundOrBlank_(hum, 2),
    roundOrBlank_(voltage, 2),
    roundOrBlank_(current, 3)
  ]);

  var row = sheet.getLastRow();

  resetRowFormat_(sheet, row);
  formatTemperatureCell_(sheet, row, temp, high, low);
  formatHumidityCell_(sheet, row, hum);
  formatVoltageCell_(sheet, row, voltage);
  formatCurrentCell_(sheet, row, current);

  return ContentService.createTextOutput("Success");
}

function ensureHeader_(sheet) {
  if (sheet.getLastRow() > 0) {
    sheet.getRange(1, 3).setValue("Thời gian");
    return;
  }

  sheet.appendRow([
    "STT",
    "Ngày tháng",
    "Thời gian",
    "Nhiệt độ (°C)",
    "Độ ẩm (%)",
    "Điện áp (V)",
    "Dòng điện (A)"
  ]);

  var header = sheet.getRange(1, 1, 1, 7);
  header.setFontWeight("bold");
  header.setBackground("#dbeafe");
  header.setFontColor("#1e3a8a");
  header.setHorizontalAlignment("center");
}

function resetRowFormat_(sheet, row) {
  var range = sheet.getRange(row, 1, 1, 7);

  range.setBackground("white");
  range.setFontColor("black");
  range.setFontWeight("bold");
  range.setFontStyle("normal");
  range.setHorizontalAlignment("center");
  range.setNotes([["", "", "", "", "", "", ""]]);

  sheet.getRange(row, 2, 1, 2).setFontStyle("italic");
}

function formatTemperatureCell_(sheet, row, temp, high, low) {
  var cell = sheet.getRange(row, 4);

  if (isNaN(temp)) {
    markCell_(cell, "#fee2e2", "#b91c1c", "Lỗi dữ liệu nhiệt độ");
  } else if (temp > high) {
    markCell_(cell, "#fecaca", "#b91c1c", "Quá nhiệt > " + high + "°C");
  } else if (temp < low) {
    markCell_(cell, "#bfdbfe", "#1e40af", "Nhiệt độ thấp < " + low + "°C");
  } else {
    markCell_(cell, "#dcfce7", "#15803d", "Nhiệt độ ổn định");
  }
}

function formatHumidityCell_(sheet, row, hum) {
  var cell = sheet.getRange(row, 5);

  if (isNaN(hum)) {
    markCell_(cell, "#fee2e2", "#b91c1c", "Lỗi dữ liệu độ ẩm");
  }
}

function formatVoltageCell_(sheet, row, voltage) {
  var cell = sheet.getRange(row, 6);

  if (isNaN(voltage)) {
    markCell_(cell, "#fee2e2", "#b91c1c", "Lỗi dữ liệu điện áp");
  } else if (voltage < 5.0) {
    markCell_(cell, "#fecaca", "#b91c1c", "Điện áp thấp < 5V");
  } else if (voltage >= 6.0 && voltage <= 8.4) {
    markCell_(cell, "#dcfce7", "#15803d", "Điện áp ổn định");
  } else {
    markCell_(cell, "#fef3c7", "#92400e", "Điện áp bình thường");
  }
}

function formatCurrentCell_(sheet, row, current) {
  var cell = sheet.getRange(row, 7);

  if (isNaN(current)) {
    markCell_(cell, "#fee2e2", "#b91c1c", "Lỗi dữ liệu dòng điện");
  }
}

function markCell_(cell, bgColor, textColor, note) {
  cell.setBackground(bgColor);
  cell.setFontColor(textColor);
  cell.setFontWeight("bold");
  cell.setNote(note || "");
}

function toNumber_(value) {
  if (value === undefined || value === null || value === "") return NaN;
  return parseFloat(String(value).replace(",", "."));
}

function firstNumber_(values) {
  for (var i = 0; i < values.length; i++) {
    var value = toNumber_(values[i]);
    if (!isNaN(value)) return value;
  }
  return NaN;
}

function roundOrBlank_(value, digits) {
  if (isNaN(value)) return "";
  var factor = Math.pow(10, digits);
  return Math.round(value * factor) / factor;
}