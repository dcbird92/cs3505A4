#ifndef TRANSACTION
#define TRANSACTION

#include "warehouse.h"

public class transaction
{
  private:
   std::vector<food_item> foodList;
   std::vector<warehouse> warehouseList;
   void getTrasactions(std::string file);

  public:
   void acceptFile(std::string filename);


};

#endif
