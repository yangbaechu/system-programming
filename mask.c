#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<sys/wait.h>
#define MAX_LENGTH 100000
typedef struct data
{
	char type[30];
	int cut_length_back;
	int cut_length_front;
}DATA;
typedef struct med
{
	double lat; 
	double lng; 
	double dis; 
	char name[200]; 
	char addr[1000]; 
	char amnt[50]; 
	char time[100]; 
	char date[100]; 
}MED;
double distance(double a,double b)
{
	return sqrt(pow(a,2)+pow(b,2));
}
int compare(const void* a,const void* b)
{
	double v1,v2;
	v1=((MED *)a)->dis;
	v2=((MED *)b)->dis;
	if(v1>v2)return -1;
	else if(v1==v2)return 0;
	else return 1;
}
int c_system(char* command)
{
    int status;
    switch(fork())
    { 
	case 0:
	    execl("/bin/bash","bin/bash","-c",command,NULL);
	    exit(0);
        case -1:
	    perror("fork error");
	    exit(0);
	default:
	    wait(&status);
	    break;  
    }
    return status;
}

int downloader(void)
{
    char url1[1000];
    char url2[1000];
    char gu[1000];
    printf("탐색할 구 이름을 입력하세요\n");
    scanf("%s",gu);
    getchar();
    strcpy(url1, "wget --header='Accpet-Charset: utf-8' https://8oi9s0nnth.apigw.ntruss.com/corona19-masks/v1/storesByAddr/json?address=서울특별시%20");
    strcat(url1, gu);
    c_system(url1);
    strcpy(url2, "mv 'json?address=서울특별시 ");
    strcat(url2, gu);
    strcat(url2, "' ./med.txt");
    c_system(url2);
    c_system("find . ! -name '*.txt' ! -name '*.c' -type f -exec rm -f {} +");
    return 0;
}

char* strtok_str(char* str,char key[])
{  
    int i,j;
    int chsize=strlen(key);
    char* last=(char*)malloc(1000);
    last=str;
    printf("sizeofstr:%lu\n",strlen(str));
    for(i=0;i<strlen(str)-chsize;i++)
    {
	char* temp=malloc(strlen(str));
        for(j=0;j<chsize;j++)
	{
	    temp[j]=str[i+j];
             	
	    if(strcmp(temp,key)==0)
	    {
		 printf("found!\n");
	         last=(str+i+j+1);
	         printf("last:%s\n",last);
		 return last;
	    }
	}
    }
    return last; 
}


