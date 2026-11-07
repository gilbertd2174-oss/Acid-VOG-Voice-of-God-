#include "AcidEngine.h"
#include <iostream>
#include <stdexcept>
#include <set>
#include <sstream>
void check(bool v,const char*m){if(!v)throw std::runtime_error(m);}
float render(acid::Engine&e,int n){float peak=0;while(n--){float l,r;e.sample(l,r,128);check(std::isfinite(l)&&std::isfinite(r),"Non-finite output");check(std::abs(l)<=.50001f&&std::abs(r)<=1.001f,"Output overflow");peak=std::max(peak,std::max(std::abs(l),std::abs(r)));}return peak;}
int main(){try{
 for(int pat=0;pat<16;pat++)for(auto&s:acid::factoryPattern(pat)){auto copy=acid::unpack(acid::pack(s));check(copy.note==s.note&&copy.gate==s.gate&&copy.accent==s.accent&&copy.slide==s.slide&&copy.vibrato==s.vibrato,"Step serialization mismatch");}
 for(int length=1;length<=16;length++)for(int rate=0;rate<4;rate++)for(float swing:{0.f,30.f,60.f})for(int i=-40;i<100;i++){auto c=acid::clockAt(i*.017,length,rate,swing);check(c.step>=0&&c.step<length&&c.fraction>=0&&c.fraction<=1,"Clock position invalid");}
 check(acid::clockAt(.25,16,1,0).step==1,"Straight clock mismatch");check(acid::clockAt(.25,16,1,50).step==0,"Swing long step mismatch");check(acid::clockAt(.375,16,1,50).step==1,"Swing boundary mismatch");
 std::set<std::string> unique;
 for(auto&scene:acid::scenes){std::ostringstream id;for(size_t i=0;i<scene.values.size();i++){float v=scene.values[i];check(v>=acid::specs[i].lo&&v<=acid::specs[i].hi,"Scene outside parameter range");id<<v<<',';}unique.insert(id.str());acid::Engine e;e.prepare(48000);e.setParameters(scene.values);e.note(36,.85f,true,false,false);check(render(e,16000)>.00001f,"Silent scene");e.note(43,.85f,false,true,true);render(e,8000);e.off();render(e,8000);e.reset();check(render(e,32)==0,"Panic tails not cleared");}
 check(unique.size()==256,"Factory scenes are not unique");
 for(double sr:{44100.,48000.,96000.}){acid::Engine e;e.prepare(sr);auto p=acid::defaults();p[acid::Resonance]=.95f;p[acid::Drive]=1;p[acid::DistAmount]=1;p[acid::EchoFeedback]=.85f;p[acid::ReverbMix]=.65f;e.setParameters(p);e.note(60,1,true,false,true);render(e,(int)sr);e.off();render(e,(int)sr);}
 std::cout<<"PASS: step state, swing clock, 256 distinct scenes, audio routes, slides, reset and three sample rates\n";return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
