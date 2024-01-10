#include <iomanip>
#include <iostream>
#include "Datatype.h"

using std::any_cast;
using std::boolalpha;
using std::ostream;

auto isSize(any value) -> bool
{ // size_t 타입인지 확인
  return value.type() == typeid(size_t);
}

auto toSize(any value) -> size_t // size_t 타입으로 변환
{
  return any_cast<size_t>(value);
}

auto isNull(any value) -> bool // nullptr_t 타입인지 확인
{
  return value.type() == typeid(nullptr_t);
}

auto isTrue(any value) -> bool // bool 타입이고, true인지 확인
{
  return isBoolean(value) && toBoolean(value);
}

auto isFalse(any value) -> bool // bool 타입이고, false인지 확인
{
  return isBoolean(value) && toBoolean(value) == false;
}

auto isBoolean(any value) -> bool // bool 타입인지 확인
{
  return value.type() == typeid(bool);
}

auto toBoolean(any value) -> bool // bool 타입으로 변환
{
  return any_cast<bool>(value);
}

auto isNumber(any value) -> bool // double 타입인지 확인
{
  return value.type() == typeid(double);
}

auto toNumber(any value) -> double // double 타입으로 변환
{
  return any_cast<double>(value);
}

auto isString(any value) -> bool // string 타입인지 확인
{
  return value.type() == typeid(string);
}

auto toString(any value) -> string // string 타입으로 변환

{
  return any_cast<string>(value);
}

auto isArray(any value) -> bool // Array 타입인지 확인
{
  return value.type() == typeid(Array *);
}

auto toArray(any value) -> Array * // Array 타입으로 변환
{
  return any_cast<Array *>(value);
}

auto getValueOfArray(any object, any index) -> any // 배열의 요소를 반환 getValueOfArray([1, 2, 3], 0) -> 1
{
  auto i = static_cast<size_t>(toNumber(index));    // index를 size_t 타입으로 변환
  if (i >= 0 && i < toArray(object)->values.size()) // index가 0보다 크거나 같고, index가 배열의 길이보다 작은 경우
    return toArray(object)->values[i];              // 배열의 index번째 요소를 반환
  return nullptr;                                   // 아닐 경우 nullptr 반환
}

auto setValueOfArray(any object, any index, any value) -> any // 배열의 요소를 수정 setValueOfArray([1, 2, 3], 0, 10) -> [10, 2, 3]
{
  auto i = static_cast<size_t>(toNumber(index));    // index를 size_t 타입으로 변환
  if (i >= 0 && i < toArray(object)->values.size()) // index가 0보다 크거나 같고, index가 배열의 길이보다 작은 경우
    toArray(object)->values[i] = value;             // 배열의 index번째 요소를 value로 수정
  return value;                                     // value 반환
}

auto isMap(any value) -> bool // Map 타입인지 확인
{
  return value.type() == typeid(Map *);
}

auto toMap(any value) -> Map * // Map 타입으로 변환
{
  return any_cast<Map *>(value);
}

auto getValueOfMap(any object, any key) -> any // 맵의 요소를 반환 getValueOfMap({a: 1, b: 2}, "a") -> 1
{
  if (toMap(object)->values.count(toString(key))) // 맵에 key가 존재하는 경우
    return toMap(object)->values[toString(key)];  // 맵의 key번째 요소를 반환
  return nullptr;                                 // 아닐 경우 nullptr 반환
}

auto setValueOfMap(any object, any key, any value) -> any // 맵의 요소를 수정 setValueOfMap({a: 1, b: 2}, "a", 10) -> {a: 10, b: 2}
{
  toMap(object)->values[toString(key)] = value; // 맵의 key번째 요소를 value로 수정
  return value;                                 // value 반환
}

auto isFunction(any value) -> bool // Function 타입인지 확인
{
  return value.type() == typeid(Function *);
}

auto toFunction(any value) -> Function * // Function 타입으로 변환
{
  return any_cast<Function *>(value);
}

auto isBuiltinFunction(any value) -> bool // 내장 함수인지 확인
{
  return value.type() == typeid(function<any(vector<any>)>); // function<any(vector<any>)> 타입인지 확인
}

auto toBuiltinFunction(any value) -> function<any(vector<any>)> // 내장 함수로 변환
{
  return any_cast<function<any(vector<any>)>>(value);
}

auto operator<<(ostream &stream, any &value) -> ostream & // any 타입을 출력
{
  if (isNull(value)) // nullptr_t 타입인 경우
  {
    stream << "null"; // "null" 출력
  }
  else if (isBoolean(value)) // bool 타입인 경우
  {
    stream << boolalpha << any_cast<bool>(value); // bool 타입으로 변환
  }
  else if (isNumber(value)) // double 타입인 경우
  {
    printf("%g", toNumber(value)); // double 타입으로 변환
  }
  else if (isString(value)) // string 타입인 경우
  {
    stream << toString(value); // string 타입으로 변환
  }
  else if (isArray(value)) // Array 타입인 경우
  {
    stream << "[ ";                            // "[ " 출력
    for (auto &value : toArray(value)->values) // 배열의 요소를 출력
      stream << value << ", ";                 // 요소 사이에 공백을 출력
    stream << "]";                             // "]" 출력
  }
  else if (isMap(value)) // Map 타입인 경우
  {
    stream << "{ ";                                 // "{ " 출력
    for (auto &[key, value] : toMap(value)->values) // 맵의 요소를 출력
      stream << key << ":" << value << ", ";        // 요소 사이에 공백을 출력
    stream << "}";                                  // "}" 출력
  }
  return stream;
}
