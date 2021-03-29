using System;
using System.Collections.Generic;

namespace Assembler {
    public static class CodeGenerator {
        public static byte[] Generate(CompoundToken ast) {
            var program = new List<byte>();
            var labels = new Dictionary<string, ulong>();
            var jumps = new Dictionary<int, string>();
            
            // Add placeholder for program length
            for(int i = 0; i < 8; i++) {
                program.Add(0);
            }
            
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
                        labels.Add(
                            ((SymbolToken) ident).Source, (ulong) program.Count
                        );
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
            foreach(var jumpLoc in jumps) {
                var toLoc = labels[jumpLoc.Value];
                var toLocArr = new byte[] {
                    (byte) (toLoc >> 56), (byte) (toLoc >> 48),
                    (byte) (toLoc >> 40), (byte) (toLoc >> 32),
                    (byte) (toLoc >> 24), (byte) (toLoc >> 16),
                    (byte) (toLoc >> 8), (byte) toLoc
                };
                for(int i = 0; i < 8; i++) {
                    program[jumpLoc.Key + 1 + i] = toLocArr[i];
                }
            }
            
            // Insert the length at the beginning
            var progLen = (ulong) program.Count;
            program[7] = (byte) progLen;
            program[6] = (byte) (progLen >> 8);
            program[5] = (byte) (progLen >> 16);
            program[4] = (byte) (progLen >> 24);
            program[3] = (byte) (progLen >> 32);
            program[2] = (byte) (progLen >> 40);
            program[1] = (byte) (progLen >> 48);
            program[0] = (byte) (progLen >> 56);
            
            return program.ToArray();
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
                 *  - mov r <lit>, <lit>
                 *  - mov r <lit>, r <lit>
                 */
                case "mov": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    /*
                     * First we need to figure out what kind of move it is
                     * Then we must check to make sure there's the correct num
                     * of arguments
                     * Finally parse the data and construct the command
                     */
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        // These are only in mov
                        case "spr": {
                            if(argList.Count != 4) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var xReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            var yReg = 
                                ((SymbolToken) (
                                    (CompoundToken) argList[2]
                                ).Children[0]).Source == "r" ? 0x04 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[3]
                                ).Children[0]).Source == "r" ? 0x08 : 0;
                            
