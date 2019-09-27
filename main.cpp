#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "hash.h"
#include "com_t.h"
void hash_dir_all(char *path,hash & hash1,hash_type type,char * dirname=0,uint16_t bch_m=CONFIG_M,uint16_t bch_t=CONFIG_T) {
    DIR *pdir;
    struct dirent *ent;
    char child_path[512];
    pdir = opendir(path);
    if (pdir == NULL) {
        std::cout << "Open dir error! path is :" << path << std::endl;
        exit(-1);
    }
    //    std::cout<<"1"<<std::endl;
    while ((ent = readdir(pdir)) != NULL) {
        memset(child_path, 0, 512);
        if (ent->d_type & DT_DIR) { //if the ent is dir
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            sprintf(child_path, "%s/%s", path, ent->d_name);
            //std::cout<<child_path<<std::endl;
            hash_dir_all(child_path, hash1, type,dirname,bch_m,bch_t);
        } else {
            sprintf(child_path, "%s/%s", path, ent->d_name);
            //std::cout<<child_path<<std::endl;
            if (type == sha1_hash) {
                hash1.sha1_file_comp(ent->d_name, child_path);
            }
            if (type == ead_hash) {
                hash1.ecc_file_comp(ent->d_name,dirname, child_path,bch_m,bch_t);
            }
        }
    }
    closedir(pdir);
}

void disRE(int x, int y, char note, int (*cal)(int x, int y)) {   //int (*cal)(int x, int y)：函数指针变量
    std::cout << x << note << y << "的结果为：" << cal(x, y) << std::endl;
    return;
}
//加法


int main(int argc, char **argv) {

    DIR *dir;
    struct dirent *ptr;
    char dir_name[256];
    char filename[256];
    hash sha1_h;
//the second string represent director name, and the third string represent the device name.
    if (argc != 3) {
        std::cout << "error:parameter is not completed!" << std::endl;
        exit(0);
    }

    strcpy(dir_name, argv[1]);
    if ((dir = opendir(dir_name)) == NULL) {
        std::cout << "error:open director error!" << std::endl;
        exit(0);
    }

    {
        hash sha1_h;
        std::cout << "**************************************************"
                  << "sha1 test start!"
                  << "**************************************************" << std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        /*
        while (NULL != (ptr = readdir(dir))) {
            strcpy(filename, ptr->d_name);
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
//            std::cout<<"We will start to test:"<<filename<<std::endl;
            sha1_h.sha1_file_comp(filename, dir_name);
        }
        */
        //sha1_dir_all(dir_name,hash::sha1_file_comp);
        hash_dir_all(dir_name,sha1_h,sha1_hash);
        if ((dir = opendir(dir_name)) == NULL) {
            std::cout << "error:open director error!" << std::endl;
            exit(0);
        }
    }

    //this module is to test ecc
    {
        hash ecc(CONFIG_M,CONFIG_T);
        std::cout << "******************************************************************************"
                  << "ECC test start!,T=" << CONFIG_T
                  << "******************************************************************************" << std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        /* while (NULL != (ptr = readdir(dir))) {
           strcpy(filename, ptr->d_name);
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
            std::cout<<"We will start to test:"<<filename<<std::endl;
            ecc.ecc_file_comp(filename, argv[2], dir_name);
        }
*/
            hash_dir_all(dir_name,ecc,ead_hash,argv[2]);



        closedir(dir);
        if ((dir = opendir(dir_name)) == NULL) {
            std::cout << "error:open director error!" << std::endl;
            exit(0);
        }
    }

    //this module is to test ecc-T=4
    {
        uint16_t T = 4;
        hash ecc(CONFIG_M,T);
        std::cout << "******************************************************************************"
                  << "ECC test start! ,T=" << T
                  << "******************************************************************************" << std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        hash_dir_all(dir_name,ecc,ead_hash,argv[2],CONFIG_M,T);
        closedir(dir);
        if ((dir = opendir(dir_name)) == NULL) {
            std::cout << "error:open director error!" << std::endl;
            exit(0);
        }
    }
    //this module is to test ecc-T=8
    {
        uint16_t T = 8;
        hash ecc(CONFIG_M,T);
        std::cout << "******************************************************************************"
                  << "ECC test start! ,T=" << T
                  << "******************************************************************************" << std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        hash_dir_all(dir_name,ecc,ead_hash,argv[2],CONFIG_M,T);

        closedir(dir);
        if ((dir = opendir(dir_name)) == NULL) {
            std::cout << "error:open director error!" << std::endl;
            exit(0);
        }
    }

/*

    //this module is to test sha256
    {
        hash sha256_h;


        std::cout<<"**************************************************"
                 <<"sha256 test start!"
                 <<"**************************************************"<<std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        while(NULL != (ptr = readdir(dir)) ){
            strcpy(filename, ptr->d_name);
            if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
//            std::cout<<"We will start to test:"<<filename<<std::endl;
            sha256_h.sha256_file_comp(filename, dir_name);

        }
    }

    closedir(dir);
    if((dir = opendir(dir_name)) == NULL){
        std::cout<<"error:open director error!"<<std::endl;
        exit(0);
    }
*/

/*
    //this module is to test sha1
    {
        hash sha1_h;


        std::cout<<"**************************************************"
                 <<"sha1 test start!"
                 <<"**************************************************"<<std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        while(NULL != (ptr = readdir(dir)) ){
            strcpy(filename, ptr->d_name);
            if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
//            std::cout<<"We will start to test:"<<filename<<std::endl;
            sha1_h.sha1_file_comp(filename, dir_name);

        }
    }

    closedir(dir);
    if((dir = opendir(dir_name)) == NULL){
        std::cout<<"error:open director error!"<<std::endl;
        exit(0);
    }

*/
/*
//this module is to test md5
    {
        hash md5_h;


        std::cout<<"**************************************************"
                 <<"MD5 test start!"
                 <<"**************************************************"<<std::endl;
        std::cout << std::left << std::setw(120) << "Filename"
                  << std::left << std::setw(20) << "Total time(ms)"
                  << std::left << std::setw(20) << "Chunk number"
                  << std::left << std::setw(8) << "Average time(ms)" << std::endl;
        while(NULL != (ptr = readdir(dir)) ){
            strcpy(filename, ptr->d_name);
            if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
//            std::cout<<"We will start to test:"<<filename<<std::endl;
            md5_h.md5_file_comp(filename, dir_name);

        }
    }

*/

    closedir(dir);
    return 0;
}