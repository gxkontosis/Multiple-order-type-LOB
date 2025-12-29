#include <algorithm>
#include <vector>

#include "Order.h"

class CompletedOrders
{
public:
   void Add(Order&& order)
   {
       m_completed.push_back(std::move(order));
   }

   const std::vector<Order>& GetAll() const { return m_completed; }

   bool Contains(ID orderId) const
   {
      for (const auto& order : m_completed)
      {
         if (order.GetId() == orderId)
            return true;
      }
      return false;
   }

private:
   std::vector<Order> m_completed;
};