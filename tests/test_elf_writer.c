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

#include "mock_elf_interface.h"

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
 * Setup/Teardown Functions
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

/*****************************************************************************
 * Test Cases
 *****************************************************************************/

/*****************************************************************************
 * Writer Context Initializatio Cases
 *****************************************************************************/

void test_elf_writer_init_null_context(void) {
    
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface;
    
    elf_writer_ret_t ret = elf_writer_init(NULL, &interface);
    
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_BAD_ARGS);
}

void test_elf_writer_init_null_interface(void) {
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface;

    elf_writer_ret_t ret = elf_writer_init(&ctx, NULL);

    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_BAD_ARGS);
}

void test_elf_writer_init_invalid_interface(void) {
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {0};

    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);

    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_BAD_INTERFACE);
}

void test_elf_writer_init_valid(void) {
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close
    };

    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);

    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
    TEST_ASSERT_EQUAL_MEMORY(&ctx.elf_header, &prv_default_elf_23_header, sizeof(elf_32_header_t));
}

/*****************************************************************************
 * Program Entry Cases
 *****************************************************************************/

void test_add_one_program_entry(void) {
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);
    
    // Ensure that linked list is empty
    TEST_ASSERT_EQUAL(ctx.ph_head, NULL);
    TEST_ASSERT_EQUAL(ctx.ph_tail, NULL);
    TEST_ASSERT_EQUAL(ctx.elf_header.ph_count, 0);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};
    
    ret = elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
    
    TEST_ASSERT_EQUAL(ctx.ph_head, &entry);
    TEST_ASSERT_EQUAL(ctx.ph_tail, &entry);
    TEST_ASSERT_EQUAL(ctx.elf_header.ph_count, 1);

    TEST_ASSERT_EQUAL(entry.prev, NULL);
    TEST_ASSERT_EQUAL(entry.next, NULL);
    
    TEST_ASSERT_EQUAL(entry.ph.type, PT_LOAD);
    TEST_ASSERT_EQUAL(entry.ph.p_align, 1);
    TEST_ASSERT_EQUAL(entry.ph.p_mem_size, sizeof(payload));
    TEST_ASSERT_EQUAL(entry.ph.p_file_size, 2 * sizeof(payload));
    TEST_ASSERT_EQUAL(entry.ph.p_paddr, 0xFEEDBEEF);
    TEST_ASSERT_EQUAL(entry.ph.p_vaddr, 0xCAFEBABE);
    TEST_ASSERT_EQUAL(entry.ph.p_flags, PF_R | PF_X);

    TEST_ASSERT_EQUAL(ctx.total_size, sizeof(elf_32_header_t) + sizeof(elf_program_header_t) + entry.ph.p_file_size);
}

void test_add_two_program_entries(void) {
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);
    
    // Ensure that linked list is empty
    TEST_ASSERT_EQUAL(ctx.ph_head, NULL);
    TEST_ASSERT_EQUAL(ctx.ph_tail, NULL);
    TEST_ASSERT_EQUAL(ctx.elf_header.ph_count, 0);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};

    elf_writer_program_entry_t entry_two = {0};
    uint8_t payload_two[128] = {0};

    ret = elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);

    ret = elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0xDEADBEEF, 0xCAFED00D, PF_W | PF_R, payload_two, sizeof(payload_two), 2 * sizeof(payload_two));
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);

    TEST_ASSERT_EQUAL(ctx.ph_head, &entry);
    TEST_ASSERT_EQUAL(ctx.ph_tail, &entry_two);
    TEST_ASSERT_EQUAL(ctx.elf_header.ph_count, 2);

    TEST_ASSERT_EQUAL(entry.prev, NULL);
    TEST_ASSERT_EQUAL(entry.next, &entry_two);
    TEST_ASSERT_EQUAL(entry_two.prev, &entry);
    TEST_ASSERT_EQUAL(entry_two.next, NULL);

    TEST_ASSERT_EQUAL(entry.ph.type, PT_LOAD);
    TEST_ASSERT_EQUAL(entry.ph.p_align, 1);
    TEST_ASSERT_EQUAL(entry.ph.p_mem_size, sizeof(payload));
    TEST_ASSERT_EQUAL(entry.ph.p_file_size, 2 * sizeof(payload));
    TEST_ASSERT_EQUAL(entry.ph.p_paddr, 0xFEEDBEEF);
    TEST_ASSERT_EQUAL(entry.ph.p_vaddr, 0xCAFEBABE);
    TEST_ASSERT_EQUAL(entry.ph.p_flags, PF_R | PF_X);

    TEST_ASSERT_EQUAL(entry_two.ph.type, PT_LOAD);
    TEST_ASSERT_EQUAL(entry_two.ph.p_align, 1);
    TEST_ASSERT_EQUAL(entry_two.ph.p_mem_size, sizeof(payload_two));
    TEST_ASSERT_EQUAL(entry_two.ph.p_file_size, 2 * sizeof(payload_two));
    TEST_ASSERT_EQUAL(entry_two.ph.p_paddr, 0xDEADBEEF);
    TEST_ASSERT_EQUAL(entry_two.ph.p_vaddr, 0xCAFED00D);
    TEST_ASSERT_EQUAL(entry_two.ph.p_flags, PF_R | PF_W);

    TEST_ASSERT_EQUAL(ctx.total_size, sizeof(elf_32_header_t) + (sizeof(elf_program_header_t) * 2) + entry.ph.p_file_size + entry_two.ph.p_file_size);
}

