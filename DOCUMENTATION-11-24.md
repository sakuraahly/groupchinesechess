# 技术文档-11-24

## 项目结构

chinese-chess/
├── chess_database.h # 数据结构和常量定义
├── chess_database.c # 核心数据库功能实现
├── move_parser.h # 棋谱解析器头文件
├── move_parser.c # 棋谱解析器实现
├── test_database.c # 测试程序
└── DOCUMENTATION.md # 项目说明文档

## 核心功能模块

### 1. 棋子编码系统

系统使用统一的编码方案表示棋子：

- **颜色定义**：
  - `COLOR_RED = 1`
  - `COLOR_BLACK = 2`

- **棋子类型**：
  - `TYPE_JIANG = 1` (将/帅)
  - `TYPE_SHI = 2` (士/仕)
  - `TYPE_XIANG = 3` (象/相)
  - `TYPE_MA = 4` (马)
  - `TYPE_JU = 5` (车)
  - `TYPE_PAO = 6` (炮)
  - `TYPE_BING = 7` (兵/卒)

- **棋子编码规则**：颜色 × 10 + 类型
  - 红方棋子：`RED_JIANG = 11`, `RED_SHI = 12`, ...
  - 黑方棋子：`BLACK_JIANG = 21`, `BLACK_SHI = 22`, ...

### 2. 棋盘坐标系

- **x轴**：0-9行，从上到下
- **y轴**：0-8列，从左到右
- **左上角(0,0)**：黑方车的位置

### 3. 核心数据结构

#### ChessMove - 棋步记录

记录每一步棋的详细信息，包括棋子、坐标、时间等。

#### GameRecord - 对局记录

管理整盘棋的所有信息，包括玩家、棋步记录、对局结果等。

### 4. 主要API函数

#### 数据库管理函数

- `init_game_record()` - 初始化对局记录
- `add_move_to_game()` - 添加棋步到对局
- `set_game_result()` - 设置对局结果
- `print_game_summary()` - 打印对局摘要
- `save_game_to_file()` - 保存对局到文件

#### 棋子操作函数

- `encode_piece()` - 编码棋子
- `get_piece_color()` - 获取棋子颜色
- `get_piece_type()` - 获取棋子类型
- `get_piece_name_cn()` - 获取棋子中文名

#### 棋谱解析函数

- `parse_notation()` - 解析棋谱字符串
- `print_parse_result()` - 打印解析结果

## 编译和运行

### 编译命令

```bash
gcc -c chess_database.c -o chess_database.o
gcc -c move_parser.c -o move_parser.o
gcc -c test_database.c -o test_database.o
gcc chess_database.o move_parser.o test_database.o -o chess_game
```

## 运行程序

./chess_game
测试输出示例
程序运行后会输出：

对局初始化信息

棋步记录过程

棋谱解析测试

对局统计摘要

文件保存确认

使用示例
创建对局
c
GameRecord game;
init_game_record(&game, "G20231124001", "张三", "李四");
记录棋步
c
ChessMove move;
init_chess_move(&move, 1, RED_BING, "兵五进一", 6, 4, 5, 4);
add_move_to_game(&game, &move);
解析棋谱
c
ParseResult result = parse_notation("炮8平5", COLOR_BLACK, &game);
print_parse_result(&result);
保存对局
c
save_game_to_file(&game, "my_game.txt");
文件格式
导出的对局文件包含：

对局基本信息（ID、玩家、结果）

详细的棋步记录（步数、玩家、棋子、走法、坐标、时间）

当前状态
已完成功能
基础数据结构定义

棋步记录和管理

对局信息统计

文件导出功能

基础测试框架
