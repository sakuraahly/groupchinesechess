#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void quicksort(int arr[], int low, int high);


int main(){
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr)/sizeof(arr[0]);
    quicksort(arr,0,n-1);
    printf("Sorted array: \n");
    for (int i=0; i<n; i++)
        printf("%d ", arr[i]);
return 0;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pivot = arr[high]; // 选择最后一个元素作为基准
        int i = (low - 1);     // 指向小于基准的区域的末尾(小于等于基准值的区域初始为空)
//这个i用来跟踪所有小于等于基准值的元素的最后一个索引位置
        for (int j = low; j <= high - 1; j++) {
            // 如果当前元素小于或等于基准
            if (arr[j] <= pivot) {
                i++; // 扩展小于基准的区域
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }//循环结束后索引范围[low,i]包含所有小于等于基准值的元素,但是[i+1,high-1]包含所有大于等于基准值的元素]
        // 将基准放到正确位置(这样基准左侧的元素都小于等于基准,右侧的元素都大于等于基准)
        int temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        int pi = i + 1; // 分区点

        // 递归排序分区点左侧的元素
        quicksort(arr, low, pi - 1);
        // 递归排序分区点右侧的元素
        quicksort(arr, pi + 1, high);
    }
}

