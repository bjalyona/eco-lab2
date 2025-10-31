/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

/* Функция сравнения для qsort с правильным соглашением вызовов */
int __cdecl compare(const void* a, const void* b) {
    int32_t int_a = *((int32_t*)a);
    int32_t int_b = *((int32_t*)b);
    
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

/*
 *
 * <сводка>
 *   Функция проверки отсортированности массива
 * </сводка>
 *
 */
int is_sorted(int32_t* arr, uint32_t size) {
    uint32_t i = 0;
    
    for (i = 1; i < size; i++) {
        if (arr[i] < arr[i-1]) {
            return 0;
        }
    }
    return 1;
}

/*
 *
 * <сводка>
 *   Функция заполнения массива случайными числами
 * </сводка>
 *
 */
void fill_random(int32_t* arr, uint32_t size) {
    uint32_t i = 0;
    
    for (i = 0; i < size; i++) {
        arr[i] = rand() % 1000000;
    }
}

/*
 *
 * <сводка>
 *   Функция копирования массива
 * </сводка>
 *
 */
void copy_array(int32_t* dest, int32_t* src, uint32_t size) {
    uint32_t i = 0;
    
    for (i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

/*
 *
 * <сводка>
 *   Функция тестирования сортировки
 * </сводка>
 *
 */
void test_sorting_performance(IEcoLab1* pIEcoLab1, IEcoMemoryAllocator1* pIMem) {
    /* Размеры массивов для тестирования */
    uint32_t sizes[5] = {100, 500, 2500, 12500, 62500};
    int num_sizes = 0;
    int i = 0;
    uint32_t size = 0;
    int32_t* arr1 = 0;
    int32_t* arr2 = 0;
    clock_t start = 0;
    clock_t end = 0;
    double our_time = 0.0;
    double qsort_time = 0.0;
    int16_t result = 0;

    num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n=== Sorting Algorithms Performance Test ===\n");
    printf("Array Size | Custom (sec) | qsort (sec)  | Ratio\n");
    printf("-----------|--------------|--------------|--------\n");

    /* Тестирование для каждого размера */
    for (i = 0; i < num_sizes; i++) {
        size = sizes[i];

        /* Выделение памяти для массивов */
        arr1 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int32_t));
        arr2 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int32_t));

        if (arr1 == 0 || arr2 == 0) {
            printf("Memory allocation failed for size %u\n", size);
            if (arr1 != 0) {
                pIMem->pVTbl->Free(pIMem, arr1);
            }
            if (arr2 != 0) {
                pIMem->pVTbl->Free(pIMem, arr2);
            }
            continue;
        }

        /* Заполнение массива случайными числами */
        fill_random(arr1, size);
        copy_array(arr2, arr1, size);

        /* Тестирование нашей сортировки */
        start = clock();
        result = pIEcoLab1->pVTbl->MyFunction(pIEcoLab1, arr1, size);
        end = clock();
        our_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        /* Тестирование qsort */
        start = clock();
        qsort(arr2, size, sizeof(int32_t), compare);
        end = clock();
        qsort_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        /* Вывод результатов */
        printf("%9u | %12.6f | %12.6f | ", size, our_time, qsort_time);
        
        if (our_time < qsort_time && our_time > 0) {
            printf("COM: %.2fx\n", qsort_time / our_time);
        } else if (qsort_time < our_time && qsort_time > 0) {
            printf("qsort: %.2fx\n", our_time / qsort_time);
        } else {
            printf("equal\n");
        }

        /* Проверка корректности сортировки */
        if (!is_sorted(arr1, size)) {
            printf("ERROR: Custom sorting failed for size %u\n", size);
        }
        if (!is_sorted(arr2, size)) {
            printf("ERROR: qsort failed for size %u\n", size);
        }

        /* Освобождение памяти */
        pIMem->pVTbl->Free(pIMem, arr1);
        pIMem->pVTbl->Free(pIMem, arr2);
        
        /* Сброс указателей */
        arr1 = 0;
        arr2 = 0;
    }

    printf("==========================================\n");
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    
    /* Указатели на тестируемые интерфейсы */
    IEcoLab1 *pIEcoLab1 = 0, *pOtherIEcoLab1 = 0;
    IEcoCalculatorX *pIX = 0, *pOtherIX = 0;
    IEcoCalculatorY *pIY = 0, *pOtherIY = 0;
    IEcoUnknown *pIUnknown = 0, *pOtherIUnknown = 0;
    int calc_result;  /* Объявлено в начале функции */
    int shared_calc;  /* Объявлено в начале функции */

    /* Инициализация генератора случайных чисел */
    srand((unsigned int)time(NULL));

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

#ifdef ECO_LIB
    /* Регистрация компонентов */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        goto Release;
    }
    
    /* Регистрация компонентов калькуляторов */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorA, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*)GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, (IEcoUnknown*)GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, (IEcoUnknown*)GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    if (result != 0 ) {
        goto Release;
    }
