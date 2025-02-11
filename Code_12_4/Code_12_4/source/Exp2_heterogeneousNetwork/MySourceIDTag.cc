
#include "MySourceIDTag.h"
namespace ns3{


uint32_t MySourceIDTag::Get (void) const{
  return m_MySourceIDValue;
}

void MySourceIDTag::Set(uint32_t value){
  m_MySourceIDValue=value;
}

MySourceIDTag::MySourceIDTag(){

}
MySourceIDTag::~MySourceIDTag(){

}

TypeId 
MySourceIDTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MySourceIDTag")
    .SetParent<Tag> ()
    .AddConstructor<MySourceIDTag> ()
    .AddAttribute ("MySourceIDValue",
                   "MySourceIDvalue",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&MySourceIDTag::Get),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId 
MySourceIDTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MySourceIDTag::GetSerializedSize (void) const
{
  return sizeof (uint32_t);
}
void 
MySourceIDTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_MySourceIDValue);
}
void 
MySourceIDTag::Deserialize (TagBuffer i)
{
  //赋值语句，内存赋值给对象的语句
  m_MySourceIDValue = i.ReadU32 ();
}
void 
MySourceIDTag::Print (std::ostream &os) const
{
  os << "MySourceID=" << (uint32_t)m_MySourceIDValue;
}
}