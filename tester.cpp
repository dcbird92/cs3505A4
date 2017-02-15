
#include "food_item.cpp"
#include <iostream>

int main()
{
  std::string UPCTest = "0000000001";
  std::string nameTest = "chocolate salty balls";
  int lifeTime = 5;
  int amount = 10;

  food_item::food_item testing(UPCTest, nameTest, lifeTime, amount);

}
