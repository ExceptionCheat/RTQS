
#ifndef MY_SOURCE_ID_TYPE_HB
#define MY_SOURCE_ID_TYPE_HB

#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include <iostream>

namespace ns3 {

class MySourceIDType : public Tag
{
public:
  MySourceIDType ();
  ~MySourceIDType();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  void Set(uint32_t value);
  // void Set (uint32_t value){
    
  // }

  uint32_t Get (void) const{
    return m_MySourceIDValue;
  }

//   void MySetTime(Time time);
//   Time MyGetTime(void);


private:
  uint32_t m_MySourceIDValue;
//   Time m_some_time;
};



// void MySourceIDTag::MySetTime(Time time){
//   m_some_time=time;
// }

// Time MySourceIDTag::MyGetTime(void){
//   return m_some_time;
// }


}
#endif