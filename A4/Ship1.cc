#include "Ship.h"

using namespace shipping;

int main() {

  // create restrictions for specific locations on the ship
  std::vector<std::tuple<X, Y, Height>> restrictions = {
      std::tuple(X{2}, Y{6}, Height{0}),
      std::tuple(X{2}, Y{7}, Height{1}),
      std::tuple(X{2}, Y{5}, Height{6}),
  };

  std::cout << "test1\n";
  // create bad ship 1
  try {
    restrictions.push_back(std::tuple(X{2}, Y{5}, Height{6}));
    Ship<std::string> myShip{X{4}, Y{12}, Height{16}, restrictions};
  } catch (BadShipOperationException &e) {
    // exception: duplicate restrictions (whether or not it has same limit):
    // restriction with X{2}, Y{5} appears more than once (added in the try)
    restrictions.pop_back(); // remove the duplicate restriction
  }

  std::cout << "test2\n";
  // create bad ship 2
  try {
    Ship<std::string> myShip{X{4}, Y{7}, Height{8}, restrictions};
  } catch (BadShipOperationException &e) {
    // exception due to bad restrictions:
    // restriction with Y=7, when the size of Y is 7
  }
  std::cout << "test3\n";
  // create bad ship 3
  try {
    Ship<std::string> myShip{X{4}, Y{12}, Height{6}, restrictions};
  } catch (BadShipOperationException &e) {
    // exception due to bad restrictions:
    // restriction with height=6, when original height is equal or smaller
  }
  std::cout << "good-test4\n";
  // create good ship
  Ship<std::string> myShip{X{4}, Y{8}, Height{8}, restrictions};

  std::cout << "test5\n";
  // bad load - no room
  try {
    myShip.load(X{2}, Y{6}, "Hello");
  } catch (BadShipOperationException &e) { // no room at this location
  }

  std::cout << "good-test6\n";
  // good load
  try {
    myShip.load(X{2}, Y{7}, "Hello");
  } catch (BadShipOperationException &e) {
    e.print();
  }

  std::cout << "test7\n";
  // bad load - no room
  try {
    myShip.load(X{2}, Y{7}, "Hello");
  } catch (BadShipOperationException &e) { // no room at this location
  }

  // bad unload - no container at location
  try {
    std::string container = myShip.unload(X{1}, Y{1});
  } catch (BadShipOperationException &e) { // no container at this location
  }

  // bad load - wrong index
  try {
    myShip.load(X{1}, Y{8}, "Hi");
  } catch (BadShipOperationException &e) { /* bad index Y {8} */
  }
}
