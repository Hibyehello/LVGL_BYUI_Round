/**
 * Display configuration for Waveshare ESP32-S3-Touch-LCD-2.8C
 * Based on official ESP-IDF demo code
 */

#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp32-hal-ledc.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

// Display resolution
#define LCD_WIDTH  480
#define LCD_HEIGHT 480

// SPI pins for ST7701S initialization
#define LCD_SPI_MOSI  1
#define LCD_SPI_CLK   2

// Backlight
#define LCD_BL_PIN    6

// I2C pins
#define I2C_SDA_PIN   15
#define I2C_SCL_PIN   7

// RGB Timing - reduced to 14MHz for stability
#define RGB_TIMING_FREQ_HZ    (14 * 1000 * 1000)  // 14MHz
#define RGB_TIMING_HPW        8
#define RGB_TIMING_HBP        10
#define RGB_TIMING_HFP        50
#define RGB_TIMING_VPW        2
#define RGB_TIMING_VBP        18
#define RGB_TIMING_VFP        8
#define RGB_PCLK_ACTIVE_NEG   0  // 0: rising edge

// RGB Data pins
#define RGB_DATA0     5   // B0
#define RGB_DATA1     45  // B1
#define RGB_DATA2     48  // B2
#define RGB_DATA3     47  // B3
#define RGB_DATA4     21  // B4
#define RGB_DATA5     14  // G0
#define RGB_DATA6     13  // G1
#define RGB_DATA7     12  // G2
#define RGB_DATA8     11  // G3
#define RGB_DATA9     10  // G4
#define RGB_DATA10    9   // G5
#define RGB_DATA11    46  // R0
#define RGB_DATA12    3   // R1
#define RGB_DATA13    8   // R2
#define RGB_DATA14    18  // R3
#define RGB_DATA15    17  // R4

// Control pins
#define RGB_HSYNC     38
#define RGB_VSYNC     39
#define RGB_DE        40
#define RGB_PCLK      41

// TCA9554 IO Expander
#define TCA9554_ADDR        0x20
#define TCA9554_INPUT_REG   0x00
#define TCA9554_OUTPUT_REG  0x01
#define TCA9554_CONFIG_REG  0x03

// EXIO pins (1-based like ESP-IDF demo: Pin 1-8)
#define EXIO_LCD_RST    1   // EXIO1 - LCD Reset
#define EXIO_TOUCH_RST  2   // EXIO2 - Touch Reset
#define EXIO_LCD_CS     3   // EXIO3 - LCD CS
#define EXIO_BUZZER     8   // EXIO8 - Buzzer

// GT911 Touch Controller
#define GT911_ADDR          0x5D
#define GT911_READ_XY_REG   0x814E
#define TOUCH_INT_PIN       16

// Global handles
static spi_device_handle_t spi_handle = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;

//=============================================================================
// TCA9554 IO Expander Functions
//=============================================================================

static uint8_t tca9554_output_state = 0x00;

