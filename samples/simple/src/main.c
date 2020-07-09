/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdlib.h>
#include <string.h>
#include "bh_platform.h"
#include "bh_assert.h"
#include "bh_log.h"
#include "wasm_export.h"
#include "test_wasm.h"

#include <zephyr.h>
#include <sys/printk.h>

#define CONFIG_GLOBAL_HEAP_BUF_SIZE 131072
#define CONFIG_APP_STACK_SIZE 8192
#define CONFIG_APP_HEAP_SIZE 8192

#ifdef CONFIG_NO_OPTIMIZATIONS
#define CONFIG_MAIN_THREAD_STACK_SIZE 8192
#else
#define CONFIG_MAIN_THREAD_STACK_SIZE 4096
#endif

static int app_argc;
static char **app_argv;

static NativeSymbol native_symbols[] =
{
  EXPORT_WASM_API_WITH_SIG(test_call_wasm_runtime_native, "()"),
  // {
  //           "test_wasm_runtime_native_print", 		    // the name of WASM function name
  //           test_wasm_runtime_native_print, 			// the native function pointer
  //           "()",			// the function prototype signature, avoid to use i32
  //           NULL                // attachment is NULL
  // }
  EXPORT_WASM_API_WITH_SIG(aerogel_sensor_native, "(*~*~*~*~)"),
  EXPORT_WASM_API_WITH_SIG(aerogel_actuator_native, "(*~*~*~*~*~)")
};

/**
 * Find the unique main function from a WASM module instance
 * and execute that function.
 *
 * @param module_inst the WASM module instance
 * @param argc the number of arguments
 * @param argv the arguments array
 *
 * @return true if the main function is called, false otherwise.
 */
bool
wasm_application_execute_main(wasm_module_inst_t module_inst,
                              int argc, char *argv[]);

static void*
app_instance_main(wasm_module_inst_t module_inst)
{
    const char *exception;

    wasm_application_execute_main(module_inst, app_argc, app_argv);
    if ((exception = wasm_runtime_get_exception(module_inst)))
        printf("%s\n", exception);
    return NULL;
}

static char global_heap_buf[CONFIG_GLOBAL_HEAP_BUF_SIZE] = { 0 };
static char global_heap_buf2[CONFIG_GLOBAL_HEAP_BUF_SIZE] = { 0 };

uint8* get_wasm_binary_array(char* wasm_binary_array, uint32* length) {
  uint32 index = 0;
  for(; index < wasm_index_len; index++) {
    if(!strcmp(wasm_binary_array, wasm_index[index])) {
      break;
    }
  }

  if(index >= wasm_index_len) {
    printf("something wrong with the indexing.\n");
    return 0;
  }

  switch(index) {
    case 0:
      length[0] = sizeof(wasm_max_access);
      return (uint8*)wasm_max_access;
    case 1:
      length[0] = sizeof(wasm_regular_uav1);
      return (uint8*)wasm_regular_uav1;
    case 2:
      length[0] = sizeof(wasm_shortage_mcu);
      return (uint8*)wasm_shortage_mcu;
    case 3:
      length[0] = sizeof(wasm_regular_smarthome1);
      return (uint8*)wasm_regular_smarthome1;
    case 4:
      length[0] = sizeof(wasm_shortage_camera_power);
      return (uint8*)wasm_shortage_camera_power;
    case 5:
      length[0] = sizeof(wasm_regular_uav2);
      return (uint8*)wasm_regular_uav2;
    case 6:
      length[0] = sizeof(wasm_regular_smarthome2);
      return (uint8*)wasm_regular_smarthome2;
    case 7:
      length[0] = sizeof(wasm_init_access_denied);
      return (uint8*)wasm_init_access_denied;
    case 8:
      length[0] = sizeof(wasm_shortage_memory);
      return (uint8*)wasm_shortage_memory;
    case 9:
      length[0] = sizeof(wasm_regular_uav2_20hz);
      return (uint8*)wasm_regular_uav2_20hz;
    case 10:
      length[0] = sizeof(wasm_regular_uav2_40hz);
      return (uint8*)wasm_regular_uav2_40hz;
    case 11:
      length[0] = sizeof(wasm_regular_uav2_60hz);
      return (uint8*)wasm_regular_uav2_60hz;
    case 12:
      length[0] = sizeof(wasm_regular_uav2_80hz);
      return (uint8*)wasm_regular_uav2_80hz;
    case 13:
      length[0] = sizeof(wasm_regular_uav2_100hz);
      return (uint8*)wasm_regular_uav2_100hz;
    case 14:
      length[0] = sizeof(wasm_test_sensor);
      return (uint8*)wasm_test_sensor;
    case 15:
      length[0] = sizeof(wasm_test_actuator);
      return (uint8*)wasm_test_actuator;
    case 16:
      length[0] = sizeof(wasm_test_file2);
      return (uint8*)wasm_test_file2;
    case 17:
      length[0] = sizeof(wasm_test_file);
      return (uint8*)wasm_test_file;
  }
  return 0;
}

