#ifndef TRANSACTION
#define TRANSACTION

#include "warehouse.h"

class transaction
{
  private:
   std::vector<food_item> foodList;
   std::vector<warehouse> warehouseList;

  public:
   transaction();
   void getTransactions(std::string file);
   void acceptFile(std::string filename);
};

#endif
