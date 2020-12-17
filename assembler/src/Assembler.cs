using System;
using System.Collections.Generic;
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
                switch(mnemonic.Source) {
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
