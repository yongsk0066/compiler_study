#include "Main.h"

auto main(int argc, char **argv) -> int
{
  string sourceCode = R""""(
    function sum(x, y) {
      return x + y;
    }

    function main() {
      var z = sum(10, 20);
      var array = [1, 2, 3];
      print 'Hello, World!', z;
      print array;
      return z;
    }
  )"""";
  // string sourceCode = R"""(
  //   function main(x, y, z) {
  //     var a = 10;
  //     var b = a + x;
  //     var list = [1, 2, a];
  //     var map = {'key1': 'value1', 'key2': b};

  //     if x < y {
  //       print 'x is less than y';
  //     } else {
  //       print 'x is not less than y';
  //     }

  //     for i = 0, i < 10, i = i + 1 {
  //       a = a + i;
  //     }

  //     var result = sum(a, b);

  //     return result;
  //   }

  //   function sum(p, q) {
  //     return p + q;
  //   }
  // )""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  // printSyntaxTree(syntaxTree);
  return 0;
}

// FUNCTION main:
//   PARAMETERS:x y z
//   BLOCK:
//     VAR a:
//       10
//     VAR b:
//       +:
//         LHS:
//           GET_VARIABLE: a
//         RHS:
//           GET_VARIABLE: x
//     VAR list:
//       [
//         1
//         2
//         GET_VARIABLE: a
//       ]
//     VAR map:
//       {
// key1:         "value1"
// key2:         GET_VARIABLE: b
//       }
//     IF:
//       CONDITION:
//         <:
//           LHS:
//             GET_VARIABLE: x
//           RHS:
//             GET_VARIABLE: y
//       BLOCK:
//         PRINT:
//           "x is less than y"
//     ELSE:
//       PRINT:
//         "x is not less than y"
//     FOR:
//       VARIABLE:
//         VAR i:
//           0
//       CONDITION:
//         <:
//           LHS:
//             GET_VARIABLE: i
//           RHS:
//             10
//       EXPRESSION:
//         SET_VARIABLE: i
//           +:
//             LHS:
//               GET_VARIABLE: i
//             RHS:
//               1
//       BLOCK:
//         EXPRESSION:
//           SET_VARIABLE: a
//             +:
//               LHS:
//                 GET_VARIABLE: a
//               RHS:
//                 GET_VARIABLE: i
//     VAR result:
//       CALL:
//         EXPRESSION:
//           GET_VARIABLE: sum
//         ARGUMENT:
//           GET_VARIABLE: a
//         ARGUMENT:
//           GET_VARIABLE: b
//     RETURN:
//       GET_VARIABLE: result
// FUNCTION sum:
//   PARAMETERS:p q
//   BLOCK:
//     RETURN:
//       +:
//         LHS:
//           GET_VARIABLE: p
//         RHS:
//           GET_VARIABLE: q

// FUNCTION main:
//   PARAMETERS:x y
//   BLOCK:
//     VAR z:
//       +:
//         LHS:
//           GET_VARIABLE: x
//         RHS:
//           GET_VARIABLE: y
//     PRINT:
//       "Hello, World!"
//       GET_VARIABLE: z
//     RETURN:
//       GET_VARIABLE: z

// KIND        STRING
// -----------------------
// function    function
// #identifier main
// (           (
// #identifier x
// ,           ,
// #identifier y
// )           )
// {           {
// var         var
// #identifier z
// =           =
// #identifier x
// +           +
// #identifier y
// ;           ;
// print       print
// #String     Hello, World!
// ,           ,
// #identifier z
// ;           ;
// return      return
// #identifier z
// ;           ;
// }           }
// #EndOfToken