// Created by victor on 4/5/17.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <aio.h>
#include <openssl/sha.h>
#include <openssl/md5.h>


#include "hash.h"
#include "com_t.h"

hash::hash() {
    chunk_num = 0;
    time_total = 0.0;
    time_aver = 0.0;
    chunk_not_dup = 0;
    //bch = init_bch(CONFIG_M, CONFIG_T, 0);
    //bch2 = init_bch(CONFIG_M, CONFIG_T_2, 0);

}
hash::hash(uint16_t m,uint16_t t) {
    chunk_num = 0;
    time_total = 0.0;
    time_aver = 0.0;
    chunk_not_dup = 0;
    bch = init_bch(m, t, 0);
    //bch2 = init_bch(CONFIG_M, CONFIG_T_2, 0);

}
hash::~hash() {
    free_bch(bch);
    if(time_total > 0) {
        time_aver = time_total / chunk_num;
        std::cout <<std::endl<< "**************************"<<"This option's time performance***********************"<<std::endl;
        std::cout << "The total time is " << time_total <<"ms"<< std::endl;
        std::cout << "The chunk number is " << chunk_num << std::endl;
        if(chunk_not_dup > 0){
            std::cout << "The dedupe rate is " <<  (chunk_num - chunk_not_dup) * 100.0 / chunk_num <<"%"<<std::endl;
            std::cout << "The not dedupe chunk number is " << chunk_not_dup << std::endl;
        }

        std::cout << "The average time is " << time_aver <<"ms"<< std::endl<<std::endl;
    }
}

uint8_t hash::ecc_file_comp(char *filename, char *dev_name, char *dir_name, uint16_t bch_m,uint16_t bch_t) {
    int ecc_bytes=bch->ecc_bytes;
    FILE *fp;
    uint8_t chk_cont[4097];
    uint64_t another_chk = 0;
    struct aiocb64 myaio;
    struct aiocb64 *cblist[1];
    double stat_time = 0.0, fin_time = 0.0;
    uint8_t hv[20];
    char result[256];
    std::string mid_str;
    cp_t time_cpt;
    char file_dir[256];
//    double xor1=0.0, xor2=0.0;
    /* zjd 2019-4-30
    int fd = open(dev_name, O_RDWR|O_LARGEFILE);

    if(fd==-1) //add by zwd
    {
        std::cout<<"open dev error!"<<std::endl;
        exit(0);
    }

    bzero( (char *)cblist, sizeof(cblist) );
    bzero((char *)&myaio,sizeof(struct aiocb64));

    myaio.aio_buf=malloc(4097);
    if(!myaio.aio_buf)
        perror("malloc");
    myaio.aio_fildes = fd;
    myaio.aio_nbytes = 4096;
    myaio.aio_offset = 0;
    aio_write64(&myaio);
    while (EINPROGRESS == aio_error64(&myaio )) ; //EINPROGRESS represent the request are not complete.
    close(fd);

    //sprintf(file_dir, "%s/%s", dir_name, filename);
    close(fd);
          */
    sprintf(file_dir,"%s",dir_name);
    if((fp = fopen(file_dir, "r")) == NULL){
        std::cout<<"Open file error!The file name is: "<<filename<<std::endl;
        return 0;
    }
    while(!feof(fp)){
        memset(chk_cont, 0, READ_LENGTH);
        if(fread(chk_cont, sizeof(char), READ_LENGTH, fp) == 0)
            break;
/*        if(fgets((char *)chk_cont, READ_LENGTH, fp) == NULL ){
//            std::cout<<"The file: "<<filename<<" ecc computation is already finished"<<std::endl;
            break;
        }*/
//bch func
        memset(hv, 0, 20);
        memset(result, 0, 256);
        for(int i=0;i<8;i++){
            encode_bch(bch, chk_cont+512*i, READ_LENGTH/8, hv);
            ByteToHexStr(hv, result+ecc_bytes*2*i, ecc_bytes);
        }
        //encode_bch(bch, chk_cont, READ_LENGTH, hv);
        //ByteToHexStr(hv, result, 24);
//        std::cout<<strlen((char *)result)<<std::endl<<result<<std::endl;
        result[ecc_bytes*2*8]='/0';
        mid_str = result;

        if(list.find(mid_str) == list.end()){
            chunk_not_dup++;
            list.insert(mid_str);
        }
//bch end
        double elps_time = 0.0;
        uint64_t xor_num = 0;
        int i;
        cblist[0] = &myaio;
        stat_time = time_cpt.get_time();
//read overhead
//        xor1 = time_cpt.get_time();
/* zjd 2019-4-30
        aio_read64(&myaio);
        aio_suspend64(cblist, 1, NULL);
*/
//        xor2 = time_cpt.get_time();
//        std::cout<<"xor2 - xor1 = "<<(xor2-xor1)*1000<<std::endl;
//read over
//xor start

        for(i=0; i < 512; i++) {

            string_convert_uint64(chk_cont + i * 8, &xor_num);
            another_chk = another_chk ^ xor_num;
        }

//xor finish
        fin_time = time_cpt.get_time();
        elps_time = (fin_time - stat_time) * 1000;//ms
//        std::cout<<"finish - start = "<<elps_time<<std::endl;
        time_cpt.cp_all(elps_time);
    }
    fclose(fp);
    time_total += time_cpt.time_total;
    chunk_num += time_cpt.chunk_num;
    time_cpt.cp_aver(filename);

    return 1;
}