void tca9554_write_reg(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t tca9554_read_reg(uint8_t reg) {
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(TCA9554_ADDR, (uint8_t)1);
    return Wire.read();
}

void tca9554_init() {
    // Set all pins as outputs
    tca9554_write_reg(TCA9554_CONFIG_REG, 0x00);
    // Initialize output state - all low
    tca9554_output_state = 0x00;
    tca9554_write_reg(TCA9554_OUTPUT_REG, tca9554_output_state);
    Serial.println("TCA9554 initialized");
}

void tca9554_set_pin(uint8_t pin, bool state) {
    // Read current output state first (like ESP-IDF demo)
    // Pin is 1-based (1-8), so we shift by (pin-1)
    uint8_t current = tca9554_read_reg(TCA9554_OUTPUT_REG);
    if (pin < 1 || pin > 8) {
        Serial.printf("TCA9554: Invalid pin %d\n", pin);
        return;
    }
    if (state) {
        current |= (1 << (pin - 1));
    } else {
        current &= ~(1 << (pin - 1));
    }
    tca9554_write_reg(TCA9554_OUTPUT_REG, current);
    tca9554_output_state = current;
}

// Convenience functions
void buzzer_off() {
    tca9554_set_pin(EXIO_BUZZER, false);
    Serial.println("Buzzer OFF");
}

void lcd_reset() {
    Serial.println("LCD Reset starting...");
    tca9554_set_pin(EXIO_LCD_RST, false);  // Reset LOW
    delay(10);
    tca9554_set_pin(EXIO_LCD_RST, true);   // Reset HIGH
    delay(120);  // Same as ESP-IDF demo
    Serial.println("LCD Reset done");
}

void lcd_cs_enable() {
    tca9554_set_pin(EXIO_LCD_CS, false);
    delay(10);
}

void lcd_cs_disable() {
    tca9554_set_pin(EXIO_LCD_CS, true);
    delay(10);
}

//=============================================================================
// ST7701S SPI Communication
//=============================================================================

void ST7701_WriteCommand(uint8_t cmd) {
    spi_transaction_t t = {};
    t.cmd = 0;      // Command bit = 0
    t.addr = cmd;
    t.length = 0;
    spi_device_transmit(spi_handle, &t);
}

void ST7701_WriteData(uint8_t data) {
    spi_transaction_t t = {};
    t.cmd = 1;      // Command bit = 1 (data)
    t.addr = data;
    t.length = 0;
    spi_device_transmit(spi_handle, &t);
}

void init_spi_for_st7701() {
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = LCD_SPI_MOSI;
    buscfg.miso_io_num = -1;
    buscfg.sclk_io_num = LCD_SPI_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {};
    devcfg.command_bits = 1;
    devcfg.address_bits = 8;
    devcfg.mode = 0;
    devcfg.clock_speed_hz = 4000000;  // 4MHz for init
    devcfg.spics_io_num = -1;  // CS via IO expander
    devcfg.queue_size = 1;

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle));
    Serial.println("SPI initialized for ST7701S");
}

//=============================================================================
// ST7701S Initialization Sequence (from ESP-IDF demo)
//=============================================================================

