/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file test_elf_writer.c
 * @author Evan Stoddard
 * @brief Test cases for ELF writer module
 */

#include "unity.h"

#include <stdint.h>
#include <stddef.h>

#include "elf_writer.c"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Set called for every test case
 * 
 */
void setUp(void) {}

/**
 * @brief Tear down called for every test case
 * 
 */
void tearDown(void) {}

/**
 * @brief Test elf writer init with NULL context
 * 
 */
void test_elf_writer_init_null_context(void) {
    
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface;
    
    elf_writer_ret_t ret = elf_writer_init(NULL, &interface);
    
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_BAD_ARGS);
}