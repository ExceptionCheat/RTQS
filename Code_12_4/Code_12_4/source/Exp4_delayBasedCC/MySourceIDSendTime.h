
#ifndef MY_SOURCE_ID_SendTime_HB
#define MY_SOURCE_ID_SendTime_HB

#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/nstime.h"
#include <iostream>

namespace ns3 {

class MySourceIDSendTime : public Tag
{
public:
  MySourceIDSendTime ();
  ~MySourceIDSendTime();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

//   void Set(uint32_t value);
//   // void Set (uint32_t value){
    
//   // }

//   uint32_t Get (void) const{
//     return m_MySourceIDValue;
//   }

  void MySetTime(Time time);
  Time MyGetTime(void)const;


private:
  // uint32_t m_MySourceIDValue;
  Time m_some_time;
};

}
#endif