/*
 * ExactMatch.cpp
 *
 *  Created on: 2019年7月13日
 *      Author: jupiter
 */

#include "ExactMatch.h"

char * get_queryfrag(uint32_t k,char *query)
{
	int32_t len = strlen(query);
	char *frag = NULL;
	frag = (char *)malloc(len*sizeof(char));
	memset(frag,0,len);
	strcpy(frag,query+k);
	return frag;
}

uint32_t calc_C(sFMindex mem,char c)
{
	uint32_t rslt;
	switch(c)
	{
		case 'A' :
			rslt = 0;
			break;
		case 'C' :
			rslt = mem.c[0];
			break;
		case 'G' :
			rslt = mem.c[0] + mem.c[1];
			break;
		case 'T' :
			rslt = mem.c[0] + mem.c[1] + mem.c[2];
			break;
		default:
			rslt = -1;
			break;
	}
	return rslt;
}

uint32_t calc_offset(char c)
{
	uint32_t offset;
	switch(c)
	{
		case 'A' :
				offset = 0;
				break;
		case 'C' :
				offset = 1;
				break;
		case 'G' :
				offset = 2;
				break;
		case 'T' :
				offset = 3;
				break;
		default:
				offset = -1;
				break;
	}
	return offset;
}

uint32_t calc_OCC(sFMindex mem,struct build_para para,char c,uint32_t pos)
{
	//calc the occ_array  input : pos(pos in bseq) gap(every gap pos save a occa)
	uint32_t offset = calc_offset(c);
	if(pos % para.occ_gap == 0)
	{
		return *(mem.occa + (pos/para.occ_gap)*4 + offset);
	}
	else
	{
		uint32_t rslt = *(mem.occa + (pos/para.occ_gap)*4 + offset);
		uint32_t len = pos > para.occ_gap ? pos - pos % para.occ_gap : 0;
		for(uint32_t i = 0; i < pos % para.occ_gap; i++)
		{
			if(mem.b[len+i] == c)
			{
				rslt++;
			}
		}
		return rslt;
	}
}

uint32_t LF_Mapping(sFMindex mem,struct build_para para,char c,uint32_t pos)
{
	//last first mapping
	return calc_C(mem,c) + calc_OCC(mem,para,c,pos) + 1;
}

uint32_t LF_Mapping_l(sFMindex mem,struct build_para para,char c,uint32_t pos)
{
	//last first mapping
	return calc_C(mem,c) + calc_OCC(mem,para,c,pos) + 1;
}

uint32_t LF_Mapping_h(sFMindex mem,struct build_para para,char c,uint32_t pos)
{
	//last first mapping
	return calc_C(mem,c) + calc_OCC(mem,para,c,pos+1) + 1 - 1;
}



uint32_t* calc_SArange(sFMindex mem,struct build_para para,char *read)
{
	char ch;
	int32_t i = strlen(read) - 1;
	ch = read[i];
	uint32_t C[5];
	C[0] = 0;
	C[1] = C[0] + mem.c[0];
	C[2] = C[1] + mem.c[1];
	C[3] = C[2] + mem.c[2];
	C[4] = C[3] + mem.c[3];
	uint32_t offset;
	uint32_t *p = (uint32_t *)malloc(sizeof(uint32_t)*2);
	offset = calc_offset(ch);
	p[0] = 0;
	p[1] = strlen(mem.b)-1;
	i--;
	while(p[0] <= p[1] && i >= 0)
	{
		ch = read[i];
		p[0] = LF_Mapping_l(mem,para,ch,p[0]);
		p[1] = LF_Mapping_h(mem,para,ch,p[1]);
		i--;
	}
	return p;
}

uint32_t calc_SA(sFMindex mem,struct build_para para,uint32_t pos)
{
	if(pos % para.sa_gap == 0)
	{
		return mem.sa[pos/para.sa_gap];
	}
	else
	{
		uint32_t tmp = pos;
		uint32_t cnt = 0;
		char ch = mem.b[tmp];
		while(tmp % para.sa_gap != 0)
		{
			if(ch == '$')
			{
				return cnt;
			}
			tmp = LF_Mapping(mem,para,ch,tmp);
			ch = mem.b[tmp];
			cnt++;
		}
		return mem.sa[tmp/para.sa_gap] + cnt;
	}
}

int mainem(int argc, char** argv)
{
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

	uint32_t *occav = NULL;
	occav = (uint32_t *)read_binfile("./index/OCCAV");

	cout << "c = " << c << endl;
	uint32_t ret, retv;
	double span;

	gettimeofday(&tvs,NULL);
	for(uint32_t i = 0; i < strlen(prm.b); i++)
	{
		ret = calc_OCC(prm,para,c,i);
		if(i == 5902)
		{
			cout << ret << endl;
		}
	}
	gettimeofday(&tve,NULL);
	span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
	cout <<"time of construct calc_OCC is: "<<span<<endl;

	return 0;
}
