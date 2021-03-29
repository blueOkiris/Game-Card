using System;

namespace Assembler {
    public class UnexpectedCharacterException : Exception {
        public UnexpectedCharacterException(
                string file, char c, int line, int pos) :
                base(
                    "Error in '" + file + "':\nUnexpected character '" + c
                        + " found on line " + line + ", pos " + pos
                ) {
        }
    }
    
    public class UnexpectedSymbolTokenException : Exception {
        public UnexpectedSymbolTokenException(string file, SymbolToken tok) :
                base(
                    "Error in '" + file + "':\nUnexpected token '" + tok + "'"
                ) {
        }
    }
    
    public class UnexpectedCompoundTokenException : Exception {
        public UnexpectedCompoundTokenException(
                string file, CompoundToken tok) :
                base(
                    "Error in '" + file + "':\nUnexpected token:\n" + tok
                ) {
        }
    }
    
    public class UnexpectedEofException : Exception {
        public UnexpectedEofException(string file) :
            base("Error in '" + file + "':\nUnexpected EOF") {
        }
    }
    
    public class InvalidIncludeException : Exception {
        public InvalidIncludeException(string file, string fileName) :
                base(
                    "Error in '" + file + "':\nCould not include file '"
                        + fileName + "'"
                ) {
        }
    }
}