                            program.Add((byte) (idReg + xReg + yReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(xReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(yReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[2]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[2];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[3]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[3];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spi": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x49 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "bg": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x4E + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        // These are in mov and the other setting instructions
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x10 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x2D + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x56);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x55);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                /*
                 * Spr Options: NONE!!!
                 * Spx Options: Same as mov
                 * Spy Options: Same as mov
                 * Spi Options: NONE!!!
                 * Bg Options: NONE!!!
                 * Reg Options: Same as mov
                 *
                 * So only Spx, Spy, and Reg, and they're same as mov
                 * This whole thing is the same as mov, just different insts
                 */
                case "add": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x14 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x31 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x58);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x57);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                // Same options as add
                case "sub": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x18 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x35 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x5A);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x59);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                // Same options as add
                case "mul": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x1C + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x39 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x5C);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x5B);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                // Same options as add
                case "div": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x20 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x3D + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x5E);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x5D);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                // Same options as add
                case "shr": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x25 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x41 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x60);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x5F);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
                // Same options as add
                case "shl": {
                    // First get the arg list
                    var argList = nestedArgListsToListArgs(
                        (CompoundToken) token.Children[1]
                    );
                    
                    switch(((SymbolToken) (argList[0].Children[0])).Source) {
                        case "spx": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x27 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "spy": {
                            if(argList.Count != 2) {
                                throw new WrongNumberArgsException(
                                    token.File, token
                                );
                            }
                            
                            var idReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[0].Children[1]
                                ).Children[0]).Source == "r" ? 0x01 : 0;
                            var iReg =
                                ((SymbolToken) (
                                    (CompoundToken) argList[1]
                                ).Children[0]).Source == "r" ? 0x02 : 0;
                            
                            program.Add((byte) (0x45 + idReg + iReg));
                            
                            if(idReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[0].Children[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg =
                                    (CompoundToken) argList[0].Children[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                            if(iReg == 0) {
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        (CompoundToken) argList[1]
                                    ).Children[0]
                                ));
                            } else {
                                var reg = (CompoundToken) argList[1];
                                if(((SymbolToken) reg.Children[0]).Source
                                        != "r") {
                                    throw new UnexpectedCompoundTokenException(
                                        token.File,
                                        reg
                                    );
                                }
                                
                                program.Add(integerToByte(
                                    (SymbolToken) (
                                        ((CompoundToken)
                                            reg.Children[1]).Children[0]
                                    )
                                ));
                            }
                        } break;
                        
                        case "r": {
                            // Either integer or just one other register
                            if(argList.Count != 2) {
                                throw new UnexpectedCompoundTokenException(
                                    argList[0].File, argList[0]
                                );
                            }
                            
                            // Make sure the other part of the argument's int
                            var regNum = (CompoundToken) argList[0].Children[1];
                            if(regNum.Children.Length > 1) {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, regNum
                                );
                            }
                            if(((SymbolToken) regNum.Children[0]).Type
                                    != SymbolTokenType.Integer) {
                                throw new UnexpectedSymbolTokenException(
                                    regNum.File,
                                    (SymbolToken) regNum.Children[0]
                                );
                            }
                            
                            // Either we're an integer, or it's another reg
                            var nextArgStart =
                                (SymbolToken) argList[1].Children[0];
                            if(nextArgStart.Source == "r") {
                                // Make sure no sub argument
                                var regNum2 =
                                    (CompoundToken) argList[0].Children[1];
                                if(regNum.Children.Length > 1) {
                                    throw new UnexpectedCompoundTokenException(
                                        regNum2.File, regNum2
                                    );
                                }
                                if(((SymbolToken) regNum2.Children[0]).Type
                                        != SymbolTokenType.Integer) {
                                    throw new UnexpectedSymbolTokenException(
                                        regNum2.File,
                                        (SymbolToken) regNum2.Children[0]
                                    );
                                }
                                
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var regInd1 = integerToByte(
                                    (SymbolToken) regNum2.Children[0]
                                );
                                
                                program.Add(0x62);
                                program.Add(regInd0);
                                program.Add(regInd1);
                            } else if(nextArgStart.Type
                                    == SymbolTokenType.Integer) {
                                // Get the values
                                var regInd0 = integerToByte(
                                    (SymbolToken) regNum.Children[0]
                                );
                                var value = integerToUint32(nextArgStart);
                                
                                program.Add(0x61);
                                program.Add(regInd0);
                                foreach(var bt in value) {
                                    program.Add(bt);
                                }
                            } else {
                                throw new UnexpectedCompoundTokenException(
                                    regNum.File, argList[0]
                                );
                            }
                        } break;
                        
                        default:
                            throw new UnexpectedCompoundTokenException(
                                argList[0].File, argList[0]
                            );
                    }
                } break;
                
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var arg01 =
                        ((CompoundToken) argList[0].Children[1]).Children[0];
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
                                != SymbolTokenType.Integer) {
                        throw new UnexpectedCompoundTokenException(
                            argList[0].File, argList[0]
                        );
                    }
                    var arg10 = argList[1].Children[0];
                    var arg11 =
                        ((CompoundToken) argList[1].Children[1]).Children[0];
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
                                != SymbolTokenType.Integer) {
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                                    (SymbolToken) argChild
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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
                    var argList = (CompoundToken) token.Children[1];
                    if(argList.Children.Length > 1) {
                        throw new WrongNumberArgsException(
                            argList.File,
                            ((CompoundToken) token.Children[1])
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