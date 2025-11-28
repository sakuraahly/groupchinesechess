#include <stdio.h>
#include <stdlib.h>

void fillSpiral(int **matrix, int x, int y, int start, int n) {
    if (n <= 0) return;
    if (n == 1) {
        matrix[y][x] = start;
        return;
    }

    // 填充上边（从左到右）
    for (int i = x; i < x + n - 1; i++) {
        matrix[y][i] = start++;
    }
    
    // 填充右边（从上到下）
    for (int i = y; i < y + n - 1; i++) {
        matrix[i][x + n - 1] = start++;
    }
    
    // 填充下边（从右到左）
    for (int i = x + n - 1; i > x; i--) {
        matrix[y + n - 1][i] = start++;
    }
    
    // 填充左边（从下到上）
    for (int i = y + n - 1; i > y; i--) {
        matrix[i][x] = start++;
    }
    
    // 递归处理内层（大小减2）
    fillSpiral(matrix, x + 1, y + 1, start, n - 2);
}

int main() {
    int N;
    printf("请输入方阵大小 N (3 <= N <= 25): ");
    scanf("%d", &N);

    // 动态分配 N x N 的二维数组
    int **matrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }

    // 填充螺旋方阵
    fillSpiral(matrix, 0, 0, 1, N);

    // 打印结果
    printf("\n螺旋方阵 (N = %d):\n", N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%-3d", matrix[i][j]); // 格式化对齐
        }
        printf("\n");
    }

    // 释放内存
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}