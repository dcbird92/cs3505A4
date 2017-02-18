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
  int date;

 public:
  food_item();
  food_item(std::string numberID, std::string nameID, int life, int amount, int day);
  food_item(const food_item &here);
  std::string getUPC()const;
  std::string getName()const;
  int getLife()const;
  int getTotal()const;
  int setDate(int someDate);
  void isBad();
  void changeQuantity(int foodNum);
  int getDate();

};



#endif
