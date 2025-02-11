#include "MySourceIDMPD.h"
namespace ns3{



void MySourceIDMPD::MySetTime(Time time){
  m_some_time=time;
}

Time MySourceIDMPD::MyGetTime(void)const{
  return m_some_time;
}


MySourceIDMPD::MySourceIDMPD(){

}
MySourceIDMPD::~MySourceIDMPD(){

}

TypeId 
MySourceIDMPD::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MySourceIDMPD")
    .SetParent<Tag> ()
    .AddConstructor<MySourceIDMPD> ()
    .AddAttribute ("MySomeTime",
                   "MySomeTime",
                   EmptyAttributeValue (),
                   MakeTimeAccessor (&MySourceIDMPD::MyGetTime),
                   MakeTimeChecker ())
  ;
  return tid;
}
TypeId 
MySourceIDMPD::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MySourceIDMPD::GetSerializedSize (void) const
{
  return sizeof (Time);
}
void 
MySourceIDMPD::Serialize (TagBuffer i) const
{
  uint64_t t = m_some_time.GetNanoSeconds ();
  i.Write ((const uint8_t *)&t, 8);//TimestampTag::Serialize (TagBuffer i) const
}
void 
MySourceIDMPD::Deserialize (TagBuffer i)
{
  //赋值语句，内存赋值给对象的语句
  uint64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_some_time = NanoSeconds (t);//TimestampTag::Deserialize (TagBuffer i)
}
void 
MySourceIDMPD::Print (std::ostream &os) const
{
  os << "MySourceIDMPD=" << m_some_time;
  //void TimestampTag::Print (std::ostream &os) const
}
}