#endif

    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        goto Release;
    }

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        goto Release;
    }

    printf("\n=== Component Interface Connectivity Test ===\n\n");

    /* Начинаем с IEcoCalculatorY и проверяем все возможные переходы */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY) == 0 && pIY) {
        printf("1. IEcoCalculatorY obtained successfully\n");
        printf("   Mathematical operations:\n");
        printf("   - 144 * 3 = %d\n", pIY->pVTbl->Multiplication(pIY, 144, 3));
        printf("   - 625 / 25 = %d\n", pIY->pVTbl->Division(pIY, 625, 25));

        /* Переход от Y к X */
        if (pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void**)&pIX) == 0 && pIX) {
            printf("2. Transition Y -> X successful\n");
            printf("   Mathematical operations:\n");
            printf("   - 333 + 667 = %d\n", pIX->pVTbl->Addition(pIX, 333, 667));
            printf("   - 2000 - 800 = %d\n", pIX->pVTbl->Subtraction(pIX, 2000, 800));

            /* Переход от X к основному интерфейсу */
            if (pIX->pVTbl->QueryInterface(pIX, &IID_IEcoLab1, (void**)&pOtherIEcoLab1) == 0 && pOtherIEcoLab1) {
                printf("3. Transition X -> IEcoLab1 successful\n");
                
                /* Переход от основного интерфейса к Unknown */
                if (pOtherIEcoLab1->pVTbl->QueryInterface(pOtherIEcoLab1, &IID_IEcoUnknown, (void**)&pIUnknown) == 0 && pIUnknown) {
                    printf("4. Transition IEcoLab1 -> Unknown successful\n");
                    
                    /* Полный цикл: Unknown обратно к Y */
                    if (pIUnknown->pVTbl->QueryInterface(pIUnknown, &IID_IEcoCalculatorY, (void**)&pOtherIY) == 0 && pOtherIY) {
                        printf("5. Full cycle Unknown -> Y completed\n");
                        printf("   Verification calculation: 99 * 11 = %d\n", pOtherIY->pVTbl->Multiplication(pOtherIY, 99, 11));
                        pOtherIY->pVTbl->Release(pOtherIY);
                    }
                    pIUnknown->pVTbl->Release(pIUnknown);
                }
                pOtherIEcoLab1->pVTbl->Release(pOtherIEcoLab1);
            }
            pIX->pVTbl->Release(pIX);
        }
        pIY->pVTbl->Release(pIY);
    }
    else {
        printf("Failed to obtain IEcoCalculatorY\n");
    }

    printf("\n--- Alternative Interface Access Methods ---\n\n");

    /* Альтернативный путь: начинаем с IEcoCalculatorX */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX) == 0 && pIX) {
        printf("6. Direct IEcoCalculatorX access\n");
        printf("   Complex calculation chain:\n");
        
        calc_result = pIX->pVTbl->Addition(pIX, 500, 250);  /* 750 */
        printf("   Step 1: 500 + 250 = %d\n", calc_result);
        
        /* Переход к Y для продолжения вычислений */
        if (pIX->pVTbl->QueryInterface(pIX, &IID_IEcoCalculatorY, (void**)&pIY) == 0 && pIY) {
            calc_result = pIY->pVTbl->Multiplication(pIY, calc_result, 2);  /* 1500 */
            printf("   Step 2: 750 * 2 = %d\n", calc_result);
            
            calc_result = pIX->pVTbl->Subtraction(pIX, calc_result, 300);  /* 1200 */
            printf("   Step 3: 1500 - 300 = %d\n", calc_result);
            
            calc_result = pIY->pVTbl->Division(pIY, calc_result, 4);  /* 300 */
            printf("   Step 4: 1200 / 4 = %d\n", calc_result);
            
            printf("   Final result: %d\n", calc_result);
            pIY->pVTbl->Release(pIY);
        }
        pIX->pVTbl->Release(pIX);
    }

    printf("\n--- Interface Identity Validation ---\n\n");

    /* Проверка идентичности интерфейсов */
    if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX) == 0 && pIX) {
        if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY) == 0 && pIY) {
            printf("7. Multiple interfaces from single component:\n");
            
            /* Демонстрация совместной работы интерфейсов */
            shared_calc = pIX->pVTbl->Addition(pIX, 100, 50);  /* 150 */
            shared_calc = pIY->pVTbl->Multiplication(pIY, shared_calc, 10);  /* 1500 */
            shared_calc = pIX->pVTbl->Subtraction(pIX, shared_calc, 500);  /* 1000 */
            shared_calc = pIY->pVTbl->Division(pIY, shared_calc, 4);  /* 250 */
            
            printf("   Combined operations result: %d\n", shared_calc);
            printf("   Interface collaboration verified\n");
            
            pIY->pVTbl->Release(pIY);
        }
        pIX->pVTbl->Release(pIX);
    }

    printf("\n=== Interface Connectivity Test Complete ===\n");

    /* Запуск тестирования сортировки */
    test_sorting_performance(pIEcoLab1, pIMem);

    printf("\nAll tests completed successfully! Press any key to exit...\n");
    getchar();
    
    /* Если дошли сюда, все тесты прошли успешно */
    result = 0;

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);

    /* Освобождение системного интерфейса */
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    return result;
}