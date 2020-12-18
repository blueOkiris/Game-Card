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
                    
                    default:
                        Console.WriteLine(
                            "Error: Unknown instruction '{0}' on line {1}",
                            mnemonic.Source, mnemonic.Line
                        );
                        return new byte[] {};
                }
            }
            return hex.ToArray();
        }
    }
}
