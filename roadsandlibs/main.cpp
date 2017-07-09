/* 
   Solves hackerrank problem:
   https://www.hackerrank.com/challenges/torque-and-development

   My approach:
   
   Each city must either have a library, or be connected to some city that has one.
   It doesn't matter how far apart they are... sounds like an MST, doesn't it!

   My solution is to augment the graph with a 'library' node, connect to each city
   with cost Clib. Then, generate an MST starting at this node.
 */ 

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main()
{
  cout << "hello world!" << endl;
  
  return 0;
}

