#include "engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define N 27
static const char *keys[N]={"input_gain","polarity","hpf","lpf","comp_threshold","comp_ratio","comp_attack","comp_release","comp_makeup","gate_threshold","gate_release","limiter","lf_freq","lf_gain","lf_shelf","lmf_freq","lmf_gain","lmf_q","hmf_freq","hmf_gain","hmf_q","hf_freq","hf_gain","hf_shelf","output_gain","mix","bypass"};
static const float defs[N]={0,0,20,20000,0,1,10,120,0,-80,100,0,100,0,1,400,0,1,2500,0,1,10000,0,1,0,100,0};
typedef struct {float p[N],env,gate; float hp[2],lp[2],eq[4][2];} Inst;
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);}
static int idx(const char*k){for(int i=0;i<N;i++)if(!strcmp(k,keys[i]))return i;return -1;}
static void *create(const char*d){(void)d;Inst*s=calloc(1,sizeof(*s));if(s){memcpy(s->p,defs,sizeof(defs));s->gate=1;}return s;}
static void destroy(void*p){free(p);}
static void setp(void*p,const char*k,const char*v){Inst*s=p;int i=idx(k);if(i>=0)s->p[i]=strtof(v,0);}
static int getp(void*p,const char*k,char*b,int n){Inst*s=p;int i=idx(k);if(i<0)return 0;snprintf(b,n,"%.6g",s->p[i]);return 1;}
static float onepole(float x,float *z,float a){*z+=a*(x-*z);return *z;}
static float shape_eq(float x,float hz,float gain,float q,float *mem,int mode,float sr){
 if(fabsf(gain)<0.001f)return x;
 float a=expf(-2.0f*3.14159265f*clampf(hz,20,18000)/sr);
 float low=onepole(x,mem,1-a);
 float g=powf(10.0f,gain/20.0f);
 if(mode==1)return x+(low-x)*(g-1.0f);
 float band=x-low;
 return x+band*(g-1.0f)/(1.0f+fabsf(q-1.0f)*0.25f);
}
static void process(void*p,const int16_t*in,int16_t*out,int frames){
 Inst*s=p;float drymix=clampf(s->p[25]/100,0,1),ing=powf(10,s->p[0]/20),og=powf(10,s->p[24]/20);
 float hpcoef=1-expf(-2*3.14159265f*clampf(s->p[2],20,500)/44100);
 float lpcoef=1-expf(-2*3.14159265f*clampf(s->p[3],2000,20000)/44100);
 float ca=expf(-1/(44100*clampf(s->p[6],.2f,100)/1000));
 float cr=expf(-1/(44100*clampf(s->p[7],10,1000)/1000));
 float gr=expf(-1/(44100*clampf(s->p[10],5,500)/1000));
 float ct=powf(10,s->p[4]/20),gt=powf(10,s->p[9]/20);
 for(int f=0;f<frames;f++)for(int c=0;c<2;c++){
  int j=f*2+c;float x=in[j]/32768.0f, dry=x;
  if(s->p[26]<.5f){
   x*=ing*(s->p[1]>.5f?-1:1);
   float low=onepole(x,&s->hp[c],hpcoef);x-=low;
   x=onepole(x,&s->lp[c],lpcoef);
   x=shape_eq(x,s->p[12],s->p[13],1,&s->eq[0][c],(int)s->p[14],44100);
   x=shape_eq(x,s->p[15],s->p[16],s->p[17],&s->eq[1][c],0,44100);
   x=shape_eq(x,s->p[18],s->p[19],s->p[20],&s->eq[2][c],0,44100);
   x=shape_eq(x,s->p[21],s->p[22],1,&s->eq[3][c],(int)s->p[23],44100);
   float env=fabsf(x);float coeff=env>s->env?ca:cr;s->env=coeff*s->env+(1-coeff)*env;
   float target=1;
   if(s->env>ct){float db=20*log10f(s->env/ct);target=powf(10,-db*(1-1/clampf(s->p[5],1,20))/20);}
   x*=target*powf(10,s->p[8]/20);
   float gateTarget=s->env<gt?0.0f:1.0f;
   s->gate=gr*s->gate+(1-gr)*gateTarget;
   x*=s->gate;
   if(s->p[11]>.5f){float lim=0.89f;if(x>lim)x=lim;if(x< -lim)x=-lim;}
   x*=og;
   x=dry*(1-drymix)+x*drymix;
  }
  x=clampf(x,-1,1);out[j]=(int16_t)lrintf(x*32767);
 }
}
static const mpc_engine_t engine={create,destroy,NULL,setp,getp,NULL,process};
const mpc_engine_t *mpc_engine(void){return &engine;}
