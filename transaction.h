#ifndef TRANSACTION
#define TRANSACTION

#include "warehouse.h"

class transaction
{
  private:
   std::vector<food_item> foodList;
   std::map<std::string, warehouse> warehouseMap;

  public:
   transaction();
   void getTransactions(std::string file);
};

#endif
