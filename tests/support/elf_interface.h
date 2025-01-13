/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file elf_interface.h
 * @author Evan Stoddard
 * @brief Mock API for ELF writer file
 */

#ifndef ELF_INTERFACE_H_
#define ELF_INTERFACE_H_

#include "elf_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Mock declaration for opening a file
 * 
 * @param size Total size of ELF
 * @param file_ctx Optional pointer to file context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_iface_file_open(size_t size, void *file_ctx);

/**
 * @brief Mock delcaration for writing payload to file
 *
 * @param file_ctx Optional pointer to file context
 * @param offset Offset into file
 * @param buffer Pointer to write data to
 * @param len Length of buffer
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_iface_file_write(void *file_ctx, size_t offset, void *buffer, size_t len);

/**
 * @brief Mock declaration of closing file
 *
 * @param file_ctx Optional pointer to file context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_iface_file_close(void *file_ctx);

#ifdef __cplusplus
}
#endif
#endif /* ELF_INTERFACE_H_ */