void iwasm_second(void* wasm_binary_array, void* module_name, void *arg3) {
  printf("\n\n\nAlso hello\n");

  int start, end;
  start = os_time_get_renju_boot_microsecond();
  uint8 *wasm_file_buf = NULL;
  wasm_module_t wasm_module = NULL;
  wasm_module_inst_t wasm_module_inst = NULL;
  RuntimeInitArgs init_args;
  char error_buf[128];
  uint32 wasm_aerogel_file_size = 0;

  // (void) arg1;
  // (void) arg2;
  (void) arg3;

  memset(&init_args, 0, sizeof(RuntimeInitArgs));

  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = global_heap_buf2;
  init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf2);

  init_args.native_module_name = "env";
  init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
  init_args.native_symbols = native_symbols;

  /* initialize runtime environment */
  if (!wasm_runtime_full_init(&init_args)) {
      printf("Init runtime environment failed.\n");
      return;
  }

  wasm_file_buf = get_wasm_binary_array((char*)wasm_binary_array, &wasm_aerogel_file_size);
  // printf("array_length points to: %p\n", array_length);
  // printf("file buf info: %p\n", wasm_file_buf);
  // printf("actuator address: %p\n", wasm_test_actuator);
  // printf("file size: %d\n", wasm_aerogel_file_size);

  if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_aerogel_file_size,
                                        error_buf, sizeof(error_buf)))) {
      printf("%s\n", error_buf);
      goto fail1;
  }
  // while(1);
  //
  if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module,
                                                    CONFIG_APP_STACK_SIZE,
                                                    CONFIG_APP_HEAP_SIZE,
                                                    error_buf,
                                                    sizeof(error_buf)))) {
      printf("%s\n", error_buf);
      goto fail2;
  }
  wasm_add_module_name(wasm_module_inst, (char*)module_name);

  /* invoke the main function */
  // Real execution from here.
  int exe_start, exe_end;
  exe_start = k_uptime_get_32();
  app_instance_main(wasm_module_inst);
  exe_end = k_uptime_get_32();
  printf("\t%d\n", exe_end - exe_start);
  printf("%s real execution time: %d ms\n", (char*)module_name, exe_end - exe_start);

  /* destroy the module instance */
  wasm_runtime_deinstantiate(wasm_module_inst);

fail2:
  /* unload the module */
  wasm_runtime_unload(wasm_module);

fail1:
  /* destroy runtime environment */
  wasm_runtime_destroy();

  end = os_time_get_renju_boot_microsecond();

  printf("%s elpase: %d ms\n", (char*)module_name, (end - start)/1000);
}

void iwasm_main(void *arg1, void *arg2, void *arg3)
{
    printf("Hello here.\n");
    int start, end;
    start = k_uptime_get_32();
    uint8 *wasm_file_buf = NULL;
    uint32 wasm_file_size;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    RuntimeInitArgs init_args;
    char error_buf[128];
#if WASM_ENABLE_LOG != 0
    int log_verbose_level = 2;
#endif

    (void) arg1;
    (void) arg2;
    (void) arg3;


    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return;
    }

#if WASM_ENABLE_LOG != 0
    bh_log_set_verbose_level(log_verbose_level);
#endif

    /* load WASM byte buffer from byte buffer of include file */
    wasm_file_buf = (uint8*) wasm_test_file;
    wasm_file_size = sizeof(wasm_test_file);

    /* load WASM module */
    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                          error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail1;
    }

    /* instantiate the module */
    if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module,
                                                      CONFIG_APP_STACK_SIZE,
                                                      CONFIG_APP_HEAP_SIZE,
                                                      error_buf,
                                                      sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail2;
    }

    /* invoke the main function */
    app_instance_main(wasm_module_inst);

    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);

fail2:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();

    end = k_uptime_get_32();

    printf("elpase: %d\n", (end - start));
}

#define MAIN_THREAD_STACK_SIZE (CONFIG_MAIN_THREAD_STACK_SIZE)
#define MAIN_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(iwasm_main_thread_stack, MAIN_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(iwasm_main2_thread_stack, MAIN_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(iwasm_main3_thread_stack, MAIN_THREAD_STACK_SIZE);
static struct k_thread iwasm_main_thread;
static struct k_thread iwasm_main2_thread;
static struct k_thread iwasm_main3_thread;

bool iwasm_init(void)
{
    // k_thread_create(&iwasm_main_thread, iwasm_main_thread_stack,
    //                               MAIN_THREAD_STACK_SIZE,
    //                               iwasm_main, NULL, NULL, NULL,
    //                               MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
    // printf("first tid: %d\n", tid);

    /* RENJU: DEFINED AS A GLOBAL VARIABLE*/
    // wasm_aerogel_file_size = sizeof(wasm_test_file2);
    // k_thread_create(&iwasm_main2_thread, iwasm_main2_thread_stack,
    //                               MAIN_THREAD_STACK_SIZE,
    //                               iwasm_second, (void*)"wasm_regular_uav1", (void*)"regular_uav1", NULL,
    //                               MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
    // printf("Second tid: %d\n", tid);
    k_thread_create(&iwasm_main3_thread, iwasm_main3_thread_stack,
                                  MAIN_THREAD_STACK_SIZE,
                                  iwasm_second, "wasm_init_access_denied", (void*)"init_access_denial", NULL,
                                  MAIN_THREAD_PRIORITY, 0, K_MSEC(10000));
    return true;
}
void main(void)
{
    // create a main thread.
    iwasm_init();
}
