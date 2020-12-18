using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;

namespace assembler {
    static class Assembler {
        public static byte[] Assemble(CompoundToken ast) {
            // Parse out the label indices first as it will be useful
            var labels = new Dictionary<string, int>();
            var instructions = new List<CompoundToken>();
            
            int instrIndex = 0;
            foreach(var child in ast.Children) {
                switch(child.Type) {
                    case TokenType.Instruction:
                        instructions.Add(child as CompoundToken);
                        instrIndex++;
                        break;
                    
                    case TokenType.Label: {
                        var label = child as CompoundToken;
                        var ident = label.Children[0] as SymbolToken;
                        if(labels.ContainsKey(ident.Source)) {
                            Console.WriteLine(
                                "Error: Redefinition of label '{0}' "
                                    + "on line {1}",
                                ident.Source, ident.Line
                            );
                            return new byte[] {};
                        }
                        labels.Add(ident.Source, instrIndex * 10);
                    } break;
                    
                    default:
                        Console.WriteLine(
                            "Error: Unexpected token in byte stream!"
                        );
                        return new byte[] {};
                }
            }
            
            var hex = new List<byte>();
            foreach(var instruction in instructions) {
                var mnemonic = instruction.Children[0] as SymbolToken;
                var dataList =
                    (instruction.Children[1] as CompoundToken).Children;
                switch(mnemonic.Source) {
                    case "tile": {
                        hex.Add((byte) 'T');
                        if(dataList[0].Type != TokenType.Int) {
                            Console.WriteLine(
                                "Error: Expected integer value for tile index "
                                    + "but received {0} on line {1}",
                                instruction.Children[1].Type, mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        var index = dataList[0] as SymbolToken;
                        if(index.Source.StartsWith("0x")) {
                            hex.Add(Convert.ToByte(index.Source, 16));
                        } else {
                            hex.Add(byte.Parse(index.Source));
                        }
                        var data = dataList.Skip(2).Where(
                            child => child.Type == TokenType.Int
                        ).ToArray();
                        if(data.Length < 2) {
                            Console.WriteLine(
                                "Error: Expected 8 integers after tile index, "
                                    + "but received {0} on line {1}.",
                                data.Length, mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        foreach(var datum in data) {
                            if((datum as SymbolToken).Source.StartsWith("0x")) {
                                var b = Convert.ToByte(
                                    (datum as SymbolToken).Source, 16
                                );
                                hex.Add(b);
                            } else {
                                var b = byte.Parse(
                                    (datum as SymbolToken).Source,
                                    NumberStyles.Integer
                                );
                                hex.Add(b);
                            }
                        }
                    } break;
                    
                    case "bgi": {
                        hex.Add((byte) 'B');
                        
                        var indexTok = dataList[0] as SymbolToken;
                        byte index = 0;
                        switch(indexTok.Type) {
                            case TokenType.Register:
                                hex.Add((byte) 'R');
                                index = byte.Parse(
                                    indexTok.Source.Substring(3)
                                );
                                break;
                            case TokenType.Int:
                                hex.Add((byte) 'L');
                                if(indexTok.Source.StartsWith("0x")) {
                                    index = Convert.ToByte(
                                        indexTok.Source, 16
                                    );
                                } else {
                                    index = byte.Parse(indexTok.Source);
                                }
                                break;
                            default:
                                Console.WriteLine(
                                    "Error: Expected number or register for "
                                        + "bg tile index but received {0} on "
                                        + "line {1}.",
                                    dataList[0].Type, mnemonic.Line
                                );
                                return new byte[] {};
                        }
                        hex.Add(index);
                        
                        if(dataList.Skip(1).ToArray().Length != 2) {
                            Console.WriteLine(
                                "Error: Expected ONE integer or register "
                                    + "after index in bgi on line {0}",
                                mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        var tileIndex = dataList[2] as SymbolToken;
                        byte tile = 0;
                        switch(tileIndex.Type) {
                            case TokenType.Register:
                                hex.Add((byte) 'R');
                                tile = byte.Parse(
                                    tileIndex.Source.Substring(3)
                                );
                                break;
                            case TokenType.Int:
                                hex.Add((byte) 'L');
                                if(tileIndex.Source.StartsWith("0x")) {
                                    tile = Convert.ToByte(
                                        tileIndex.Source, 16
                                    );
                                } else {
                                    tile = byte.Parse(tileIndex.Source);
                                }
                                break;
                            default:
                                Console.WriteLine(
                                    "Error: Expected number or register for "
                                        + "bg tile index but received {0} on "
                                        + "line {1}.",
                                    dataList[0].Type, mnemonic.Line
                                );
                                return new byte[] {};
                        }
                        hex.Add(tile);
                        
                        // Just padding
                        while(hex.Count % 10 != 0) {
                            hex.Add(0x00);
                        }
                    } break;
                    
                    case "add":
                    case "sub":
                    case "mul":
                    case "div":
                    case "ls":
                    case "rs":
                    case "mov": {
                        var regName = dataList[0] as SymbolToken;
                        
                        switch(regName.Type) {
                            case TokenType.Register: {
                                hex.Add((byte) 'R');
                                
                                if(dataList.Length != 3) {
                                    Console.WriteLine(
                                        "Error: Expected register index then  "
                                            + " value on line {0}.",
                                        regName.Line
                                    );
                                }
                                hex.Add(
                                    byte.Parse(regName.Source.Substring(3))
                                );
                                
                                var indexTok = dataList[2] as SymbolToken;
                                var indexStr = indexTok.Source;
                                switch(indexTok.Type) {
                                    case TokenType.Int: {
                                        int index = 0;
                                        if(indexStr.StartsWith("0x")) {
                                            index = Convert.ToInt32(
                                                indexStr, 16
                                            );
                                        } else {
                                            index = int.Parse(indexStr);
                                        }
                                        
                                        hex.Add((byte) 'L');
                                        hex.Add((byte) (index >> 24));
                                        hex.Add((byte) (index >> 16));
                                        hex.Add((byte) (index >> 8));
                                        hex.Add((byte) index);
                                    } break;
                                    
                                    case TokenType.Register: {
                                        var regInd = byte.Parse(
                                            indexStr.Substring(3)
                                        );
                                        
                                        hex.Add((byte) 'R');
                                        hex.Add(regInd);
                                    } break;
                                    
                                    default:
                                        Console.WriteLine(
                                            "Error: Expected register or int "
                                                + " for value but "
                                                + " received {0} on line {1}.",
                                            indexTok.Type, indexTok.Line
                                        );
                                        return new byte[] {};
                                }
                            } break;
                                
                            case TokenType.SpritePiece: {
                                hex.Add((byte) 'S');
                                
                                if(dataList.Length != 5) {
                                    Console.WriteLine(
                                        "Error: Expected sprite option, sprite "
                                            + "index, and value on line {0}.",
                                        regName.Line
                                    );
                                }
                                
                                var indexTok = dataList[2] as SymbolToken;
                                var indexStr = indexTok.Source;
                                switch(indexTok.Type) {
                                    case TokenType.Int: {
                                        byte index = 0;
                                        if(indexStr.StartsWith("0x")) {
                                            index = Convert.ToByte(
                                                indexStr, 16
                                            );
                                        } else {
                                            index = byte.Parse(indexStr);
                                        }
                                        
                                        hex.Add((byte) 'L');
                                        hex.Add(index);
                                    } break;
                                    
                                    case TokenType.Register: {
                                        var regInd = byte.Parse(
                                            indexStr.Substring(3)
                                        );
                                        
                                        hex.Add((byte) 'R');
                                        hex.Add(regInd);
                                    } break;
                                    
                                    default:
                                        Console.WriteLine(
                                            "Error: Expected register or int "
                                                + " for sprite index but "
                                                + " received {0} on line {1}.",
                                            indexTok.Type, indexTok.Line
                                        );
                                        return new byte[] {};
                                }
                                
                                if(regName.Source == "sprx") {
                                    hex.Add((byte) 'X');
                                } else {
                                    hex.Add((byte) 'Y');
                                }
                                
                                var valueTok = dataList[4] as SymbolToken;
                                var valueStr = valueTok.Source;
                                switch(valueTok.Type) {
                                    case TokenType.Int: {
                                        byte value = 0;
                                        if(valueStr.StartsWith("0x")) {
                                            value = Convert.ToByte(
                                                valueStr, 16
                                            );
                                        } else {
                                            value = byte.Parse(valueStr);
                                        }
                                        
                                        hex.Add((byte) 'L');
                                        hex.Add(value);
                                    } break;
                                    
                                    case TokenType.Register: {
                                        var regInd = byte.Parse(
                                            valueStr.Substring(3)
                                        );
                                        
                                        hex.Add((byte) 'R');
                                        hex.Add(regInd);
                                    } break;
                                }
                            } break;
                            
                            default:
                                Console.WriteLine(
                                    "Error: Expected register, sprx, or spry, "
                                        + "but received {0} at line {1}.",
                                    regName.Type, regName.Line
                                );
                                break;
                        }
                        
                        switch(mnemonic.Source) {
                            case "mov":
                                hex.Add((byte) 'S');
                                break;
                            case "add":
                                hex.Add((byte) 'R');
                                break;
                            case "sub":
                                hex.Add((byte) 'N');
                                break;
                            case "mul":
                                hex.Add((byte) 'P');
                                break;
                            case "div":
                                hex.Add((byte) 'Q');
                                break;
                            case "ls":
                                hex.Add((byte) '>');
                                break;
                            case "rs":
                                hex.Add((byte) '>');
                                break;
                        }
                        
                        // Just padding
                        while(hex.Count % 10 != 0) {
                            hex.Add(0x00);
                        }
                    } break;
                    
                    case "spr":
                        if(dataList.Length < 7) {
                            Console.WriteLine(
                                "Error: Expected register or int for sprite "
                                    + "index, x position, y position, and tile"
                                    + " on line {0}.",
                                mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        hex.Add((byte) 'S');
                        for(int i = 0; i < 4; i++) {
                            var tok = dataList[i * 2] as SymbolToken;
                            var str = tok.Source;
                            byte value = 0;
                            switch(tok.Type) {
                                case TokenType.Int: {
                                    if(str.StartsWith("0x")) {
                                        value = Convert.ToByte(str, 16);
                                    } else {
                                        value = byte.Parse(str);
                                    }
                                    
                                    hex.Add((byte) 'L');
                                } break;
                                
                                case TokenType.Register: {
                                    value = byte.Parse(str.Substring(3));
                                    hex.Add((byte) 'R');
                                } break;
                                
                                default:
                                    Console.WriteLine(
                                        "Error: Expected register or int "
                                            + " for value but "
                                            + " received {0} on line {1}.",
                                        tok.Type, tok.Line
                                    );
                                    return new byte[] {};
                            }
                            
                            hex.Add(value);
                            if(i == 0) {
                                hex.Add((byte) 'W');
                            }
                        }
                    break;
                    
                    case "spri":
                        if(dataList.Length < 3) {
                            Console.WriteLine(
                                "Error: Expected register or int for sprite "
                                    + "index and tile"
                                    + " on line {0}.",
                                mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        hex.Add((byte) 'S');
                        for(int i = 0; i < 2; i++) {
                            var tok = dataList[i * 2] as SymbolToken;
                            var str = tok.Source;
                            byte value = 0;
                            switch(tok.Type) {
                                case TokenType.Int: {
                                    if(str.StartsWith("0x")) {
                                        value = Convert.ToByte(str, 16);
                                    } else {
                                        value = byte.Parse(str);
                                    }
                                    
                                    hex.Add((byte) 'L');
                                } break;
                                
                                case TokenType.Register: {
                                    value = byte.Parse(str.Substring(3));
                                    hex.Add((byte) 'R');
                                } break;
                                
                                default:
                                    Console.WriteLine(
                                        "Error: Expected register or int "
                                            + " for value but "
                                            + " received {0} on line {1}.",
                                        tok.Type, tok.Line
                                    );
                                    return new byte[] {};
                            }
                            
                            hex.Add(value);
                            if(i == 0) {
                                hex.Add((byte) 'I');
                            }
                        }
                        
                        // Just padding
                        while(hex.Count % 10 != 0) {
                            hex.Add(0x00);
                        }
                    break;
                    
                    case "upd":
                        if(dataList[0].Type != TokenType.UpdateWord) {
                            Console.WriteLine(
                                "Error: Expected update word on line {0}",
                                mnemonic.Line
                            );
                            return new byte[] {};
                        }
                        
                        hex.Add((byte) 'U');
                        
                        switch((dataList[0] as SymbolToken).Source) {
                            case "all":
                                hex.Add((byte) 'A');
                                break;
                            case "map":
                                hex.Add((byte) 'M');
                                break;
                            case "sprites":
                                hex.Add((byte) 'S');
                                break;
                        }
                        
                        // Just padding
                        while(hex.Count % 10 != 0) {
                            hex.Add(0x00);
                        }
                        break;
                    
                    default:
                        Console.WriteLine(
                            "Error: Unknown instruction '{0}' on line {1}",
                            mnemonic.Source, mnemonic.Line
                        );
                        return new byte[] {};
                }
            }
            
            ulong hexSize = (ulong) hex.Count();
            hex.Insert(0, 0);
            hex.Insert(0, 0);
            for(int i = 0; i < 8; i++) {
                hex.Insert(2, (byte) (hexSize >> (i * 8)));
            }
            return hex.ToArray();
        }
    }
}