void st7701s_init_sequence() {
    // 2.8inch initialization sequence
    Serial.println("Beginning ST7701 init...");
    ST7701_WriteCommand(0xFF);
    ST7701_WriteData(0x77);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x13);

    ST7701_WriteCommand(0xEF);
    ST7701_WriteData(0x08);

    ST7701_WriteCommand(0xFF);
    ST7701_WriteData(0x77);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x10);

    ST7701_WriteCommand(0xC0);
    ST7701_WriteData(0x3B);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0xC1);
    ST7701_WriteData(0x10);
    ST7701_WriteData(0x0C);

    ST7701_WriteCommand(0xC2);
    ST7701_WriteData(0x07);
    ST7701_WriteData(0x0A);

    ST7701_WriteCommand(0xC7);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0xCC);
    ST7701_WriteData(0x10);

    ST7701_WriteCommand(0xCD);
    ST7701_WriteData(0x08);

    ST7701_WriteCommand(0xB0);
    ST7701_WriteData(0x05);
    ST7701_WriteData(0x12);
    ST7701_WriteData(0x98);
    ST7701_WriteData(0x0E);
    ST7701_WriteData(0x0F);
    ST7701_WriteData(0x07);
    ST7701_WriteData(0x07);
    ST7701_WriteData(0x09);
    ST7701_WriteData(0x09);
    ST7701_WriteData(0x23);
    ST7701_WriteData(0x05);
    ST7701_WriteData(0x52);
    ST7701_WriteData(0x0F);
    ST7701_WriteData(0x67);
    ST7701_WriteData(0x2C);
    ST7701_WriteData(0x11);

    ST7701_WriteCommand(0xB1);
    ST7701_WriteData(0x0B);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x97);
    ST7701_WriteData(0x0C);
    ST7701_WriteData(0x12);
    ST7701_WriteData(0x06);
    ST7701_WriteData(0x06);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x22);
    ST7701_WriteData(0x03);
    ST7701_WriteData(0x51);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x66);
    ST7701_WriteData(0x2B);
    ST7701_WriteData(0x0F);

    ST7701_WriteCommand(0xFF);
    ST7701_WriteData(0x77);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x11);

    ST7701_WriteCommand(0xB0);
    ST7701_WriteData(0x5D);

    ST7701_WriteCommand(0xB1);
    ST7701_WriteData(0x3E);

    ST7701_WriteCommand(0xB2);
    ST7701_WriteData(0x81);

    ST7701_WriteCommand(0xB3);
    ST7701_WriteData(0x80);

    ST7701_WriteCommand(0xB5);
    ST7701_WriteData(0x4E);

    ST7701_WriteCommand(0xB7);
    ST7701_WriteData(0x85);

    ST7701_WriteCommand(0xB8);
    ST7701_WriteData(0x20);

    ST7701_WriteCommand(0xC1);
    ST7701_WriteData(0x78);

    ST7701_WriteCommand(0xC2);
    ST7701_WriteData(0x78);

    ST7701_WriteCommand(0xD0);
    ST7701_WriteData(0x88);

    ST7701_WriteCommand(0xE0);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x02);

    ST7701_WriteCommand(0xE1);
    ST7701_WriteData(0x06);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0x05);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0x07);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x33);
    ST7701_WriteData(0x33);

    ST7701_WriteCommand(0xE2);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x33);
    ST7701_WriteData(0x33);
    ST7701_WriteData(0xF4);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0xF4);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0xE3);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x11);

    ST7701_WriteCommand(0xE4);
    ST7701_WriteData(0x44);
    ST7701_WriteData(0x44);

    ST7701_WriteCommand(0xE5);
    ST7701_WriteData(0x0D);
    ST7701_WriteData(0xF5);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x0F);
    ST7701_WriteData(0xF7);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x09);
    ST7701_WriteData(0xF1);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x0B);
    ST7701_WriteData(0xF3);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);

    ST7701_WriteCommand(0xE6);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x11);
    ST7701_WriteData(0x11);

    ST7701_WriteCommand(0xE7);
    ST7701_WriteData(0x44);
    ST7701_WriteData(0x44);

    ST7701_WriteCommand(0xE8);
    ST7701_WriteData(0x0C);
    ST7701_WriteData(0xF4);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x0E);
    ST7701_WriteData(0xF6);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);
    ST7701_WriteData(0x0A);
    ST7701_WriteData(0xF2);
    ST7701_WriteData(0x30);
    ST7701_WriteData(0xF0);

    ST7701_WriteCommand(0xE9);
    ST7701_WriteData(0x36);
    ST7701_WriteData(0x01);

    ST7701_WriteCommand(0xEB);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0xE4);
    ST7701_WriteData(0xE4);
    ST7701_WriteData(0x44);
    ST7701_WriteData(0x88);
    ST7701_WriteData(0x40);

    ST7701_WriteCommand(0xED);
    ST7701_WriteData(0xFF);
    ST7701_WriteData(0x10);
    ST7701_WriteData(0xAF);
    ST7701_WriteData(0x76);
    ST7701_WriteData(0x54);
    ST7701_WriteData(0x2B);
    ST7701_WriteData(0xCF);
    ST7701_WriteData(0xFF);
    ST7701_WriteData(0xFF);
    ST7701_WriteData(0xFC);
    ST7701_WriteData(0xB2);
    ST7701_WriteData(0x45);
    ST7701_WriteData(0x67);
    ST7701_WriteData(0xFA);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0xFF);

    ST7701_WriteCommand(0xEF);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x08);
    ST7701_WriteData(0x45);
    ST7701_WriteData(0x3F);
    ST7701_WriteData(0x54);

    ST7701_WriteCommand(0xFF);
    ST7701_WriteData(0x77);
    ST7701_WriteData(0x01);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0x11);  // Sleep out
    delay(120);

    ST7701_WriteCommand(0x3A);
    ST7701_WriteData(0x66);  // 18-bit color (RGB666)

    ST7701_WriteCommand(0x36);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0x35);
    ST7701_WriteData(0x00);

    ST7701_WriteCommand(0x29);  // Display on

    Serial.println("ST7701S init sequence complete");
}

