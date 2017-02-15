#ifndef FOOD_ITEM
#define FOOD_ITEM

#include <string>

/* 
 *Class holding the information about the food items in the transactions.
*/
class food_item
{
 private:
  std::string UPC;
  std::string name;
  int shelf_life;
  int quantity;

 public:
  food_item(std::string numberID, std::string nameID, int life, int amount);
  std::string getUPC()const;
  std::string getName()const;
  int getLife()const;
  int getTotal()const;

};



#endif
