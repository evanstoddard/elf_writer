/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file main.c
 * @author Evan Stoddard
 * @brief Example ARM Core Dump
 */

#include "elf_writer.h"

#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

static elf_writer_ctx_t ctx;
static elf_writer_interface_t interface;

static FILE *file;

static char *file_name;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

static elf_writer_ret_t prv_file_open(size_t size, void *file_ctx)
{
    file = fopen(file_name, "wb");
    if (file == NULL)
    {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    return ELF_WRITER_RET_SUCCESS;
}

static elf_writer_ret_t prv_file_write(void *file_ctx, size_t offset, void *buffer, size_t len)
{
    size_t ret = fwrite(buffer, 1, len, file);
    
    if (ret != len) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    return ELF_WRITER_RET_SUCCESS;
}

static elf_writer_ret_t prv_file_close(void *file_ctx)
{
    int ret = fclose(file);
    
    if (ret != 0) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    return ELF_WRITER_RET_SUCCESS;
}

static void die(char *msg, int err) {
    printf("Error: %s\r\n", msg);
    
    exit(err);
}

int main(int argc, char **argv) {
    
    if (argc < 2) {
        die("Please provide a path to write the ELF file to.", 1);
    }
    
    
    file_name = argv[1];
    
    interface.open = prv_file_open;
    interface.write = prv_file_write;
    interface.close = prv_file_close;
    
    elf_writer_ret_t ret = elf_writer_init(&ctx, &interface);
    if (ret != ELF_WRITER_RET_SUCCESS) {
        die("Failed to initialize ELF writer context.", ret);
    }
    
    ret = elf_writer_set_elf_type(&ctx, ET_CORE);
    if (ret != ELF_WRITER_RET_SUCCESS)
    {
        die("Failed to set ELF type.", ret);
    }
    
    ret = elf_writer_set_machine_info(&ctx, 0x61, 0, 0x28, 0);
    if (ret != ELF_WRITER_RET_SUCCESS)
    {
        die("Failed to set machine info.", ret);
    }

    uint8_t payload[1024] = {0};
    elf_writer_program_entry_t entry = {0};
    elf_writer_add_program_entry(&ctx, &entry, PT_LOAD, 0x0, 0x08000000, PF_R, payload, sizeof(payload), sizeof(payload));
    
    uint8_t payload_two[256] = {0};
    elf_writer_program_entry_t entry_two = {0};
    elf_writer_add_program_entry(&ctx, &entry_two, PT_LOAD, 0x0, 0x20000000, PF_W | PF_X | PF_R, payload_two, sizeof(payload_two), sizeof(payload_two));

    ret = elf_writer_commit(&ctx);
    if (ret != ELF_WRITER_RET_SUCCESS)
    {
        die("Failed to write ELF.", ret);
    }

    return 0;
}