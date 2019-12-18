/*
 * ExactMatch.h
 *
 *  Created on: 2019年7月13日
 *      Author: jupiter
 */

#ifndef EXACTMATCH_H_
#define EXACTMATCH_H_

#include "basic.h"
#include "read.h"
#include "build.h"

char * get_queryfrag(uint32_t k,char *query);
uint32_t calc_C(ReadMem mem,char c);
uint32_t calc_OCC(ReadMem mem,struct build_para para,char c,uint32_t pos);
uint32_t LF_Mapping(ReadMem mem,struct build_para para,char c,uint32_t pos);
uint32_t* calc_SArange(ReadMem mem,struct build_para para,char *read);
uint32_t calc_SA(ReadMem mem,struct build_para para,uint32_t pos);

#endif /* EXACTMATCH_H_ */
