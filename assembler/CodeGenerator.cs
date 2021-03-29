using System.Collections.Generic;

namespace Assembler {
    public static class CodeGenerator {
        public static byte[] Generate(CompoundToken ast) {
            var program = new List<byte>();
            var labels = new Dictionary<string, int>();
            var jumps = new Dictionary<int, string>();
            
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
                    
                    case CompoundTokenType.Instruction: {
                        var instBytes = interpretInstruction(
                            (CompoundToken) token, ref jumps
                        );
                        foreach(var instByte in instBytes) {
                            program.Add(instByte);
                        }
                    } break;
                    
                    // Shouldn't happen if ast comes from parser
                    default:
                        throw new UnexpectedCompoundTokenException(
                            ((CompoundToken) token).File,
                            (CompoundToken) token
                        );
                }
            }
            
            // Go back and add all the correct locations for jumps
            var progArr = program.ToArray();
            foreach(var jumpLoc in jumps) {
                var toLoc = labels[jumpLoc.Value];
                var toLocArr = new byte[] {
                    (byte) (toLoc >> 56), (byte) (toLoc >> 48),
                    (byte) (toLoc >> 40), (byte) (toLoc >> 32),
                    (byte) (toLoc >> 24), (byte) (toLoc >> 16),
                    (byte) (toLoc >> 8), (byte) toLoc
                };
                for(int i = 0; i < 8; i++) {
                    progArr[jumpLoc.Key + 1 + i] = toLocArr[i];
                }
            }
            
            return progArr;
        }
        
        private static byte[] interpretInstruction(
                CompoundToken token, ref Dictionary<int, string> jumps) {
            var instBytes = new List<byte>();
            
            var cmd = (SymbolToken) token.Children[0];
            switch(cmd.Source) {
                case "mov":
                    break;
                    
                case "add":
                    break;
                    
                case "sub":
                    break;
                    
                case "mul":
                    break;
                    
                case "div":
                    break;
                    
                case "shr":
                    break;
                    
                case "shl":
                    break;
                    
                case "til":
                    break;
                    
                case "upd":
                    break;
                    
                case "cmp":
                    break;
                    
                case "del":
                    break;
                    
                case "jmp":
                    break;
                    
                case "je":
                    break;
                    
                case "jne":
                    break;
                    
                case "jgt":
                    break;
                    
                case "jlt":
                    break;
                    
                case "jge":
                    break;
                    
                case "jle":
                    break;
            }
            
            return instBytes.ToArray();
        }
    }
}