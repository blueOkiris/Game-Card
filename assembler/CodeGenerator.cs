using System;
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
                
                // Options: None. Everything MUST BE integers
                case "til": {
                    // First go from nested arg-lists to a list of args
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    // Make sure it's just integers
                    foreach(var arg in argList) {
                        var firstChild = arg.Children[0];
                        if(!(firstChild is SymbolToken)) {
                            throw new UnexpectedCompoundTokenException(
                                arg.File, (CompoundToken) firstChild
                            );
                        }
                        if(((SymbolToken) firstChild).Type
                                != SymbolTokenType.Integer) {
                            throw new UnexpectedSymbolTokenException(
                                arg.File, (SymbolToken) firstChild
                            );
                        }
                    }
                    
                    // Make sure there's enough data available
                    if(argList.Count != 9) {
                        throw new WrongNumberArgsException(
                            argList[0].File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Convert all the numbers
                    var data = new List<byte>();
                    foreach(var arg in argList) {
                        data.Add(integerToByte((SymbolToken) arg.Children[0]));
                    }
                    
                    // Add the correct bytes to the program
                    instBytes.Add(0x4D);
                    foreach(var datum in data) {
                        instBytes.Add(datum);
                    }
                } break;
                    
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
        
        private static byte integerToByte(SymbolToken integer) {
            var value = integer.Source;
            if(value.StartsWith("0b")) {
                return Convert.ToByte(value.Substring(2), 2);
            } else if(value.StartsWith("0x")) {
                return Convert.ToByte(value.Substring(2), 16);
            } else {
                return byte.Parse(value);
            }
        }
        
        private static List<CompoundToken> nestedArgListsToListArgs(
                CompoundToken argList) {
            var args = new List<CompoundToken>();
            args.Add((CompoundToken) argList.Children[0]);
            
            if(argList.Children.Length > 1) {
                var subList = nestedArgListsToListArgs(
                    (CompoundToken) argList.Children[2]
                );
                foreach(var arg in subList) {
                    args.Add(arg);
                }
            }
            
            return args;
        }
    }
}