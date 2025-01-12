/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file elf_types.h
 * @author Evan Stoddard
 * @brief ELF types and definitions
 */

#ifndef ELF_TYPES_H_
#define ELF_TYPES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief ELF Type enum
 *
 */
typedef enum
{
    ET_NONE = 0x0,
    ET_REL = 0x1,
    ET_EXEC = 0x2,
    ET_DYN = 0x3,
    ET_CORE = 0x4,
    ET_LOOS = 0xFE00,
    ET_HIOS = 0xFEFF,
    ET_LOPROC = 0xFF00,
    ET_HIPROC = 0xFFFF,
} elf_type_t;

/**
 * @brief (Incomplete) Program header type enum
 *
 */
typedef enum
{
    PT_NULL = 0x0,
    PT_LOAD = 0x1,
} elf_ph_type_t;

/**
 * @brief Program header segment flags
 *
 */
typedef enum
{
    PF_X = 0x1,
    PF_W = 0x2,
    PF_R = 0x4,
} elf_ph_flag_t;

/**
 * @brief Header for 32-bit ELF
 *
 */
typedef struct elf_32_header_t
{
    uint32_t magic;
    uint8_t bit_length;
    uint8_t endianness;
    uint8_t version;
    uint8_t os_abi;
    uint8_t abi_version;
    uint8_t padding[7];
    uint16_t type;
    uint16_t machine;
    uint16_t e_version;
    uint32_t entry;
    uint32_t ph_offset;
    uint32_t sh_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_header_size;
    uint16_t ph_count;
    uint16_t sh_header_size;
    uint16_t sh_count;
    uint16_t sh_str_idx;
} elf_32_header_t;

/**
 * @brief Program header definition
 *
 */
typedef struct elf_program_header_t
{
    uint32_t type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_file_size;
    uint32_t p_mem_size;
    uint32_t p_flags;
    uint32_t p_align;
} elf_program_header_t;

#ifdef __cplusplus
}
#endif
#endif /* ELF_TYPES_H_ */