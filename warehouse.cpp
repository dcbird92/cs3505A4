#ifndef WAREHOUSECPP
#define WAREHOUSECPP

#include "warehouse.h"
//
//
//
//here is a comment
{

  warehouse::warehouse(std::string warehouseName)
  {
    this->name = warehouseName;
  }

  std::string warehouse::getName() const
  {
    return name;
  }

  // adds a food_item to the list
  void warehouse::receive(food_item stock)
  {
    std::map<std::string, std::vector<food_item>>::iterator it = inventory.find(stock.getName());
    it.second.push_back(stock);
  }
  
  food_item warehouse::request(food_item sell)
  {


  }
  // Update the shelf life of the product and removes if == 0
  void warehouse::update()
  {
    
  }

  bool warehouse::contains(food_item product)
  {
    
  }
};



#endif
