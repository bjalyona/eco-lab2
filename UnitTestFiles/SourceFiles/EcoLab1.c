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

#include "IEcoLab1Events.h"
#include "IdEcoList1.h"
#include "CEcoLab1Sink.h"
#include "IEcoConnectionPointContainer.h"

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
        arr[i] = rand() % 100;
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

void print_array(const void *startPtr, size_t elem_count) {
    const int32_t *array = (const int32_t *)startPtr;
    size_t i = 0;
    
    if (startPtr == 0 || elem_count == 0) {
        printf("[]\n");
        return;
    }
    
    printf("[");
    for (i = 0; i < elem_count; i++) {
        printf("%d", array[i]);
        if (i < elem_count - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void test_insertion_sort(IEcoLab1* pIEcoLab1, IEcoMemoryAllocator1* pIMem) {
    uint32_t size = 10;
    int32_t* arr = 0;
    int16_t result = 0;
    
    /* Выделение памяти для массива */
    arr = (int32_t*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int32_t));
    if (arr == 0) {
        printf("Memory allocation failed\n");
        return;
    }
    
    /* Заполнение массива случайными числами */
    fill_random(arr, size);
    
    /* Запуск сортировки вставкой */
    result = pIEcoLab1->pVTbl->MyFunction(pIEcoLab1, arr, size);
    
    if (result != ERR_ECO_SUCCESES) {
        printf("Sorting failed with error: %d\n", result);
    } else {
        /* Проверка корректности сортировки */
        if (is_sorted(arr, size)) {
            printf("Sorting: SUCCESS\n");
        } else {
            printf("Sorting: FAILED\n");
        }
    }
	printf("Sorted array:\n");
	print_array(arr, size);
    
    /* Освобождение памяти */
    pIMem->pVTbl->Free(pIMem, arr);
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

	/* Указатель на интерфейс контейнера точек подключения */
    IEcoConnectionPointContainer* pICPC = 0;
    /* Указатель на интерфейс точки подключения */
    IEcoConnectionPoint* pICP = 0;
    /* Указатель на обратный интерфейс */
    IEcoLab1Events* pIEcoLab1Sink = 0;
	IEcoUnknown* pISinkUnk = 0;
    uint32_t cAdvise = 0;

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

	/* Проверка поддержки подключений обратного интерфейса */
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoConnectionPointContainer, (void **)&pICPC);
    if (result != 0 || pICPC == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

	/* Запрос на получения интерфейса точки подключения */
    result = pICPC->pVTbl->FindConnectionPoint(pICPC, &IID_IEcoLab1Events, &pICP);
    if (result != 0 || pICP == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }
    /* Освобождение интерфейса */
    pICPC->pVTbl->Release(pICPC);

    /* Создание экземпляра обратного интерфейса */
    result = createCEcoLab1Sink(pIMem, (IEcoLab1Events**)&pIEcoLab1Sink);

    if (pIEcoLab1Sink != 0) {
        result = pIEcoLab1Sink->pVTbl->QueryInterface(pIEcoLab1Sink, &IID_IEcoUnknown,(void **)&pISinkUnk);
        if (result != 0 || pISinkUnk == 0) {
            /* Освобождение интерфейсов в случае ошибки */
            goto Release;
        }
        /* Подключение */
        result = pICP->pVTbl->Advise(pICP, pISinkUnk, &cAdvise);
        /* Проверка */
        if (result == 0 && cAdvise == 1) {
            /* Сюда можно добавить код */
        }
        /* Освобождение интерфейса */
        pISinkUnk->pVTbl->Release(pISinkUnk);
    }

    /* Запуск тестирования сортировки */
    test_insertion_sort(pIEcoLab1, pIMem);

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