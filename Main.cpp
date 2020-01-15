/*
 * Main.cpp
 *
 *  Created on: Feb 19, 2019
 *      Author: bio
 */
#include "basic.h"
#include "build.h"
#include "read.h"
#include "ExactMatch.h"
#include "Bit_sseInstruction.h"
#include <ctime>
#include "BitVec_index.h"
#include "print.h"
#include <bitset>


inline uint32_t icalc_OCCV(sFMindexv indxv,struct build_para para,char c,uint32_t pos)
{
	uint32_t offset;
	uint32_t (*p)(__m128i, __m128i);
	uint32_t (*q)(__m128i, __m128i , uint32_t, uint64_t *);
	switch(c)
	{
		case 'A' :
				offset = 0;
				p = calcVec_Anum;
				q = calcVec_Acut;
				break;
		case 'C' :
				offset = 1;
				p = calcVec_Cnum;
				q = calcVec_Ccut;
				break;
		case 'G' :
				offset = 2;
				p = calcVec_Gnum;
				q = calcVec_Gcut;
				break;
		case 'T' :
				offset = 3;
				p = calcVec_Tnum;
				q = calcVec_Tcut;
				break;
		default:
				offset = -1;
				break;
	}
	if(pos % para.occ_gap == 0)
	{
		return *(indxv.occa + (pos/para.occ_gap)*4 + offset);
	}
	else
	{
		uint32_t rslt = *(indxv.occa + (pos/para.occ_gap)*4 + offset);
		uint32_t remain = pos % para.occ_gap;
		uint32_t loop = remain/128 + 1;
		uint32_t index = (pos/para.occ_gap) * (para.occ_gap/128);
		for(uint32_t i = 0; i < loop; i++)
		{
			if(i == loop-1)
			{
				rslt += (*q)(indxv.bvh[index+i],indxv.bvl[index+i],remain,CUTVECL);
			}
			else
			{
				rslt += (*p)(indxv.bvh[index+i],indxv.bvl[index+i]);
			}
			remain -= 128;
		}
		if(offset == 0 && pos > indxv.dolpos && pos < (indxv.dolpos/para.occ_gap+1)*para.occ_gap)
		{
			rslt--;
		}
		return rslt;
	}
}

int main2()
{
	init_CUTVECL();
	init_CUTVECH();
	for(uint32_t i = 0; i < 65; i++)
	{
	}
//	__m128i mtemp;
//	for(uint32_t i = 0; i < 128; i++)
//	{
//		mtemp = get_cutm128i(i,CUTVECL);
//		print128_bit(mtemp);
//	}
	getchar();
	srand((unsigned)time(NULL));
	uint8_t array[16] = {0};
	for(int i = 0; i < 16; i++)
	{
		array[i] = 194;
	}
	__m128i m1 = *((__m128i*)array);
	__m128i m2;
    int res1 = popcnt_sse_u64(m1);  //这个正确
    cout << "answer1:" << res1 << endl;
    print128_bit(m1);
//    m2 = _mm_not_si128(m1);
    print128_bit(m2);

}


int main(int argc, char** argv)
{
	init_CUTVECL();
	init_CUTVECH();
	struct build_para para;
	char c;
	for(int32_t i=1;i<argc;i=i+2)
	{
		if(argv[i][0]=='-'&&argv[i][1]=='h')//Method
		{
			para.ref_path=argv[i+1];
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='s')//sa_gap
		{
			para.sa_gap=atoi(argv[i+1]);
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='o')//sa_gap
		{
			para.occ_gap=atoi(argv[i+1]);
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='l')//level
		{
			para.level=atoi(argv[i+1]);
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='t')//thread_num
		{
			para.thread_num=atoi(argv[i+1]);
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='m')//max_len
		{
			para.max_len=atoi(argv[i+1]);
		}
		else if(argv[i][0]=='-'&&argv[i][1]=='c')//max_len
		{
			c=*argv[i+1];
		}
	}
	char *dir = "./index";
	if(access(dir,0) == -1)
	{
		printf("%s not exist.\n",dir);
		mkdir(dir, 0755);
	}
	struct timeval tvs,tve;
	build(para);

	build_occA(para);
	char index[] = "./index";
	sFMindex prm;
	read_bfile2mem(index,&prm,0);

	sFMindexv sidxv;
	build_bVec(&sidxv);
	build_occAVec(&sidxv,para);
	uint32_t *occav = NULL;
	occav = (uint32_t *)read_binfile("./index/OCCAV");

//	for(uint32_t i = 0; i < prm.occ_num; i++)
//	{
//		if(prm.occa[i] != occav[i])
//		{
//			cout << "Different position is:" << i << endl;
//			break;
//		}
//	}
//	cout << "OCCA == OCCAV" << endl;
	sidxv.occa = occav;
	cout << "c = " << c << endl;
	uint32_t ret, retv;
	double span;
//	gettimeofday(&tvs,NULL);
//	for(uint32_t i = 0; i < strlen(prm.b); i++)
//	{
//		ret = calc_OCC(prm,para,c,i);
//
//	}
//	gettimeofday(&tve,NULL);
//	span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
//	cout <<"time of construct calc_OCC is: "<<span<<endl;

	gettimeofday(&tvs,NULL);
	for(uint32_t i = 0; i < strlen(prm.b); i++)
	{
		retv = icalc_OCCV(sidxv, para, c, i);
//		retv = calc_OCCVu(sidxv, para, c, i, 1);
	}
	gettimeofday(&tve,NULL);
	span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
	cout <<"time of construct calc_OCCV is: "<<span<<endl;

//	cout << "test main 55" << endl;

	//test LFmapping
//	char c = prm.b[0];
//	char pristr[128] = {0};
//	pristr[0] = c;
//	int tmp;
//	tmp = LF_Mapping(prm,para,c,0);
//	c = prm.b[tmp];
//	for(uint32_t i = 1; i < strlen(prm.b); i++)
//	{
//		pristr[i] = c;
//		if(i == strlen(prm.b)-1)
//		{
//			break;
//		}
//		tmp = LF_Mapping(prm,para,c,tmp);
//		c = prm.b[tmp];
//	}
//	for(int i = strlen(pristr) - 1; i >= 0; i--)
//	{
//		printf("%c",pristr[i]);
//	}
//	printf("\n");
	//test LFmapping end

	//test EXACTMATCH
//	char * f1;
//	uint32_t  *p = NULL;
//	char *str = "TTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAATTTTATTGACTTAGGTCACTAAATACTTTAACCAA";
//	int cnt = 0;
//	for(int i = 0; i < strlen(str); i++)
//	{
//		f1 = get_queryfrag(i,str);
//		p = calc_SArange(prm,para.occ_gap,f1);
//		for(int j = p[0]; j <= p[1];j++)
//		{
//			if(prm.sa[j] == i)
//			{
//				cnt++;
//				break;
//			}
//		}
//	}
//	printf("cnt is : %d\n",cnt);


//	uint32_t rslt;
//	uint32_t pos = atoi(argv[11]);
//	rslt = calc_SA(prm,para,pos);
//	printf("SA %d is : %ld\n",pos,rslt);


//	printf("SArange main \n");
//	printf("%d %d\n",p[0],p[1]);
//	for(int i = p[0]; i <= p[1]; i++)
//	{
//		printf("%d\n",prm.sa[i]);
//	}
	return 0;
}

