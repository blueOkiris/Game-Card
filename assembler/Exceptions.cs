using System;

namespace Assembler {
    public class UnexpectedCharacterException : Exception {
        public UnexpectedCharacterException(char c, int line, int pos) :
                base(
                    "Error: Unexpected character '" + c
                        + " found on line " + line + ", pos " + pos
                ) {
        }
    }
    
    public class UnexpectedSymbolTokenException : Exception {
        public UnexpectedSymbolTokenException(Token tok) :
                base("Error: Unexpected token '" + tok + "'") {
        }
    }
    
    public class UnexpectedEofException : Exception {
        public UnexpectedEofException() : base("Error: Unexpected EOF") {
        }
    }
}