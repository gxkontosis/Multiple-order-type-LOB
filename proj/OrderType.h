#include "Side.h"
#include <vector>

using Price = double;
using ID = int;
using Volume = double;
using Quantity = double;

enum class OrderType
{
   GoodTillCancel,
   ImmediateOrCancel,
   FillOrKill,
   Market,
};



enum class OrderOutcome
{
   FullyFilled,
   PartiallyFilledAndCancelled,
   PartiallyFilledAndAddedToBook,
   Cancelled,
   AddedToOrderbook
};

class Order
{
public:
   Order(OrderType orderType, int id, double price, Side side, double volume)
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
   Volume GetInitialQuantity() const { return m_initialVolume; }

   void SetRemainingVolume(Volume volume) { m_remainingVolume = volume; }

private:
   ID m_id;
   Price m_price;
   Side m_side;
   OrderType m_orderType;
   Volume m_initialVolume;
   Volume m_remainingVolume;
};

class CompletedOrders
{
public:
   // Add a completed order (prefer moving for speed)
   void Add(Order&& order)
   {
       m_completed.push_back(std::move(order));
   }

   const std::vector<Order>& GetAll() const { return m_completed; }

   // Optional: if you want mutable access
   std::vector<Order>& GetAllMutable() { return m_completed; }

private:
   std::vector<Order> m_completed;
};