void test_program_header_offset_calculation(void)
{
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_init(&ctx, &interface);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};

    elf_writer_program_entry_t entry_two = {0};
    uint8_t payload_two[128] = {0};

    elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0xDEADBEEF, 0xCAFED00D, PF_W | PF_R, payload_two, sizeof(payload_two), 2 * sizeof(payload_two));

    prv_calculate_program_header_offset(&ctx);
   
    size_t offset_one = sizeof(elf_32_header_t) + (2 * sizeof(elf_program_header_t));
    size_t offset_two = offset_one + (sizeof(payload) * 2);

    TEST_ASSERT_EQUAL(entry.ph.p_offset, offset_one);
    TEST_ASSERT_EQUAL(entry_two.ph.p_offset, offset_two);
}

/*****************************************************************************
 * Write Header Cases
 *****************************************************************************/

void test_elf_writer_write_elf_header(void)
{
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);
    
    elf_iface_file_write_ExpectAndReturn(NULL, 0, &ctx.elf_header, sizeof(elf_32_header_t), ELF_WRITER_RET_SUCCESS);
    
    ret = prv_write_elf_header(&ctx);
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
}

void test_elf_writer_write_program_headers(void)
{
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_init(&ctx, &interface);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};

    elf_writer_program_entry_t entry_two = {0};
    uint8_t payload_two[128] = {0};

    elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0xDEADBEEF, 0xCAFED00D, PF_W | PF_R, payload_two, sizeof(payload_two), 2 * sizeof(payload_two));

    prv_calculate_program_header_offset(&ctx);

    size_t offset_one = sizeof(elf_32_header_t) + (2 * sizeof(elf_program_header_t));
    size_t offset_two = offset_one + (sizeof(payload) * 2);

    elf_iface_file_write_ExpectAndReturn(NULL, sizeof(elf_32_header_t), &entry.ph, sizeof(elf_program_header_t), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, sizeof(elf_32_header_t) + sizeof(elf_program_header_t), &entry_two.ph, sizeof(elf_program_header_t), ELF_WRITER_RET_SUCCESS);
    
    elf_writer_ret_t ret = prv_write_ph_headers(&ctx);
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
}

/*****************************************************************************
 * Write Entry Cases
 *****************************************************************************/

void test_elf_writer_write_program_entries(void)
{
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_init(&ctx, &interface);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};

    elf_writer_program_entry_t entry_two = {0};
    uint8_t payload_two[128] = {0};

    elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0xDEADBEEF, 0xCAFED00D, PF_W | PF_R, payload_two, sizeof(payload_two), 2 * sizeof(payload_two));

    prv_calculate_program_header_offset(&ctx);

    size_t offset_one = sizeof(elf_32_header_t) + (2 * sizeof(elf_program_header_t));
    size_t offset_two = offset_one + (sizeof(payload) * 2);

    elf_iface_file_write_ExpectAndReturn(NULL, offset_one, entry.payload, sizeof(payload), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, offset_two, entry_two.payload, sizeof(payload_two), ELF_WRITER_RET_SUCCESS);

    elf_writer_ret_t ret = prv_write_ph_entries(&ctx);
    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
}

/*****************************************************************************
 * ELF Commit Cases
 *****************************************************************************/

void test_elf_writer_commit(void)
{
    elf_writer_ctx_t ctx = {0};
    elf_writer_interface_t interface = {
        .open = elf_iface_file_open,
        .write = elf_iface_file_write,
        .close = elf_iface_file_close};

    elf_writer_init(&ctx, &interface);

    // Add entry
    elf_writer_program_entry_t entry = {0};
    uint8_t payload[1024] = {0};

    elf_writer_program_entry_t entry_two = {0};
    uint8_t payload_two[128] = {0};

    elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0xFEEDBEEF, 0xCAFEBABE, PF_R | PF_X, payload, sizeof(payload), 2 * sizeof(payload));
    elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0xDEADBEEF, 0xCAFED00D, PF_W | PF_R, payload_two, sizeof(payload_two), 2 * sizeof(payload_two));

    prv_calculate_program_header_offset(&ctx);

    size_t total_size = sizeof(elf_32_header_t) + (2 * sizeof(elf_program_header_t)) + (2 * sizeof(payload)) + (2 * sizeof(payload_two));
    size_t offset_one = sizeof(elf_32_header_t) + (2 * sizeof(elf_program_header_t));
    size_t offset_two = offset_one + (sizeof(payload) * 2);

    elf_iface_file_open_ExpectAndReturn(total_size, NULL, ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, 0, &ctx.elf_header, sizeof(elf_32_header_t), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, sizeof(elf_32_header_t), &entry.ph, sizeof(elf_program_header_t), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, sizeof(elf_32_header_t) + sizeof(elf_program_header_t), &entry_two.ph, sizeof(elf_program_header_t), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, offset_one, entry.payload, sizeof(payload), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_write_ExpectAndReturn(NULL, offset_two, entry_two.payload, sizeof(payload_two), ELF_WRITER_RET_SUCCESS);
    elf_iface_file_close_ExpectAndReturn(NULL, ELF_WRITER_RET_SUCCESS);
    
    elf_writer_ret_t ret = elf_writer_commit(&ctx);

    TEST_ASSERT_EQUAL(ret, ELF_WRITER_RET_SUCCESS);
}