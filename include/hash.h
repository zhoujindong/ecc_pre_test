//
// Created by victor on 4/5/17.
//

#ifndef PRE_ECC_TEST_HASH_H
#define PRE_ECC_TEST_HASH_H



#include <iostream>
#include <algorithm>
#include <set>
#include <string>

#include "bch.h"
#include "tpdef.h"
//#include "md5.h"
//#include "sha256.h"

#define READ_LENGTH 4096
#define CONFIG_M 13
#define CONFIG_T 2
enum hash_type {ead_hash,sha1_hash};
class hash{


public:
    static int calAdd(int x, int y);
    uint8_t md5_file_comp(char *filename, char *dir_name);
    uint8_t sha256_file_comp(char *filename, char *dir_name);
    uint8_t sha1_file_comp(char *filename, char *dir_name);
    uint8_t ecc_file_comp(char *filename, char *dev_name, char *dir_name,uint16_t bch_m=CONFIG_M,uint16_t bch_t=CONFIG_T);
    uint8_t string_convert_uint64(uint8_t *content, uint64_t *num); //default length of content is 4
    hash(uint16_t m,uint16_t t);
    hash();
    ~hash();

public:
    uint64_t chunk_num;
    double time_total;
    double time_aver;

private:
    std::set<std::string> list;
    void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);
    struct bch_control *bch=NULL;
    uint64_t chunk_not_dup;

};

#endif //PRE_ECC_TEST_HASH_H