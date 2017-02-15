#ifndef WAREHOUSECPP
#define WAREHOUSECPP

#include "warehouse.h"


//adding a comment
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
    //records item and quantity as recieved in correct warehouses inv
    std::map<std::string, std::vector<food_item> >::iterator it = inventory.find(stock.getName());
    it->second.push_back(stock);
    
    //To keep track of day of which item will expire:
    //shelflife + currentDate

  }
  
  food_item warehouse::request(food_item sell)
  {
    //Check if warehouse contains that item

    //if multiple groups of that item exists, remove the one with the 
    //oldest expiration date

    //check if warehouse contains enough items to fulfill request
    //if it does
    //   Remove the item
    //else
    //   Remove available items, ignore the rest

  }

  // Update the shelf life of the product and removes if == 0
  void warehouse::update()
  {
    
  }

  bool warehouse::contains(food_item product)
  {
  }



#endif
