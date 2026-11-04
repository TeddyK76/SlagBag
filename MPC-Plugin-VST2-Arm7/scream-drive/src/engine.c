#include "engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define N 14
static const char *keys[N]={"mode","drive","bias","output","low_cut","low","mid","high","high_cut","body_freq","body_res","body_mix","mix","bypass"};
static const float defs[N]={0,25,0,0,20,0,0,0,20000,180,1,0,100,0};
typedef struct {float p[N],lp[2],hp[2],body[2],body2[2];} Inst;
static float cl(float x,float a,float b){return x<a?a:(x>b?b:x);}
static int idx(const char*k){for(int i=0;i<N;i++)if(!strcmp(k,keys[i]))return i;return -1;}
static void *create(const char*d){(void)d;Inst*s=calloc(1,sizeof(*s));if(s)memcpy(s->p,defs,sizeof(defs));return s;}
static void destroy(void*p){free(p);}
static void setp(void*p,const char*k,const char*v){int i=idx(k);if(i>=0)((Inst*)p)->p[i]=strtof(v,0);}
static int getp(void*p,const char*k,char*b,int n){int i=idx(k);if(i<0)return 0;snprintf(b,n,"%.6g",((Inst*)p)->p[i]);return 1;}
static float sat(float x,int m,float d,float bias){
 float z=x*d+bias;
 switch(m){
 case 0:return tanhf(z*.75f)/tanhf(.75f);
 case 1:return .82f*tanhf(z)+.18f*sinf(z*2.1f);
 case 2:return z/(1+fabsf(z));
 case 3:return (2.0f/3.14159265f)*asinf(sinf(cl(z,-1.55f,1.55f)));
 case 4:return cl(z,-1,1);
 case 5:return fabsf(z)*2-1;
 case 6:{float q=cl(z,-1,1);return floorf((q+1)*8+.5f)/8.0f*2-1;}
 case 7:return tanhf(z*2.5f);
 case 8:return z/(1+fabsf(z*.55f));
 default:return tanhf(z+(z>0?.18f:-.18f));
 }
}
static float pole(float x,float*z,float a){*z+=a*(x-*z);return *z;}
static void process(void*p,const int16_t*in,int16_t*out,int frames){
 Inst*s=p;float mix=cl(s->p[12]/100,0,1),drive=1+s->p[1]*.18f,bias=cl(s->p[2],-1,1)*.5f;
 float lowa=1-expf(-2*3.14159265f*cl(s->p[4],20,1000)/44100),higha=1-expf(-2*3.14159265f*cl(s->p[8],2000,20000)/44100);
 float bodya=1-expf(-2*3.14159265f*cl(s->p[9],40,800)/44100),res=cl(s->p[10],.1f,8);
 float gains[3]={powf(10,s->p[5]/20),powf(10,s->p[6]/20),powf(10,s->p[7]/20)};
 float outg=powf(10,s->p[3]/20),bm=cl(s->p[11]/100,0,1);
 for(int f=0;f<frames;f++)for(int c=0;c<2;c++){
  int j=f*2+c;float dry=in[j]/32768.0f,x=dry;
  if(s->p[13]<.5f){
   float lo=pole(x,&s->lp[c],lowa);x-=lo*(1-cl(s->p[4]/1000,0,.9f));
   float hi=pole(x,&s->hp[c],higha);x=hi;
   float sub=pole(x,&s->body[c],bodya);float band=x-sub;
   float body=band*res*.12f+s->body2[c];s->body2[c]=body*.35f;
   x+=body*bm;
   x=sat(x,(int)cl(s->p[0],0,9),drive,bias);
   x*=gains[0]*.25f+gains[1]*.5f+gains[2]*.25f;
   x*=outg;
   x=dry*(1-mix)+x*mix;
  }
  out[j]=(int16_t)lrintf(cl(x,-1,1)*32767);
 }
}
static const mpc_engine_t engine={create,destroy,NULL,setp,getp,NULL,process};
const mpc_engine_t *mpc_engine(void){return &engine;}
