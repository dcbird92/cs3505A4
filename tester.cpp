
#include "food_item.h"
#include <iostream>

using namespace std;

int main()
{
  std::string UPCTest = "0000000001";
  std::string nameTest = "Orange";
  int lifeTime = 5;
  int amount = 10;

  food_item testing(UPCTest, nameTest, lifeTime, amount);
  
  // Checking that the get methods all return the correct info
  // UPC Test
  if(testing.getUPC() == "0000000001")
    {
    cout << "UPC Testing Passed" << endl;
    }
  else cout << "UPC Testing Failed";
  // Name Test
  if(testing.getName() == "Orange")
    {
      cout << "Get Name Testing Passed" << endl;
    }
  else cout << "Get Name Test Failed" << endl;
  // Shelf Life Test
  if(testing.getLife() == 5)
    {
      cout << "Get Life Test Passed" << endl;
    }
  else cout << "Get Life Test Failed" << endl;
  // Amount Test
  if(testing.getTotal() == 10)
    {
      cout << "Get Total Test Passed" << endl;
    }
  else cout << "Get Total Test Passed" << endl;
}
