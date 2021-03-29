using System.Collections.Generic;

namespace Assembler {
    public static class CodeGenerator {
        public static byte[] Generate(CompoundToken ast) {
            var program = new List<byte>();
            var labels = new Dictionary<string, int>();
            
            foreach(var token in ast.Children) {
                // Shouldn't happen if ast comes from parser
                if(token is SymbolToken) {
                    throw new UnexpectedSymbolTokenException(
                        ((SymbolToken) token).File,
                        (SymbolToken) token
                    );
                }
                
                switch(((CompoundToken) token).Type) {
                    case CompoundTokenType.Label: {
                        var ident = ((CompoundToken) token).Children[0];
                        labels.Add(((SymbolToken) ident).Source, program.Count);
                    } break;
                    
                    
                    // Shouldn't happen if ast comes from parser
                    default:
                        throw new UnexpectedCompoundTokenException(
                            ((CompoundToken) token).File,
                            (CompoundToken) token
                        );
                }
            }
            
            return program.ToArray();
        }
    }
}