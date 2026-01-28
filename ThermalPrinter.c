#include "ThermalPrinter.h"

// --- 底层指令定义 ---
#define ESC 0x1B
#define GS  0x1D
#define FS  0x1C

/**
 * @brief 初始化打印机
 */
void Printer_Init(void) {
    uint8_t cmd[] = {ESC, '@'};
    Printer_WriteBytes(cmd, 2);
}

/**
 * @brief 打印并换行
 * @param lines 换行行数
 */
void Printer_LineFeed(uint8_t lines) {
    for(uint8_t i=0; i<lines; i++) {
        uint8_t cmd = 0x0A;
        Printer_WriteBytes(&cmd, 1);
    }
}

/**
 * @brief 设置对齐方式
 */
void Printer_SetAlign(PrinterAlign align) {
    uint8_t cmd[] = {ESC, 'a', (uint8_t)align};
    Printer_WriteBytes(cmd, 3);
}

/**
 * @brief 字体大小控制
 * @param size 使用 PrinterFontSize 枚举
 */
void Printer_SetFontSize(PrinterFontSize size) {
    uint8_t cmd[] = {GS, '!', (uint8_t)size};
    Printer_WriteBytes(cmd, 3);
}

/**
 * @brief 加粗控制
 */
void Printer_SetBold(uint8_t enable) {
    uint8_t cmd[] = {ESC, 'E', enable ? 1 : 0};
    Printer_WriteBytes(cmd, 3);
}

/**
 * @brief 黑白反显打印 (白字黑底)
 */
void Printer_SetInvert(uint8_t enable) {
    uint8_t cmd[] = {GS, 'B', enable ? 1 : 0};
    Printer_WriteBytes(cmd, 3);
}

/**
 * @brief 打印位图 (光栅位图模式 GS v 0)
 * @param width 像素宽度 (需为8的倍数最佳)
 * @param height 像素高度
 * @param bitmap 1位深点阵数据
 */
void Printer_PrintBitmap(uint16_t width, uint16_t height, const uint8_t* bitmap) {
    uint16_t xL = (width + 7) / 8; // 水平方向字节数
    uint16_t xBytes = xL;
    
    uint8_t header[8] = {GS, 'v', '0', 0, 
                         xBytes & 0xFF, (xBytes >> 8) & 0xFF, 
                         height & 0xFF, (height >> 8) & 0xFF};
    
    Printer_WriteBytes(header, 8);
    Printer_WriteBytes(bitmap, xBytes * height);
}

/**
 * @brief 简单的 Markdown 解析打印
 * 支持: # 标题, --- 分割线, > 引用(反显模拟)
 */
void Printer_PrintMarkdown(const char* md_line) {
    // 1. 处理大标题 #
    if (md_line[0] == '#') {
        Printer_SetAlign(ALIGN_CENTER);
        Printer_SetFontSize(FNT_BIG);
        Printer_SetBold(1);
        // 过滤掉 # 号发送文本
        const char* text = md_line;
        while(*text == '#' || *text == ' ') text++;
        Printer_WriteBytes((uint8_t*)text, strlen(text));
        Printer_LineFeed(1);
        // 还原设置
        Printer_SetFontSize(FNT_NORMAL);
        Printer_SetBold(0);
        Printer_SetAlign(ALIGN_LEFT);
    } 
    // 2. 处理分割线 ---
    else if (strncmp(md_line, "---", 3) == 0) {
        Printer_WriteBytes((uint8_t*)"--------------------------------", 32);
        Printer_LineFeed(1);
    }
    // 3. 处理引用块 > (用反显模拟)
    else if (md_line[0] == '>') {
        Printer_SetInvert(1);
        Printer_WriteBytes((uint8_t*)(md_line + 1), strlen(md_line + 1));
        Printer_SetInvert(0);
        Printer_LineFeed(1);
    }
    // 4. 普通文本
    else {
        Printer_WriteBytes((uint8_t*)md_line, strlen(md_line));
        Printer_LineFeed(1);
    }
}

// --- 自定义 RS# 指令封装 ---

void Printer_Cfg_SetDensity(uint8_t density) {
    char buf[32];
    // 根据文档 P46 规则：起始符 RS# + 命令 PTDP + 控制符 = + 数值 + 结束符 ;
    int len = sprintf(buf, "RS#PTDP=%d;", density);
    Printer_WriteBytes((uint8_t*)buf, len);
}

void Printer_Cfg_Reset(void) {
    const char* cmd = "RS#RTFA*;"; // 恢复出厂设置
    Printer_WriteBytes((uint8_t*)cmd, strlen(cmd));
}