//=============================================================================
// RGB Panel Initialization
//=============================================================================

void init_rgb_panel() {
    esp_lcd_rgb_panel_config_t rgb_config = {};

    rgb_config.clk_src = LCD_CLK_SRC_PLL160M;  // Use PLL160M

    rgb_config.timings.pclk_hz = RGB_TIMING_FREQ_HZ;
    rgb_config.timings.h_res = LCD_WIDTH;
    rgb_config.timings.v_res = LCD_HEIGHT;
    rgb_config.timings.hsync_pulse_width = RGB_TIMING_HPW;
    rgb_config.timings.hsync_back_porch = RGB_TIMING_HBP;
    rgb_config.timings.hsync_front_porch = RGB_TIMING_HFP;
    rgb_config.timings.vsync_pulse_width = RGB_TIMING_VPW;
    rgb_config.timings.vsync_back_porch = RGB_TIMING_VBP;
    rgb_config.timings.vsync_front_porch = RGB_TIMING_VFP;
    rgb_config.timings.flags.pclk_active_neg = RGB_PCLK_ACTIVE_NEG;

    rgb_config.data_width = 16;
    rgb_config.psram_trans_align = 64;
    rgb_config.sram_trans_align = 4;

    rgb_config.hsync_gpio_num = RGB_HSYNC;
    rgb_config.vsync_gpio_num = RGB_VSYNC;
    rgb_config.de_gpio_num = RGB_DE;
    rgb_config.pclk_gpio_num = RGB_PCLK;
    rgb_config.disp_gpio_num = -1;

    rgb_config.data_gpio_nums[0] = RGB_DATA0;
    rgb_config.data_gpio_nums[1] = RGB_DATA1;
    rgb_config.data_gpio_nums[2] = RGB_DATA2;
    rgb_config.data_gpio_nums[3] = RGB_DATA3;
    rgb_config.data_gpio_nums[4] = RGB_DATA4;
    rgb_config.data_gpio_nums[5] = RGB_DATA5;
    rgb_config.data_gpio_nums[6] = RGB_DATA6;
    rgb_config.data_gpio_nums[7] = RGB_DATA7;
    rgb_config.data_gpio_nums[8] = RGB_DATA8;
    rgb_config.data_gpio_nums[9] = RGB_DATA9;
    rgb_config.data_gpio_nums[10] = RGB_DATA10;
    rgb_config.data_gpio_nums[11] = RGB_DATA11;
    rgb_config.data_gpio_nums[12] = RGB_DATA12;
    rgb_config.data_gpio_nums[13] = RGB_DATA13;
    rgb_config.data_gpio_nums[14] = RGB_DATA14;
    rgb_config.data_gpio_nums[15] = RGB_DATA15;

    rgb_config.flags.fb_in_psram = true;
    rgb_config.num_fbs = 1;

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&rgb_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    Serial.println("RGB panel initialized");
}

//=============================================================================
// Backlight (5kHz as in ESP-IDF demo)
//=============================================================================

void backlight_init() {
    // Use 10-bit resolution at 5kHz (works on ESP32-S3) 
    ledcAttachChannel(LCD_BL_PIN, 5000, 10, 0); // Channel 0, 5kHz, 10-bit
    ledcWrite(LCD_BL_PIN, 716);  // ~70% brightness (1023 max for 10-bit)
    Serial.println("Backlight initialized");
}

// void backlight_init() {
//     pinMode(LCD_BL_PIN, OUTPUT);
//     digitalWrite(LCD_BL_PIN, HIGH); // Force backlight on 100%
//     Serial.println("Backlight forced HIGH");
// }

//=============================================================================
// Main Display Initialization
//=============================================================================

