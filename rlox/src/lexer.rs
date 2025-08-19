enum TokenType {

}

struct Token {
	ttype: TokenType,
	line: u32,
	col: u32,
}

struct Lexer {
	source: String,
	tokens: Vec<Token>,

}