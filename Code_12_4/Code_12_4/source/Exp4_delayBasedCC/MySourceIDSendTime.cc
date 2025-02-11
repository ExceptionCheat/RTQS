#include "MySourceIDSendTime.h"

namespace ns3{


void MySourceIDSendTime::MySetTime(Time time){
  m_some_time=time;
}

Time MySourceIDSendTime::MyGetTime(void)const{
  return m_some_time;
}


// void MySourceIDTag::Set(uint32_t value){
//   m_MySourceIDValue=value;
// }

MySourceIDSendTime::MySourceIDSendTime(){

}
MySourceIDSendTime::~MySourceIDSendTime(){

}

TypeId 
MySourceIDSendTime::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MySourceIDSendTime")
    .SetParent<Tag> ()
    .AddConstructor<MySourceIDSendTime> ()
    .AddAttribute ("MySomeTime",
                   "MySomeTime",
                   EmptyAttributeValue (),
                   MakeTimeAccessor (&MySourceIDSendTime::m_some_time),
                   MakeTimeChecker())
  ;
  return tid;
}
TypeId 
MySourceIDSendTime::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MySourceIDSendTime::GetSerializedSize (void) const
{
  return sizeof (int64_t);
}
void 
MySourceIDSendTime::Serialize (TagBuffer i) const
{
  int64_t t = m_some_time.GetNanoSeconds ();
  i.Write ((const uint8_t *)&t, 8);//TimestampTag::Serialize (TagBuffer i) const
}
void 
MySourceIDSendTime::Deserialize (TagBuffer i)
{
  //赋值语句，内存赋值给对象的语句
  int64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_some_time = NanoSeconds (t);//TimestampTag::Deserialize (TagBuffer i)
}
void 
MySourceIDSendTime::Print (std::ostream &os) const
{
  os << "MySourceIDSendTime=" << m_some_time;
  //void TimestampTag::Print (std::ostream &os) const
}
}