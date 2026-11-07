#pragma once
#include "SynthEngine.h"
#include "AcidParameters.h"
namespace acid {
struct ClockPosition{int step=0;int64_t serial=0;double fraction=0;};
inline ClockPosition clockAt(double ppq,int length,int rate,float swing){const double beats[]={.5,.25,1.0/6,.125};double u=ppq/beats[(size_t)rate];double pair=std::floor(u/2);double within=u-pair*2;double split=1+std::max(0.f,swing)*.01;int odd=within>=split?1:0;int64_t serial=(int64_t)pair*2+odd;double fraction=odd?(within-split)/(2-split):within/split;int step=(int)((serial%length+length)%length);return {step,serial,fraction};}
// Acid-local filter: limit the modulated cutoff before the integrators,
// retain headroom at high resonance, and reject corrupted internal states.
struct StableLowpass{
 float s1=0,s2=0;bool failed=false;
 void reset(){s1=s2=0;failed=false;}
 float tick(float x,float hz,float resonance,float sr){float g=std::tan(davoe::pi*davoe::clamp(hz,25.f,std::min(11000.f,sr*.22f))/sr);float k=2.f-1.65f*davoe::clamp(resonance,0.f,.82f);float a=1.f/(1.f+g*(g+k));float v1=a*(s1+g*(x-s2)),v2=s2+g*v1;s1=2.f*v1-s1;s2=2.f*v2-s2;if(!std::isfinite(s1)||!std::isfinite(s2)||std::abs(s1)>8.f||std::abs(s2)>8.f){s1=s2=0;failed=true;return 0;}return v2;}
};
class Engine{
 float sr=48000,phase=0,subPhase=0,vibPhase=0,pitch=36,targetPitch=36,amp=0,env=0,accentEnv=0,bend=0,wheel=0;
 bool held=false,sliding=false,vibrato=false;Values current=defaults(),target=defaults();StableLowpass filter1,filter2;davoe::Effects fx;davoe::Values fxValues=davoe::defaults();float slew=.001f,dcIn[2]{},dcOut[2]{},outputLow[2]{},dcPole=.997f,outputCoef=.8f,fade=0;uint64_t trips=0;int cooldown=0;
public:
 void prepare(double rate){sr=(float)std::min(384000.0,std::max(8000.0,rate));dcPole=std::exp(-2.f*davoe::pi*20.f/sr);outputCoef=1.f-std::exp(-2.f*davoe::pi*11000.f/sr);trips=0;slew=1-std::exp(-1.f/(sr*.012f));fx.prepare(sr);reset();setParameters(target);current=target;}
 void setParameters(const Values&p){for(size_t i=0;i<p.size();i++)target[i]=std::isfinite(p[i])?davoe::clamp(p[i],specs[i].lo,specs[i].hi):specs[i].initial;
  fxValues[davoe::CompOn]=0;fxValues[davoe::EqOn]=0;fxValues[davoe::DistOn]=1;fxValues[davoe::DistMode]=target[DistType];fxValues[davoe::DistDrive]=target[DistAmount];fxValues[davoe::DistTreble]=target[DistColor];fxValues[davoe::DistBass]=-target[DistColor]*.4f;fxValues[davoe::DistMix]=target[DistMix];fxValues[davoe::DistLevel]=-6;
  fxValues[davoe::Chorus]=target[Chorus];fxValues[davoe::DelayMix]=target[EchoMix];fxValues[davoe::DelayTime]=target[EchoTime];fxValues[davoe::Feedback]=std::min(.68f,target[EchoFeedback]);fxValues[davoe::EchoSync]=target[EchoSync];fxValues[davoe::EchoHeads]=1;fxValues[davoe::Reverb]=target[ReverbMix];fxValues[davoe::Space]=target[Room];fxValues[davoe::VerbDecay]=1+target[Room]*4;fxValues[davoe::PreDelay]=target[PreDelay];fx.update(fxValues,sr);
 }
 void note(int n,float velocity,bool accent,bool slide,bool vib){targetPitch=davoe::clamp((float)n,0.f,127.f);if(!std::isfinite(velocity))velocity=.5f;vibrato=vib;sliding=slide&&amp>.001f;if(!sliding){pitch=targetPitch;env=davoe::clamp(velocity,.1f,1);accentEnv=accent?1.f:0.f;}else if(accent)accentEnv=1;held=true;}
 void off(){held=false;}
 void pitchWheel(float semitones){bend=std::isfinite(semitones)?davoe::clamp(semitones,-24.f,24.f):0;}void modWheel(float value){wheel=std::isfinite(value)?davoe::clamp(value,0.f,1.f):0;}
 uint64_t protectionTrips()const{return trips;}
 void reset(){for(int c=0;c<2;c++)dcIn[c]=dcOut[c]=outputLow[c]=0;fade=0;cooldown=0;phase=subPhase=vibPhase=amp=env=accentEnv=bend=wheel=0;held=sliding=vibrato=false;filter1.reset();filter2.reset();fx.reset();}
 void sample(float& l,float&r,double bpm){if(cooldown>0){--cooldown;l=r=0;return;}for(size_t i=0;i<current.size();i++)current[i]+=(target[i]-current[i])*slew;
  if(sliding)pitch+=(targetPitch-pitch)*(1-std::exp(-1.f/(sr*.001f*current[Slide])));else pitch=targetPitch;
  vibPhase+=current[VibratoRate]/sr;if(vibPhase>=1)vibPhase-=1;
  float hz=davoe::midiHz(pitch+bend+current[Tune]*.01f+std::sin(2*davoe::pi*vibPhase)*current[VibratoDepth]*.01f*(vibrato?1.f:wheel));float dt=std::min(.45f,hz/sr);
  float x=davoe::oscillator(phase,dt,(int)target[Wave],.5f)+std::sin(2*davoe::pi*subPhase)*current[Sub];phase+=dt;phase-=std::floor(phase);subPhase+=dt*.5f;subPhase-=std::floor(subPhase);
  env*=std::exp(-1/(sr*current[Decay]*.001f));accentEnv*=std::exp(-1/(sr*current[AccentDecay]*.001f));
  float cutoff=current[Cutoff]*std::exp2(env*current[EnvDepth]+accentEnv*current[Accent]*1.4f);
  x=std::tanh(x*(1+current[Drive]*4))*.7f;x=filter1.tick(x,cutoff,current[Resonance],sr);x=filter2.tick(x,cutoff,0,sr);
  amp+=((held?1.f:0.f)-amp)*(1-std::exp(-1.f/(sr*(held?.002f:.009f))));x*=amp*(.7f+accentEnv*current[Accent]*.7f)*.5f;
  if(filter1.failed||filter2.failed||!std::isfinite(x)||std::abs(x)>4.f){reset();++trips;cooldown=(int)(sr*.1f);l=r=0;return;}
  l=r=x;fx.tick(l,r,fxValues,sr,std::isfinite(bpm)?davoe::clamp((float)bpm,20.f,300.f):128.f,wheel);
  if(!std::isfinite(l)||!std::isfinite(r)||std::abs(l)>4.f||std::abs(r)>4.f){reset();++trips;cooldown=(int)(sr*.1f);l=r=0;return;}
  float gain=std::pow(10.f,current[Volume]/20.f);float signal[]={l,r};fade=std::min(1.f,fade+1.f/(sr*.025f));for(int c=0;c<2;c++){float dc=signal[c]-dcIn[c]+dcPole*dcOut[c];dcIn[c]=signal[c];dcOut[c]=dc;outputLow[c]+=outputCoef*(dc-outputLow[c]);signal[c]=.5f*std::tanh(outputLow[c]*gain/.5f)*fade;}
  l=signal[0];r=signal[1];if(env<1.e-10f)env=0;if(accentEnv<1.e-10f)accentEnv=0;if(!held&&amp<1.e-8f)amp=0;
 }
};
}
