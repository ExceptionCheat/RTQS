#include "MySourceIDType.h"
namespace ns3{

void MySourceIDType::Set(uint32_t value){
  m_MySourceIDValue=value;
}

MySourceIDType::MySourceIDType(){

}
MySourceIDType::~MySourceIDType(){

}

TypeId 
MySourceIDType::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MySourceIDType")
    .SetParent<Tag> ()
    .AddConstructor<MySourceIDType> ()
    .AddAttribute ("MySourceIDValue",
                   "MySourceIDvalue",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&MySourceIDType::Get),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId 
MySourceIDType::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MySourceIDType::GetSerializedSize (void) const
{
  return sizeof (uint32_t);
}
void 
MySourceIDType::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_MySourceIDValue);
}
void 
MySourceIDType::Deserialize (TagBuffer i)
{
  //赋值语句，内存赋值给对象的语句
  m_MySourceIDValue = i.ReadU32 ();
}
void 
MySourceIDType::Print (std::ostream &os) const
{
  os << "MySourceID=" << (uint32_t)m_MySourceIDValue;
}

}