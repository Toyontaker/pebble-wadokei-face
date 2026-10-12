#include <stdio.h>
#include "../src/c/wadokei.h"
static void hm(char*b,float m){ int t=(int)(m+0.5f); while(t<0)t+=1440; snprintf(b,8,"%d:%02d",t/60%24,t%60);}
int main(void){
  struct { const char* label; int yday; const char* ref; } d[] = {
    {"2026-03-20",78,"5:45/17:53"}, {"2026-06-21",171,"4:25/19:00"},
    {"2026-10-12",284,"5:43/17:14"}, {"2026-12-22",355,"6:47/16:32"} };
  for(int i=0;i<4;i++){
    for(int edo=0;edo<2;edo++){
      WaDay w; wadokei_day(d[i].yday,35.6895f,139.6917f,540,edo,&w);
      char a[8],b[8],c[8],e[8]; hm(a,w.sunrise);hm(b,w.sunset);hm(c,w.ake);hm(e,w.kure);
      printf("%s %s 日の出/入 %s/%s (暦 %s)  明六 %s 暮六 %s  昼一刻 %.0f分 夜一刻 %.0f分\n",
        d[i].label, edo?"江戸":"日出", a,b,d[i].ref,c,e,(w.kure-w.ake)/6,(1440-(w.kure-w.ake))/6);
    }
  }
  WaDay w; wadokei_day(284,35.6895f,139.6917f,540,0,&w);
  for(int m=0;m<1440;m+=60){ Koku k; wadokei_koku(&w,m,&k); char s[8]; hm(s,m);
    printf("%s %s%s (%sの刻) len=%.0f\n", s, wadokei_name(k.index), k.half?"半":"", wadokei_eto(k.index), k.length);}
  return 0;}