uint8_t hash::md5_file_comp(char *filename, char *dir_name) {
    FILE *fp;
    uint8_t chk_cont[4096];
    uint8_t hv[17] = {0};
    char result[33] = {0};
    double stat_time = 0.0, fin_time = 0.0;
    std::string mid_str;
    cp_t time_cpt;
    char file_dir[256];

    sprintf(file_dir, "%s/%s", dir_name, filename);
    if((fp = fopen(file_dir, "r")) == NULL){
        std::cout<<"Open file error!The file name is:"<<filename<<std::endl;
        return 0;
    }
    while(!feof(fp)){
        memset(chk_cont, 0, READ_LENGTH);
        if(fread(chk_cont, sizeof(char), READ_LENGTH, fp) == 0)
            break;
        double elps_time = 0.0;
        memset(hv, 0, 17);
        memset(result, 0, 33);
        stat_time = time_cpt.get_time();
//md5 func

        MD5((unsigned char *)chk_cont, (size_t)4096, (unsigned char *)hv);

//md5 finish
        fin_time = time_cpt.get_time();
        elps_time = (fin_time - stat_time) * 1000;//ms
        time_cpt.cp_all(elps_time);

        ByteToHexStr(hv, result, 16);
        mid_str = result;
        if(list.find(mid_str) == list.end()){
            chunk_not_dup++;
            list.insert(mid_str);
        }
    }
    fclose(fp);
    time_cpt.cp_aver(filename);
    time_total += time_cpt.time_total;
    chunk_num += time_cpt.chunk_num;

    return 1;
}

uint8_t hash::sha256_file_comp(char *filename, char *dir_name) {
    FILE *fp;
    uint8_t chk_cont[4097];

    uint8_t hv[33] = {0};
    char result[65] = {0};
    double stat_time = 0.0, fin_time = 0.0;
    std::string mid_str;
    cp_t time_cpt;
    char file_dir[256];

    //sprintf(file_dir, "%s/%s", dir_name, filename);
    sprintf(file_dir, "%s/%s", dir_name, filename);
    if((fp = fopen(file_dir, "r")) == NULL){
        std::cout<<"Open file error!The file name is:"<<file_dir<<std::endl;
        return 0;
    }
    while(!feof(fp)){
        memset(chk_cont, 0, READ_LENGTH);
        if(fread(chk_cont, sizeof(char), READ_LENGTH, fp) == 0)
            break;
        double elps_time = 0.0;
        memset(hv, 0, 33);
        memset(result, 0, 65);
        stat_time = time_cpt.get_time();
//sha256 func

        SHA256((unsigned char *)chk_cont, (size_t)4096, (unsigned char *)hv);
/*        sha256_init(&ctx);
        sha256_hash(&ctx, chk_cont, (uint32_t)strlen((char *)chk_cont));
        sha256_done(&ctx, hv);*/

//sha256 finish
        fin_time = time_cpt.get_time();
        elps_time = (fin_time - stat_time) * 1000;//ms
        time_cpt.cp_all(elps_time);

        ByteToHexStr(hv, result, 32);
        mid_str = result;
        if(list.find(mid_str) == list.end()){
            chunk_not_dup++;
            list.insert(mid_str);
        }


    }
    fclose(fp);
    time_cpt.cp_aver(filename);
    time_total += time_cpt.time_total;
    chunk_num += time_cpt.chunk_num;
    return 1;
}

uint8_t hash::sha1_file_comp(char *filename, char *dir_name) {
    FILE *fp;
    uint8_t chk_cont[4096];
    uint8_t result[21] = {0};
    char hex_result[41] = {0};
    double stat_time = 0.0, fin_time = 0.0;
    std::string mid_str;
    cp_t time_cpt;
    char file_dir[256];

    //sprintf(file_dir, "%s/%s", dir_name, filename);
    sprintf(file_dir,"%s",dir_name);

    if((fp = fopen(file_dir, "r")) == NULL){
        std::cout<<"Open file error!The file name is:"<<file_dir<<std::endl;
        return 0;
    }
    while(!feof(fp)){
        memset(chk_cont, 0, READ_LENGTH);
        if(fread(chk_cont, sizeof(char), READ_LENGTH, fp) == 0)
            break;
        double elps_time = 0.0;

        memset(result, 0, 21);
        memset(hex_result, 0, 41);
        stat_time = time_cpt.get_time();
//sha1 func
        for(int i=0;i<10000;i++){
        SHA1((unsigned char *)chk_cont, (size_t)READ_LENGTH, (unsigned char *)result);
        }
//sha1 finish
        fin_time = time_cpt.get_time();
        elps_time = (fin_time - stat_time) * 1000;//ms
        time_cpt.cp_all(elps_time);

        ByteToHexStr(result, hex_result, 20);
        mid_str = hex_result;
        if(list.find(mid_str) == list.end()){
            chunk_not_dup++;
            list.insert(mid_str);
        }
    }
    fclose(fp);
    time_cpt.cp_aver(file_dir);
    time_total += time_cpt.time_total;
    chunk_num += time_cpt.chunk_num;
    return 1;
}

uint8_t hash::string_convert_uint64(uint8_t *content, uint64_t *num) {
    uint64_t mid;
    mid = (uint64_t)content[0];
    *num = *num ^ ((mid<<56)>>56);
    mid = (uint64_t)content[1];
    *num = *num ^ ((mid<<56)>>48);
    mid = (uint64_t)content[2];
    *num = *num ^ ((mid<<56)>>40);
    mid = (uint64_t)content[3];
    *num = *num ^ ((mid<<56)>>32);
    mid = (uint64_t)content[4];
    *num = *num ^ ((mid<<56)>>24);
    mid = (uint64_t)content[5];
    *num = *num ^ ((mid<<56)>>16);
    mid = (uint64_t)content[6];
    *num = *num ^ ((mid<<56)>>8);
    mid = (uint64_t)content[7];
    *num = *num ^ (mid<<56);
    return 1;
}

void hash::ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
            dest[i * 2] = highByte + 0x07;
        else
            dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}
