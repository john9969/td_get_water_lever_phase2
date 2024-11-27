/*
 * file_storage.h
 *
 *  Created on: Feb 8, 2023
 *      Author: Admin
 */

#ifndef MAIN_COMPONENTS_FILE_STORAGE_FILE_STORAGE_H_
#define MAIN_COMPONENTS_FILE_STORAGE_FILE_STORAGE_H_


#include "stdio.h"
#include "stdlib.h"

#define STORAGE_PATH "/spiffs"

void storage_mount(const char* path);
void storage_unmount(const char* path);

#endif /* MAIN_COMPONENTS_FILE_STORAGE_FILE_STORAGE_H_ */
