#include <any>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include "Datatype.h"
#include <cmath>

using std::any;
using std::function;
using std::map;
using std::string;
using std::vector;

map<string, function<any(vector<any>)>> builtinFunctionTable = {
    // 배열이나 맵의 길이를 반환하는 내장 함수
    {"length", [](vector<any> values) -> any // length([1, 2, 3]) -> 3 / length({a: 1, b: 2}) -> 2
     {
       if (values.size() == 1 && isArray(values[0]))                    // 배열의 길이 반환 (length([1, 2, 3]) -> 3)
         return static_cast<double>(toArray(values[0])->values.size()); // static_cast<double>는 double로 변환하는 것
       if (values.size() == 1 && isMap(values[0]))                      // 맵의 길이 반환 (length({a: 1, b: 2}) -> 2)
         return static_cast<double>(toMap(values[0])->values.size());   // static_cast<double>는 double로 변환하는 것
       return 0.0;
     }},
    // 배열에 새로운 요소를 추가하는 내장 함수
    {"push", [](vector<any> values) -> any // push([1, 2, 3], 4) -> [1, 2, 3, 4]
     {
       if (values.size() == 2 && isArray(values[0])) // 인자가 2개이고, 첫번째 인자가 배열인 경우
       {
         toArray(values[0])->values.push_back(values[1]); // 배열에 값을 추가
         return values[0];                                // 배열을 반환
       }
       return nullptr; // 아닐 경우 nullptr 반환
     }},
    // 배열의 마지막 요소를 제거하고 반환하는 내장 함수
    {"pop", [](vector<any> values) -> any // pop([1, 2, 3]) -> 3
     {
       if (values.size() == 1 && isArray(values[0]) && toArray(values[0])->values.size() != 0) // 인자가 1개이고, 첫번째 인자가 배열이고, 배열의 길이가 0이 아닌 경우
       {
         auto result = toArray(values[0])->values.back(); // 배열의 마지막 요소를 result에 저장 (back()은 배열의 마지막 요소를 반환)
         toArray(values[0])->values.pop_back();           // 배열의 마지막 요소를 제거 (pop_back()은 배열의 마지막 요소를 제거)
         return result;                                   // result 반환
       }
       return nullptr; // 아닐 경우 nullptr 반환
     }},
    // 맵에서 키에 해당하는 요소를 제거하고 반환하는 내장 함수
    {"erase", [](vector<any> values) -> any // erase({a: 1, b: 2}, "a") -> 1
     {
       if (values.size() == 2 && isMap(values[0]) && isString(values[1]) &&
           toMap(values[0])->values.count(toString(values[1]))) // 인자가 2개이고, 첫번째 인자가 맵이고, 두번째 인자가 문자열이고, 맵에 두번째 인자가 존재하는 경우
       {
         auto result = toMap(values[0])->values.at(toString(values[1])); // 맵의 두번째 인자에 해당하는 값을 result에 저장 (at()은 맵의 두번째 인자에 해당하는 값을 반환)
         toMap(values[0])->values.erase(toString(values[1]));            // 맵의 두번째 인자에 해당하는 값을 제거 (erase()는 맵의 두번째 인자에 해당하는 값을 제거)
         return result;                                                  // result 반환
       }
       return nullptr; // 아닐 경우 nullptr 반환
     }},
    // 프로그램이 시작된 후 경과한 시간을 반환하는 내장 함수
    {"clock", [](vector<any> values) -> any // clock() -> 0.0
     {
       return static_cast<double>(clock());
     }},
    // 제곱근을 반환하는 내장 함수
    {"sqrt", [](vector<any> values) -> any // sqrt(4) -> 2.0
     {
       return sqrt(toNumber(values[0])); // sqrt()는 제곱근을 반환
     }},
};
