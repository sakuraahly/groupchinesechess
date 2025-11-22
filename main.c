#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void setConsoleOutputCPToUTF8(){
    SetConsoleOutputCP(65001);
}

// 交换两个元素的值
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 分区函数 - 使用最后一个元素作为基准
int partition(int arr[], int low, int high) {
    int pivot = arr[high];  // 选择最后一个元素作为基准
    int i = (low - 1);      // 指向小于基准的区域的末尾
    
    for (int j = low; j <= high - 1; j++) {
        // 如果当前元素小于或等于基准
        if (arr[j] <= pivot) {
            i++;  // 扩展小于基准的区域
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);  // 将基准放到正确位置
    return (i + 1);
}

// 随机分区函数 - 避免最坏情况
int randomPartition(int arr[], int low, int high) {
    // 生成 low 到 high 之间的随机索引
    int random = low + rand() % (high - low + 1);
    // 将随机元素与最后一个元素交换
    swap(&arr[random], &arr[high]);
    return partition(arr, low, high);
}

// 快速排序主函数
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        // 获取分区点，基准元素已位于正确位置
        int pi = randomPartition(arr, low, high);
        
        // 递归排序分区点左侧的元素
        quickSort(arr, low, pi - 1);
        // 递归排序分区点右侧的元素
        quickSort(arr, pi + 1, high);
    }
}

// 打印数组
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// 测试代码
int main() {

    setConsoleOutputCPToUTF8();
    // 初始化随机数种子
    srand(time(NULL));
    
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("原始数组: ");
    printArray(arr, n);
    
    quickSort(arr, 0, n - 1);
    
    printf("排序后的数组: ");
    printArray(arr, n);
    
    // 测试更多例子
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    
    printf("\n另一个例子:\n");
    printf("原始数组: ");
    printArray(arr2, n2);
    
    quickSort(arr2, 0, n2 - 1);
    
    printf("排序后的数组: ");
    printArray(arr2, n2);
    
    return 0;
}