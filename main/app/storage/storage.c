/*
 * storage.c
 *
 *  Created on: Feb 8, 2023
 *      Author: Admin
 */


#include "storage.h"
#include "stdio.h"
#include <dirent.h>
#include "string.h"

void storage_init(){
	storage_mount(STORAGE_PATH);
}
// void p_storage_deinit(){
// 	storage_deinit(STORAGE_PATH);
// }


int file_find(char* file_name){
	DIR *d;
	struct dirent *dir;

	d = opendir("/spiffs");

	if(d != NULL){
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
            if(strcmp(file_name,dir->d_name) == 0){
            	return 1;
            }
        }
        closedir(d);
	}
	else{
		printf("Not open dir\r\n");
	}
	return 0;
}
