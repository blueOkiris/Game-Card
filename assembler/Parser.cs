
namespace Assembler {
    public interface Token {}
    
    public struct SymbolToken : Token {
        
    }
    
    public struct CompoundToken : Token {
        
    }
    
    public static class Parser {
        public static CompoundToken Parse(string inputFileName) {
            return new CompoundToken();
        }
    }
}