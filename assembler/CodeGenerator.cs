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
                    
                    case CompoundTokenType.Instruction:
                        interpretInstruction(
                            (CompoundToken) token, ref program, ref jumps
                        );
                        break;
                    
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
        
        // Could be broken up into further methods, but not necessary
        private static void interpretInstruction(
                CompoundToken token,
                ref List<byte> program, ref Dictionary<int, string> jumps) {
            var cmd = (SymbolToken) token.Children[0];
            switch(cmd.Source) {
                /*
                 * Spr Options:
                 *  - mov spr <lit>, <lit>, <lit>, <lit>
                 *  - mov spr r <lit>, <lit>, <lit>, <lit>
                 *  - mov spr <lit>, r <lit>, <lit>, <lit>
                 *  - mov spr r <lit>, r <lit>, <lit>, <lit>
                 *  - mov spr r <lit>, <lit>, r <lit>, <lit>
                 *  ... Any can be <lit> or r <lit>.
                 *
                 * Spx Options:
                 *  - mov spx <lit>, <lit>
                 *  - mov spx r <lit>, <lit>
                 *  - mov spx <lit>, r <lit>
                 *  - mov spx r <lit>, r <lit>
                 *
                 * Spy Options: Same as spx
                 * Spi Options: Same as spx
                 * Bg Options: Same as spx
                 *
                 * Reg options:
                 *  - mov r <lit>, <lit>, <lit>, <lit>, <lit>
                 *  - mov r <lit>, r <lit>
                 */
                case "mov":
                    break;
                
                /*
                 * Spr Options: NONE!!!
                 * Spx Options: Same as mov
                 * Spy Options: Same as mov
                 * Spi Options: NONE!!!
                 * Bg Options: NONE!!!
                 * Reg Options: Same as mov
                 *
                 * So only Spx, Spy, and Reg, and they're same as mov
                 */
                case "add":
                    break;
                
                // Same options as add
                case "sub":
                    break;
                
                // Same options as add
                case "mul":
                    break;
                
                // Same options as add
                case "div":
                    break;
                
                // Same options as add
                case "shr":
                    break;
                
                // Same options as add
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
                    program.Add(0x4D);
                    foreach(var datum in data) {
                        program.Add(datum);
                    }
                } break;
                
                // Options: sprs, map, gfx, or inp
                case "upd": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    switch(argChild.Source) {
                        case "gfx":
                            program.Add(0x52);
                            break;
                            
                        case "sprs":
                            program.Add(0x53);
                            break;
                        
                        case "map":
                            program.Add(0x54);
                            break;
                        
                        case "inp":
                            program.Add(0x6B);
                            break;
                        
                        // We should have caught it, but just in case
                        default:
                            throw new UnexpectedCompoundTokenException(
                                arg.File, arg
                            );
                    }
                } break;
                    
                case "cmp": {
                    // Make sure it's just two
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    if(argList.Count != 2) {
                        throw new WrongNumberArgsException(
                            argList[0].File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Both should be registers
                    var arg00 = argList[0].Children[0];
                    var arg01 = argList[0].Children[1];
                    if(argList[0].Children.Length != 2) {
                        throw new UnexpectedCompoundTokenException(
                            argList[0].File, argList[0]
                        );
                    }
                    if(!(arg00 is SymbolToken) || !(arg01 is SymbolToken)) {
                        throw new UnexpectedCompoundTokenException(
                            argList[0].File, argList[0]
                        );
                    }
                    if(((SymbolToken) arg00).Type != SymbolTokenType.Keyword
                            || ((SymbolToken) arg01).Type
                                != SymbolTokenType.Keyword) {
                        throw new UnexpectedCompoundTokenException(
                            argList[0].File, argList[0]
                        );
                    }
                    var arg10 = argList[1].Children[0];
                    var arg11 = argList[1].Children[1];
                    if(argList[1].Children.Length != 2) {
                        throw new UnexpectedCompoundTokenException(
                            argList[1].File, argList[1]
                        );
                    }
                    if(!(arg10 is SymbolToken) || !(arg11 is SymbolToken)) {
                        throw new UnexpectedCompoundTokenException(
                            argList[1].File, argList[1]
                        );
                    }
                    if(((SymbolToken) arg10).Type != SymbolTokenType.Keyword
                            || ((SymbolToken) arg11).Type
                                != SymbolTokenType.Keyword) {
                        throw new UnexpectedCompoundTokenException(
                            argList[1].File, argList[1]
                        );
                    }
                    
                    // Get the register indices
                    var regInd0 = integerToByte((SymbolToken) arg01);
                    var regInd1 = integerToByte((SymbolToken) arg11);
                    
                    program.Add(0x63);
                    program.Add(regInd0);
                    program.Add(regInd1);
                } break;
                    
                // Options: reg or value
                case "del": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    var argChild = (SymbolToken) arg.Children[0];
                    switch(argChild.Source) {
                        case "r": {
                            var ind = integerToByte(
                                (SymbolToken) arg.Children[1]
                            );
                            program.Add(0x6D);
                            program.Add(ind);
                        } break;
                        
                        // We should have caught it, but just in case
                        default:
                            if(((SymbolToken) argChild).Type
                                    == SymbolTokenType.Integer) {
                                var duration = integerToUint32(
                                    (SymbolToken) arg.Children[1]
                                );
                                program.Add(0x6C);
                                foreach(var b in duration) {
                                    program.Add(b);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    arg.File, arg
                                );
                            }
                            break;
                    }
                } break;
                    
                /*
                 * All of the jumps take the same 1 arg, so they're similar
                 * That one argument is an identifier
                 */
                
                case "jmp": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x64);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "je": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x65);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "jne": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x6A);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "jgt": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x67);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "jlt": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x66);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "jge": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x69);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
                    
                case "jle": {
                    // Should only have one argument
                    var argList = (CompoundToken) token.Children[0];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[0])
                        );
                    }
                    
                    // Make sure it's a valid argument
                    var arg = (CompoundToken) argList.Children[0];
                    if(arg.Children.Length > 1) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    var argChild = (SymbolToken) arg.Children[0];
                    if(argChild.Type != SymbolTokenType.Identifier) {
                        throw new UnexpectedCompoundTokenException(
                            arg.File, arg
                        );
                    }
                    
                    jumps.Add(program.Count, argChild.Source);
                    program.Add(0x68);
                    for(int i = 0; i < 8; i++) {
                        program.Add(0x00);
                    }
                } break;
            }
        }
        
        private static byte[] integerToUint32(SymbolToken integer) {
            var value = integer.Source;
            UInt32 parsed;
            if(value.StartsWith("0b")) {
                parsed = Convert.ToUInt32(value.Substring(2), 2);
            } else if(value.StartsWith("0x")) {
                parsed = Convert.ToUInt32(value.Substring(2), 16);
            } else {
                parsed = UInt32.Parse(value);
            }
            return new byte[] {
                (byte) (parsed >> 24), (byte) (parsed >> 16),
                (byte) (parsed >> 8), (byte) parsed
            };
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