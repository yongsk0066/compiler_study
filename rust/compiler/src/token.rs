use std::collections::HashMap;
use std::fmt;

#[derive(Debug, PartialEq, Eq, Hash, Clone, Copy)]

pub enum Kind {
  Unknown,
  EndOfToken,

  NullLiteral,
  TrueLiteral,
  FalseLiteral,
  NumberLiteral,
  StringLiteral,
  Identifier,

  Function,
  Return,
  Variable,
  For,
  Break,
  Continue,
  If,
  Elif,
  Else,
  Print,
  PrintLine,

  LogicalAnd,
  LogicalOr,
  Assignment,
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Equal,
  NotEqual,
  LessThan,
  GreaterThan,
  LessOrEqual,
  GreaterOrEqual,

  Comma,
  Colon,
  Semicolon,
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
}

pub struct Token {
  pub kind: Kind,
  pub string: String,
}

impl Token {
  pub fn new(kind: Kind, string: String) -> Token {
      Token { kind, string }
  }
}

impl fmt::Display for Token {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
      write!(f, "{:?}: {}", self.kind, self.string)
  }
}

pub fn to_kind(s: &str) -> Kind {
  let string_to_kind: HashMap<&str, Kind> = [
      ("#unknown", Kind::Unknown),
      ("#EndOfToken", Kind::EndOfToken),

      ("null", Kind::NullLiteral),
      ("true", Kind::TrueLiteral),
      ("false", Kind::FalseLiteral),
      ("#Number", Kind::NumberLiteral),
      ("#String", Kind::StringLiteral),
      ("#identifier", Kind::Identifier),
  
      ("function", Kind::Function),
      ("return", Kind::Return),
      ("var", Kind::Variable),
      ("for", Kind::For),
      ("break", Kind::Break),
      ("continue", Kind::Continue),
      ("if", Kind::If),
      ("elif", Kind::Elif),
      ("else", Kind::Else),
      ("print", Kind::Print),
      ("printLine", Kind::PrintLine),
  
      ("and", Kind::LogicalAnd),
      ("or", Kind::LogicalOr),
  
      ("=", Kind::Assignment),
      ("+", Kind::Add),
      ("-", Kind::Subtract),
      ("*", Kind::Multiply),
      ("/", Kind::Divide),
      ("%", Kind::Modulo),
  
      ("==", Kind::Equal),
      ("!=", Kind::NotEqual),
      ("<", Kind::LessThan),
      (">", Kind::GreaterThan),
      ("<=", Kind::LessOrEqual),
      (">=", Kind::GreaterOrEqual),
  
      (",", Kind::Comma),
      (":", Kind::Colon),
      (";", Kind::Semicolon),
      ("(", Kind::LeftParen),
      (")", Kind::RightParen),
      ("{", Kind::LeftBrace),
      ("}", Kind::RightBrace),
      ("[", Kind::LeftBracket),

      ("]", Kind::RightBracket),
  ].iter().cloned().collect();

  *string_to_kind.get(s).unwrap_or(&Kind::Unknown)
}

pub fn to_string(kind: Kind) -> &'static str {
  let kind_to_string: HashMap<Kind, &str> = [
      (Kind::Unknown, "#unknown"),
      (Kind::EndOfToken, "#EndOfToken"),

      (Kind::NullLiteral, "null"),
      (Kind::TrueLiteral, "true"),
      (Kind::FalseLiteral, "false"),
      (Kind::NumberLiteral, "#Number"),
      (Kind::StringLiteral, "#String"),
      (Kind::Identifier, "#identifier"),
      
      (Kind::Function, "function"),
      (Kind::Return, "return"),
      (Kind::Variable, "var"),
      (Kind::For, "for"),
      (Kind::Break, "break"),
      (Kind::Continue, "continue"),
      (Kind::If, "if"),
      (Kind::Elif, "elif"),
      (Kind::Else, "else"),
      (Kind::Print, "print"),
      (Kind::PrintLine, "printLine"),

      (Kind::LogicalAnd, "and"),
      (Kind::LogicalOr, "or"),

      (Kind::Assignment, "="),
      (Kind::Add, "+"),
      (Kind::Subtract, "-"),
      (Kind::Multiply, "*"),
      (Kind::Divide, "/"),
      (Kind::Modulo, "%"),

      (Kind::Equal, "=="),
      (Kind::NotEqual, "!="),
      (Kind::LessThan, "<"),
      (Kind::GreaterThan, ">"),
      (Kind::LessOrEqual, "<="),
      (Kind::GreaterOrEqual, ">="),

      (Kind::Comma, ","),
      (Kind::Colon, ":"),
      (Kind::Semicolon, ";"),
      (Kind::LeftParen, "("),
      (Kind::RightParen, ")"),
      (Kind::LeftBrace, "{"),
      (Kind::RightBrace, "}"),
      (Kind::LeftBracket, "["),
      (Kind::RightBracket, "]"),
  ].iter().cloned().collect();

  kind_to_string.get(&kind).unwrap_or(&"")
}