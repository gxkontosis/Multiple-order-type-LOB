#include "OrderDetails.h"

class Order
{
public:
   Order(OrderType orderType, ID id, Price price, Side side, Volume volume)
      :
      m_orderType(orderType),
      m_id(id),
      m_price(price),
      m_side(side),
      m_initialVolume(volume),
      m_remainingVolume(volume)
   {}

   ID GetId() const { return m_id; }
   Price GetPrice() const { return m_price; }
   Side GetSide() const { return m_side; }
   OrderType GetType() const { return m_orderType; }
   Volume GetInitialVolume() const { return m_initialVolume; }
   Volume GetRemainingVolume() const { return m_remainingVolume; }

   void SetRemainingVolume(Volume volume) { m_remainingVolume = volume; }

private:
   ID m_id;
   Price m_price;
   Side m_side;
   OrderType m_orderType;
   Volume m_initialVolume;
   Volume m_remainingVolume;
};