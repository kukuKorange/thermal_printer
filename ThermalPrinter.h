#ifndef __THERMAL_PRINTER_H__
#define __THERMAL_PRINTER_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// --- 枚举定义 ---
typedef enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
} PrinterAlign;

typedef enum {
    FNT_NORMAL = 0x00,
    FNT_DOUBLE_HEIGHT = 0x01,
    FNT_DOUBLE_WIDTH = 0x10,
    FNT_BIG = 0x11  // 倍高倍宽
} PrinterFontSize;

// --- 核心驱动接口 ---
// 注意：你需要根据你的硬件实现这个底层发送函数
extern void Printer_WriteBytes(const uint8_t* data, uint16_t len);

// --- 基础指令封装 ---
void Printer_Init(void);
void Printer_LineFeed(uint8_t lines);
void Printer_SetAlign(PrinterAlign align);
void Printer_SetBold(uint8_t enable);
void Printer_SetUnderline(uint8_t mode); // 0:关, 1:1点, 2:2点
void Printer_SetInvert(uint8_t enable);
void Printer_SetFontSize(PrinterFontSize size);

// --- Markdown 模拟打印 ---
void Printer_PrintMarkdown(const char* md_line);

// --- 图片打印 ---
void Printer_PrintBitmap(uint16_t width, uint16_t height, const uint8_t* bitmap);

// --- 厂商自定义配置 (RS# 指令) ---
void Printer_Cfg_SetDensity(uint8_t density); // 0-39
void Printer_Cfg_SetSpeed(uint8_t speed);     // 0-12
void Printer_Cfg_Reset(void);

#endif