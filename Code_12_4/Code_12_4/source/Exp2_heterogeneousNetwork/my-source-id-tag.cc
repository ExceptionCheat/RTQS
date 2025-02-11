#include "my-source-id-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CSourceIDTag");

NS_OBJECT_ENSURE_REGISTERED (CSourceIDTag);

TypeId 
CSourceIDTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CSourceIDTag")
    .SetParent<Tag> ()
    .AddConstructor<CSourceIDTag> ()
    .AddAttribute ("MySourceIDValue",
                   "MySourceIDvalue",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&CSourceIDTag::Get),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId 
CSourceIDTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
CSourceIDTag::GetSerializedSize (void) const
{
  return sizeof (uint32_t);
}
void 
CSourceIDTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_MySourceIDValue);
}
void 
CSourceIDTag::Deserialize (TagBuffer i)
{
  m_MySourceIDValue = i.ReadU32 ();
}
void 
CSourceIDTag::Print (std::ostream &os) const
{
  os << "MySourceID=" << (uint32_t)m_MySourceIDValue;
}
void 
CSourceIDTag::Set (uint32_t value)
{
  m_MySourceIDValue = value;
}
uint32_t 
CSourceIDTag::Get (void) const
{
  return m_MySourceIDValue;
}

} // namespace ns3

