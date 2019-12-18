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

int main(int argc, char** argv)
{
	struct build_para para;
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
	}
	char *dir = "./index";
	if(access(dir,0) == -1)
	{
		printf("%s not exist.\n",dir);
		mkdir(dir, 0755);
	}
	struct timeval tvs,tve;
	gettimeofday(&tvs,NULL);
	build(para);
	build_occA(para);
	char index[] = "./index";
	ReadMem prm;
	read_bfile2mem(index,&prm,0);
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


	gettimeofday(&tve,NULL);
	double span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
	cout <<"time of construct FMindex is: "<<span<<endl;
	return 0;

//	printf("SArange main \n");
//	printf("%d %d\n",p[0],p[1]);
//	for(int i = p[0]; i <= p[1]; i++)
//	{
//		printf("%d\n",prm.sa[i]);
//	}

}


int main1(int argc, char** argv)
{
	int a[] = {1,2,3,4};
	int *p = a;
	printf("%d\n",*(p));
	printf("%d\n",*(p+1));

//	if(argc != 2)
//	{
//		printf("Input Number Error!\n");
//		exit(-1);
//	}
//	char *File_path = argv[1];
//	printf("%s\n",File_path);
//	read_binfile(File_path);
//	return 0;
}
