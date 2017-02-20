#ifndef WAREHOUSE
#define WAREHOUSE

#include <map>
#include <vector>
#include "food_item.h"

/*
 * Warehouse class that hold a list of food items with UPC keys.
 */

class warehouse
{
 private:
  std::string name;
  std::map<std::string,std::vector<food_item> > inventory;
  
 public:
  warehouse();
  warehouse(std::string warehouseName);
  std::string getName()const;
  // adds a food_item to the list
  void receive(food_item stock);
  
  void request(food_item sell);
  // Update the shelf life of the product and removes if == 0
  void update(int date);
  bool contains(food_item product);
};

#endif