void init_display_rgb() {
    Serial.println("\n=== Waveshare Display Initialization (ESP-IDF method) ===");

    delay(500);

    // Initialize I2C for TCA9554
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);
    delay(50);

    // Initialize TCA9554 IO expander
    tca9554_init();

    // IMPORTANT: Turn off buzzer first!
    buzzer_off();

    lcd_cs_disable(); 
    delay(20);

    // Reset LCD (same as ESP-IDF demo)
    lcd_reset();

    // Initialize SPI for ST7701S
    init_spi_for_st7701();

    // Enable CS BEFORE SPI init (same as ESP-IDF demo)
    lcd_cs_enable();
    delay(100);

    // Run ST7701S init sequence (CS stays enabled)
    st7701s_init_sequence();

    // Initialize RGB panel (CS still enabled)
    init_rgb_panel();

    // Disable CS AFTER RGB panel init (same as ESP-IDF demo)
    lcd_cs_disable();

    // Initialize backlight
    backlight_init();

    Serial.println("=== Display initialization complete ===\n");
}

//=============================================================================
// GT911 Touch Controller
//=============================================================================

void gt911_write_reg(uint16_t reg, uint8_t data) {
    Wire.beginTransmission(GT911_ADDR);
    Wire.write((reg >> 8) & 0xFF);
    Wire.write(reg & 0xFF);
    Wire.write(data);
    Wire.endTransmission();
}

bool gt911_read_reg(uint16_t reg, uint8_t *data, uint8_t len) {
    Wire.beginTransmission(GT911_ADDR);
    Wire.write((reg >> 8) & 0xFF);
    Wire.write(reg & 0xFF);
    
    // Use standard STOP condition to prevent Error 259 bus lockups on NACK
    if (Wire.endTransmission() != 0) { 
        return false;
    }
    
    uint8_t received = Wire.requestFrom((int)GT911_ADDR, (int)len);
    if (received != len) {
        return false;
    }
    
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        data[i] = Wire.read();
    }
    return true;
}

void touch_reset() {
    // Set INT as output
    pinMode(TOUCH_INT_PIN, OUTPUT);
    
    // Pull both INT and RST LOW to start reset
    digitalWrite(TOUCH_INT_PIN, LOW);
    tca9554_set_pin(EXIO_TOUCH_RST, false);
    delay(15); 
    
    // Set INT HIGH *before* bringing RST HIGH to select address 0x14
    digitalWrite(TOUCH_INT_PIN, HIGH);
    delay(2);
    
    // Bring RST HIGH to lock in the address
    tca9554_set_pin(EXIO_TOUCH_RST, true);
    delay(50);

    // Set INT back to input for touch interrupts
    pinMode(TOUCH_INT_PIN, INPUT);

    Serial.println("Touch reset done");
}

void init_touch() {
    touch_reset();

    // Read product ID to verify touch is working
    uint8_t buf[4] = {0};
    if (gt911_read_reg(0x8140, buf, 3)) {
        Serial.printf("Touch ID: %c%c%c\n", buf[0], buf[1], buf[2]);
    } else {
        Serial.println("Touch read failed!");
    }
}

// Read touch coordinates - returns true if touched
bool read_touch(uint16_t *x, uint16_t *y) {
    uint8_t status = 0;
    if (!gt911_read_reg(GT911_READ_XY_REG, &status, 1)) {
        return false;
    }

    // Check if touch data is ready (bit 7) and at least one touch point
    uint8_t touch_count = status & 0x0F;
    if ((status & 0x80) == 0 || touch_count == 0 || touch_count > 5) {
        // Clear status register
        gt911_write_reg(GT911_READ_XY_REG, 0);
        return false;
    }

    // Read first touch point (8 bytes per point, starting at 0x814F)
    uint8_t data[8];
    if (!gt911_read_reg(GT911_READ_XY_REG + 1, data, 8)) {
        gt911_write_reg(GT911_READ_XY_REG, 0);
        return false;
    }

    // Clear status register
    gt911_write_reg(GT911_READ_XY_REG, 0);

    // Parse coordinates (little-endian)
    *x = (uint16_t)data[1] << 8 | data[0];
    *y = (uint16_t)data[3] << 8 | data[2];

    return true;
}

#endif // DISPLAY_CONFIG_H
