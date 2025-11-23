# 中国象棋棋谱管理系统 - 技术文档

## 项目概述

本项目实现了一个中国象棋棋谱管理系统，包含棋谱记录、记谱法解析、数据存储功能。

## 项目结构

groupchinesechess/
├── chess_database.h # 数据结构定义
├── chess_database.c # 数据库功能实现
├── move_parser.h # 记谱法解析头文件
├── move_parser.c # 记谱法解析实现
└── test_database.c # 功能测试代码

text

## 核心模块说明

### 1. 棋谱数据库模块

#### 数据结构

```c
// 棋子类型枚举
typedef enum {
    PIECE_KING,     // 帅/将
    PIECE_ADVISOR,  // 士/仕
    PIECE_ELEPHANT, // 相/象
    PIECE_HORSE,    // 马
    PIECE_CHARIOT,  // 车
    PIECE_CANNON,   // 炮
    PIECE_PAWN      // 兵/卒
} PieceType;

// 棋步记录
typedef struct {
    int step_number;        // 步数编号
    PlayerColor player;     // 玩家阵营
    PieceType piece_type;   // 棋子类型
    char notation[10];      // 记谱法字符串
    int from_x, from_y;     // 起始坐标
    int to_x, to_y;         // 目标坐标
    char time_str[20];      // 时间戳
} ChessMove;
核心功能函数
init_game_record() - 初始化新棋局

add_move_to_game() - 添加棋步记录

save_game_to_file() - 保存棋局到文件

print_game_summary() - 打印棋局摘要

2. 记谱法解析模块
解析结果结构
c
typedef struct {
    int valid;              // 解析是否成功
    PieceType piece_type;   // 棋子类型
    int from_x, from_y;     // 起始位置
    int to_x, to_y;         // 目标位置
    char error_msg[50];     // 错误信息
} ParseResult;

使用示例
基本用法
c
#include "chess_database.h"
#include "move_parser.h"

int main() {
    // 初始化棋局
    GameRecord game;
    init_game_record(&game, "G20231120001", "红方玩家", "黑方玩家");
    
    // 解析记谱法并记录棋步
    ParseResult result = parse_notation("兵五进一", PLAYER_RED, &game);
    if (result.valid) {
        ChessMove move;
        init_chess_move(&move, 1, PLAYER_RED, result.piece_type, 
                       "兵五进一", result.from_x, result.from_y, 
                       result.to_x, result.to_y);
        add_move_to_game(&game, &move);
    }
    
    return 0;
}
编译运行
bash
# 编译命令
gcc -o chess_system chess_database.c move_parser.c test_database.c

# 运行程序
./chess_system
功能状态
已实现功能
棋谱数据结构设计

棋步记录和管理

记谱法基础解析

文件存储功能

棋局统计信息

待完善功能
精确的坐标映射逻辑

完整的记谱法支持

棋谱查询和检索

图形界面集成

🐛 已知问题
中文显示乱码 - 终端编码问题，不影响功能

坐标映射简化 - 目前使用固定坐标，需要完善

开发者信息
开发者: Logan

项目状态: 开发中

最后更新: 2025年11月
