#include <string>
#include <iostream>

using namespace std;

struct Obj_ {
//   Obj_& operator [] (const char*) {
//      cout << "const char* subscript." << endl;
//      return *this;
//   }
   Obj_& operator [] (const string&) {
      cout << "const string& subscript." << endl;
      return *this;
   }
//   operator int () {
//      cout << "int cast." << endl;
//      return 0;
//   }
};

int main () {
   Obj_ Obj;
   char Str1[5] = "test";
   string Str2 =  "test";

   Obj[Str1];
   Obj[Str2][Str1];
   
   Str1[0];
   cout << 2[Str1] << endl;

   return 0;
}
