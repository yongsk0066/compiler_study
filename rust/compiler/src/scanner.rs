use crate::token::{Kind, Token};

use std::iter::Peekable;
use std::str::Chars;

#[derive(Debug, Clone, Copy)]
enum CharType {
    Unknown,
    WhiteSpace,
    NumberLiteral,
    StringLiteral,
    IdentifierAndKeyword,
    OperatorAndPunctuator,
}

fn scan_number_literal(chars: &mut Peekable<Chars>) -> Token {
    let mut string = String::new();
    while let Some(&c) = chars.peek() {
        if is_char_type(c, CharType::NumberLiteral) {
            string.push(chars.next().unwrap());
        } else {
            break;
        }
    }
    Token::new(Kind::NumberLiteral, string)
}

fn scan_string_literal(chars: &mut Peekable<Chars>) -> Token {

}

fn scan_identifier_and_keyword(chars: &mut Peekable<Chars>) -> Token {

}

fn scan_operator_and_punctuator(chars: &mut Peekable<Chars>) -> Token {

}

fn get_char_type(c: char) -> CharType {

}

fn is_char_type(c: char, char_type: CharType) -> bool {

}

pub fn scan(source_code: &str) -> Vec<Token> {
    let mut tokens = Vec::new();
    let mut chars = source_code.chars().peekable();

    while let Some(&c) = chars.peek() {
        match get_char_type(c) {
            CharType::WhiteSpace => { chars.next(); },
            CharType::NumberLiteral => tokens.push(scan_number_literal(&mut chars)),

            _ => {
                eprintln!("Invalid character encountered");
                break;
            }
        }
    }

    tokens.push(Token::new(Kind::EndOfToken, String::new()));
    tokens
}

