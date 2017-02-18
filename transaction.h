#ifndef TRANSACTION
#define TRANSACTION

#include "warehouse.h"

class transaction
{
  private:
  // std::vector<food_item> foodList;
   std::map<std::string, warehouse> warehouseMap;
   std::map<std::string, food_item> foodList;
   int maindate;
   
  public:
   transaction();
   void getTransactions(std::string file);
};

#endif