int main()
{
        int line_cnt1 = 1, i, flag = 0, add4_size = 0, dis = 0, num = 0;
        char buf[MAX_LENGTH];
	char* srch = NULL;
	double nowlat,nowlng;
	DATA d[10] = {
		{"주소1: ",3,8},
		{"주소2: ",3,8},
		{"주소3: ",3,8},
		{"주소4: ",3,8},
		{"날짜: ",3,14},
		{"위도: ",2,6},
		{"경도: ",2,6},
		{"이름: ",3,8},
		{"남은 수량: ",3,15},
		{"재고 입고 시간: ",3,12}
	};
	MED m[1000];
	
    char add[10000];
    char url[10000];
    downloader();
    printf("주소를 입력해주세요: ");
    scanf("%[^\n]",add);
    strcpy(url,"wget \"http://api.vworld.kr/req/address?service=address&request=getcoord&version=2.0&crs=epsg:4326&address=");
    strcat(url,add);
    strcat(url,"&refine=true&simple=false&format=xml&type=road&key=13735BFB-0D28-3552-B82D-91167E239A73\" -O test.txt");
    printf("%s",url);
    c_system(url);

    char buff[1000];
    char* ptr;
    FILE* fpr=fopen("./test.txt","r");
    fgets(buff,sizeof(buff),fpr);
	
    FILE* fpw=fopen("./xy.txt","w");
    ptr=strtok_str(buff,"<x>");
    ptr=strtok_str(ptr,"CDATA[");
    ptr=strtok(ptr,"]");
    fputs(ptr,fpw);
    
    double x;
    x  = atof(ptr);
   
    fputs(",",fpw);
    ptr=strtok(NULL,"#");
    ptr=strtok_str(ptr,"<y>");
    ptr=strtok_str(ptr,"CDATA[");
    ptr=strtok(ptr,"]");

    fputs(ptr,fpw);
      
    double y;
    y = atof(ptr);
	FILE* fp1;
	FILE* fp2;
	FILE* fp3;
	fp1=fopen("med.txt", "rt");
	fp2=fopen("med_refined.txt", "wt");
	fp3=fopen("med_sorted.txt","wt");
	if (fp1 == NULL || fp2 == NULL || fp3 == NULL)
	{
		printf("파일을 열수 없습니다!\n");
		return 0;
	}
    nowlat = y;
    nowlng = x;	

	while (fgets(buf, MAX_LENGTH, fp1) != NULL)
	{
		if (flag == 0 && (srch = strstr(buf, "address")) != NULL)
		{
			srch[strlen(srch) - 3] = 0;
			flag = 1;
			continue;
		}
		if ((srch = strstr(buf, "addr")) != NULL)
		{
			fprintf(fp2, "%d\n", line_cnt1);
			line_cnt1++;
			for (i = 0;i < 7;i++)
			{
				
				if (i == 0) {
					fprintf(fp2, "%s", d[0].type);
					srch[strlen(srch) - d[0].cut_length_back] = 0;
					srch = srch + d[i].cut_length_front;
					char* ptr = strtok(srch, " ");
					fprintf(fp2, "%s\n", ptr);
					strcpy(m[line_cnt1-2].addr, ptr);
					
					fprintf(fp2, "%s", d[1].type);
					fprintf(fp2, "%s\n", ptr = strtok(NULL, " "));
					strcat(m[line_cnt1-2].addr, " ");
					strcat(m[line_cnt1-2].addr, ptr);

					fprintf(fp2, "%s", d[3].type);
					char* temp = strtok(NULL, "(");
					fprintf(fp2, "%s\n", temp);

					fprintf(fp2, "%s", d[2].type);
					ptr = strtok(NULL, "123456789),");
          			fprintf(fp2, "%s\n", ptr);
          			strcat(m[line_cnt1-2].addr, " ");
          			strcat(m[line_cnt1-2].addr, ptr);
          			strcat(m[line_cnt1-2].addr, " ");
          			strcat(m[line_cnt1-2].addr, temp);

					fgets(buf, 100, fp1);
					fgets(buf, 100, fp1);
					srch = strstr(buf, "created_at");
				}
				else {
					fprintf(fp2, "%s", d[i+3].type);
					srch[strlen(srch) - d[i+3].cut_length_back] = 0;
					fprintf(fp2, "%s\n", srch + d[i+3].cut_length_front);
					switch(i+3)
					{
						case 4 : strcpy(m[line_cnt1-2].date, srch + d[i+3].cut_length_front);break;
						case 5 : m[line_cnt1-2].lat=atof(srch + d[i+3].cut_length_front);break;
						case 6 : m[line_cnt1-2].lng=atof(srch + d[i+3].cut_length_front);break;
						case 7 : strcpy(m[line_cnt1-2].name, srch + d[i+3].cut_length_front);break;
						case 8 : strcpy(m[line_cnt1-2].amnt, srch + d[i+3].cut_length_front);break;
						case 9 : strcpy(m[line_cnt1-2].time, srch + d[i+3].cut_length_front);break;
					}
					fgets(buf, 100, fp1);
				} 
			}
			fprintf(fp2, "\n");
			m[line_cnt1-2].dis=distance(m[line_cnt1-2].lat-nowlat,m[line_cnt1-2].lng-nowlng);
		}
	}
	qsort(m,1000,sizeof(m[0]),compare);
	fprintf(fp3, "%s에서 가까운 순서대로 정렬한 결과입니다.\n\n", add);
	for(i=line_cnt1-2;num<10;i--)
	{
                if(!(strcmp("empty", m[i].amnt)&&strcmp("break", m[i].amnt)))
                {
                	continue;
                } 	 	 
	        num++;	
		dis = m[i].dis*110000;
                dis -= dis%10;	
		fprintf(fp3, "%d\n", num);
	
		fprintf(fp3, "거리: 약 %dm\n", dis);
	
		fprintf(fp3, "%s%s\n", d[7].type,m[i].name);
	
		fprintf(fp3, "%s%s\n", d[4].type,m[i].date);
	
		fprintf(fp3, "%s%s\n", d[8].type,m[i].amnt);

		fprintf(fp3, "%s%s\n", d[9].type,m[i].time);
	
		fprintf(fp3, "주소: %s\n\n", m[i].addr);
	}
	printf("med_sorted.txt에 %d개의 결과를 출력하였습니다!\n", num);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}
