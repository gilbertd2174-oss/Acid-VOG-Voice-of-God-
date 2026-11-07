#include "PluginProcessor.h"
#include "PluginEditor.h"
juce::AudioProcessorValueTreeState::ParameterLayout AcidProcessor::layout(){juce::AudioProcessorValueTreeState::ParameterLayout l;for(auto&s:acid::specs){juce::ParameterID id(s.id,1);if(s.choices[0])l.add(std::make_unique<juce::AudioParameterChoice>(id,s.name,juce::StringArray::fromTokens(s.choices,"|",""),(int)s.initial));else l.add(std::make_unique<juce::AudioParameterFloat>(id,s.name,juce::NormalisableRange<float>(s.lo,s.hi,s.step,s.skew),s.initial));}return l;}
AcidProcessor::AcidProcessor():AudioProcessor(BusesProperties().withOutput("Stereo output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"VOG_ACID",layout()){for(size_t i=0;i<raw.size();i++)raw[i]=state.getRawParameterValue(acid::specs[i].id);for(int i=0;i<16;i++)putPattern(i,acid::factoryPattern(i));setCurrentProgram(0);}
acid::Values AcidProcessor::values()const{acid::Values p{};for(size_t i=0;i<p.size();i++)p[i]=raw[i]->load();return p;}
acid::PatternData AcidProcessor::getPattern(int index)const{acid::PatternData p{};index=juce::jlimit(0,15,index);for(int i=0;i<16;i++)p[(size_t)i]=acid::unpack(patterns[(size_t)index][(size_t)i].load());return p;}
void AcidProcessor::putPattern(int index,const acid::PatternData&p){if(index<0||index>=16)return;for(int i=0;i<16;i++)patterns[(size_t)index][(size_t)i].store(acid::pack(p[(size_t)i]));}
void AcidProcessor::setStep(int pat,int step,const acid::Step&s){if(pat>=0&&pat<16&&step>=0&&step<16)patterns[(size_t)pat][(size_t)step].store(acid::pack(s));}
int AcidProcessor::selectedPattern()const{return juce::jlimit(0,15,(int)raw[acid::Pattern]->load());}
void AcidProcessor::prepareToPlay(double rate,int){sr=rate;engine.prepare(rate);localPpq=0;lastSerial=INT64_MIN;wasPlaying=false;keys.fill(0);}
bool AcidProcessor::isBusesLayoutSupported(const BusesLayout&b)const{return b.getMainInputChannelSet().isDisabled()&&b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void AcidProcessor::processBlock(juce::AudioBuffer<float>&b,juce::MidiBuffer&midi){juce::ScopedNoDenormals guard;b.clear();auto p=values();engine.setParameters(p);bool external=(int)p[acid::Mode]==1;int patIndex=selectedPattern();auto pattern=getPattern(patIndex);
 bool hostAvailable=false,hostPlaying=false,hasPpq=false;double bpm=p[acid::Tempo],hostPpq=0;
 if(auto*h=getPlayHead())if(auto pos=h->getPosition()){hostAvailable=true;hostPlaying=pos->getIsPlaying();if(auto t=pos->getBpm())if(std::isfinite(*t)&&*t>0&&p[acid::HostSync]>.5f)bpm=*t;if(auto q=pos->getPpqPosition()){if(std::isfinite(*q)){hostPpq=*q;hasPpq=true;}}}
 bool synced=p[acid::HostSync]>.5f&&hostAvailable;bool playing=!external&&running.load()&&(!synced||hostPlaying);bpmDisplay.store((float)bpm);
 if(panic.exchange(false)){engine.reset();keys.fill(0);running.store(false);playing=false;lastSerial=INT64_MIN;}
 if(external!=bool(oldMode)||patIndex!=oldPattern){engine.off();lastSerial=INT64_MIN;}oldMode=external?1:0;oldPattern=patIndex;
 if(!playing&&wasPlaying){engine.off();lastSerial=INT64_MIN;localPpq=0;}if(playing&&!wasPlaying)lastSerial=INT64_MIN;wasPlaying=playing;
 auto it=midi.begin(),end=midi.end();float maxPeak=0;int length=juce::jlimit(1,16,(int)p[acid::Length]);
 for(int i=0;i<b.getNumSamples();i++){
  while(it!=end&&(*it).samplePosition<=i){auto message=(*it).getMessage();++it;
   if(message.isNoteOn()){int n=message.getNoteNumber();keys[(size_t)n]=message.getFloatVelocity();if(external)engine.note(n+(int)p[acid::Transpose],message.getFloatVelocity(),message.getVelocity()>100,false,false);}
   else if(message.isNoteOff()){keys[(size_t)message.getNoteNumber()]=0;if(external){int next=-1;for(int n=0;n<128;n++)if(keys[(size_t)n]>0)next=n;if(next>=0)engine.note(next+(int)p[acid::Transpose],keys[(size_t)next],false,true,false);else engine.off();}}
   else if(message.isPitchWheel())engine.pitchWheel((message.getPitchWheelValue()-8192)/8192.f*2);
   else if(message.isController()&&message.getControllerNumber()==1)engine.modWheel(message.getControllerValue()/127.f);
   else if(message.isAllNotesOff()||message.isAllSoundOff()){engine.reset();keys.fill(0);}
  }
  if(playing){double ppq=synced&&hasPpq?hostPpq+i*bpm/(60*sr):localPpq;auto pos=acid::clockAt(ppq,length,(int)p[acid::Rate],p[acid::Swing]);auto& step=pattern[(size_t)pos.step];
   if(pos.serial!=lastSerial){bool tied=lastSerial!=INT64_MIN&&pos.serial==lastSerial+1&&step.slide&&pattern[(size_t)((pos.step+length-1)%length)].gate;lastSerial=pos.serial;playStep.store(pos.step);if(step.gate)engine.note(step.note+(int)p[acid::Transpose],.85f,step.accent,tied,step.vibrato);else engine.off();}
   auto& next=pattern[(size_t)((pos.step+1)%length)];if(pos.fraction>=p[acid::Gate]*.01&&! (next.slide&&next.gate))engine.off();localPpq+=bpm/(60*sr);
  }else playStep.store(-1);
  float l=0,r=0;engine.sample(l,r,bpm);b.setSample(0,i,l);b.setSample(1,i,r);maxPeak=std::max(maxPeak,std::max(std::abs(l),std::abs(r)));
 }protectionTrips.store(engine.protectionTrips());peak.store(std::max(maxPeak,peak.load()*.8f));midi.clear();}
void AcidProcessor::setCurrentProgram(int n){if(n<0||n>=256)return;program.store(n);auto&p=acid::scenes[(size_t)n].values;for(size_t i=0;i<p.size();i++)if(auto*param=state.getParameter(acid::specs[i].id)){param->beginChangeGesture();param->setValueNotifyingHost(param->convertTo0to1(p[i]));param->endChangeGesture();}}
void AcidProcessor::getStateInformation(juce::MemoryBlock& data){auto tree=state.copyState();tree.setProperty("scene",program.load(),nullptr);auto prior=tree.getChildWithName("PATTERNS");if(prior.isValid())tree.removeChild(prior,nullptr);juce::ValueTree banks("PATTERNS");for(int j=0;j<16;j++){juce::ValueTree bank("BANK");auto p=getPattern(j);for(int i=0;i<16;i++)bank.setProperty("s"+juce::String(i),(int)acid::pack(p[(size_t)i]),nullptr);banks.appendChild(bank,nullptr);}tree.appendChild(banks,nullptr);if(auto xml=tree.createXml())copyXmlToBinary(*xml,data);}
void AcidProcessor::setStateInformation(const void*data,int size){if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType())){auto t=juce::ValueTree::fromXml(*xml);auto banks=t.getChildWithName("PATTERNS");if(banks.getNumChildren()==16)for(int j=0;j<16;j++){auto p=getPattern(j);for(int i=0;i<16;i++)p[(size_t)i]=acid::unpack((uint32_t)(int)banks.getChild(j).getProperty("s"+juce::String(i),(int)acid::pack(p[(size_t)i])));putPattern(j,p);}program.store(juce::jlimit(0,255,(int)t.getProperty("scene",0)));state.replaceState(t);panic.store(true);}}
juce::AudioProcessorEditor*AcidProcessor::createEditor(){return new AcidEditor(*this);}juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new AcidProcessor();}
