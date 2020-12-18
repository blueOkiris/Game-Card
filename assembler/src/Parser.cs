using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System;

namespace assembler {
    enum TokenType {
        Program, Label, Instruction, DataList, Data,
        Comma, Colon, Mnemonic, Ident, Int, Register, UpdateWord, SpritePiece,
        NewLine
    }
    
    class Token {
        public TokenType Type;
    }
    
    class SymbolToken : Token {
        public string Source;
        public int Line, Col;
        public override string ToString() {
            return
                "Symbol: { '" + Source + "', (" + Line + ":" + Col + "), "
                + Type + " }";
        }
    }
    
    class CompoundToken : Token {
        public Token[] Children;
        private static string str(CompoundToken token, int tabInd = 0) {
            var tokenStr = new StringBuilder();
            for(int i = 0; i < tabInd; i++) {
                tokenStr.Append("|--");
            }
            tokenStr.Append(token.Type).Append('\n');
            foreach(var child in token.Children) {
                if(child is CompoundToken) {
                    tokenStr.Append(str(child as CompoundToken, tabInd + 1));
                } else {
                    for(int i = 0; i < tabInd + 1; i++) {
                        tokenStr.Append("|--");
                    }
                    tokenStr.Append(child.ToString()).Append('\n');
                }
            }
            return tokenStr.ToString();
        }
        public override string ToString() {
            return str(this);
        }
    }
    
    static class Parser {
        public static CompoundToken BuildProgram(SymbolToken[] tokens) {
            var children = new List<Token>();
            
            for(int i = 0; i < tokens.Length; i++) {
                switch(tokens[i].Type) {
                    case TokenType.NewLine:
                        continue;
                    case TokenType.Ident:
                        children.Add(parseLabel(ref i, tokens));
                        break;
                    case TokenType.Mnemonic:
                        children.Add(parseInstruction(ref i, tokens));
                        break;
                    default:
                        Console.WriteLine(
                            "Error: Expected label or instruction but received"
                                + " {0} at line {1}, col {2}.",
                            tokens[i].Type, tokens[i].Line, tokens[i].Col
                        );
                        i = tokens.Length;
                        break;
                }
            }
            
            return new CompoundToken {
                Type = TokenType.Program,
                Children = children.ToArray()
            };
        }
        
        private static CompoundToken parseLabel(
                ref int i, SymbolToken[] tokens) {
            var children = new List<Token>();
            children.Add(tokens[i]);
            
            if(++i >= tokens.Length) {
                Console.WriteLine("Error: Unexpected EOF!");
            } else if(tokens[i].Type != TokenType.Colon) {
                Console.WriteLine(
                    "Error: Expected colon but received"
                        + " {0} at line {1}, col {2}.",
                    tokens[i].Type, tokens[i].Line, tokens[i].Col
                );
                i = tokens.Length;
            } else {
                children.Add(tokens[i]);
            }
            
            if(++i >= tokens.Length) {
                Console.WriteLine("Error: Unexpected EOF!");
            } else if(tokens[i].Type != TokenType.NewLine) {
                Console.WriteLine(
                    "Error: Expected new line but received"
                        + " {0} at line {1}, col {2}.",
                    tokens[i].Type, tokens[i].Line, tokens[i].Col
                );
                i = tokens.Length;
            } else {
                children.Add(tokens[i]);
            }
            
            return new CompoundToken {
                Type = TokenType.Label, Children = children.ToArray()
            };
        }
        
        private static CompoundToken parseInstruction(
                ref int i, SymbolToken[] tokens) {
            var children = new List<Token>();
            children.Add(tokens[i]);
            
            i++;
            children.Add(parseDataList(ref i, tokens));
            
            if(++i >= tokens.Length) {
                Console.WriteLine("Error: Unexpected EOF!");
            } else if(tokens[i].Type != TokenType.NewLine) {
                Console.WriteLine(
                    "Error: Expected new line but received"
                        + " {0} at line {1}, col {2}.",
                    tokens[i].Type, tokens[i].Line, tokens[i].Col
                );
                i = tokens.Length;
            } else {
                children.Add(tokens[i]);
            }
            
            return new CompoundToken {
                Type = TokenType.Instruction, Children = children.ToArray()
            };
        }
        
        private static CompoundToken parseDataList(
                ref int i, SymbolToken[] tokens) {
            var children = new List<Token>();
            children.Add(parseData(ref i, tokens));
            while(++i < tokens.Length && tokens[i].Type == TokenType.Comma) {
                children.Add(tokens[i++]);
                children.Add(parseData(ref i, tokens));
            }
            i--;
            return new CompoundToken {
                Type = TokenType.DataList,
                Children = children.ToArray()
            };
        }
        
        private static SymbolToken parseData(
                ref int i, SymbolToken[] tokens) {
            if(i >= tokens.Length) {
                Console.WriteLine("Error: Unexpected EOF!");
                return new SymbolToken {
                    Type = TokenType.NewLine, Source = "", Line = -1, Col = -1
                };
            } else if(tokens[i].Type != TokenType.Int
                    && tokens[i].Type != TokenType.Ident
                    && tokens[i].Type != TokenType.UpdateWord
                    && tokens[i].Type != TokenType.Register
                    && tokens[i].Type != TokenType.SpritePiece) {
                Console.WriteLine(
                    "Error: Expected data aka integer, identifier, "
                        + "register, or upd term but received"
                        + " {0} at line {1}, col {2}.",
                    tokens[i].Type, tokens[i].Line, tokens[i].Col
                );
                i = tokens.Length;
                return new SymbolToken {
                    Type = TokenType.NewLine, Source = "", Line = -1, Col = -1
                };
            }
            
            return tokens[i];
        }
    }
    
    static class Lexer {
        private static Dictionary<Regex, TokenType> patternsToTypes =
                new Dictionary<Regex, TokenType>() {
            {
                new Regex(@",", RegexOptions.Compiled),
                TokenType.Comma
            }, {
                new Regex(@":", RegexOptions.Compiled),
                TokenType.Colon
            }, {
                new Regex(
                    @"[rR][eE][gG][0-9]+", RegexOptions.Compiled
                ),
                TokenType.Register
            }, {
                new Regex(@"[A-Za-z_][A-Za-z0-9_]+", RegexOptions.Compiled),
                TokenType.Ident
            }, {
                new Regex(@"0x[0-9A-Fa-f]+|-?[0-9]+", RegexOptions.Compiled),
                TokenType.Int
            }
        };
        private static string[] mnemonics = {
            "mov", "add", "sub", "mul", "div", "rs", "ls", "upd", "tile", "spr",
            "spri", "bgi", "cmp", "je", "jne", "jl", "jg", "jle", "jge", "jmp"
        };
        private static string[] updateWords = {
            "both", "map", "sprites"
        };
        private static string[] spritePieces = {
            "sprx", "spry"
        };
        
        public static SymbolToken[] Tokens(string code) {
            var tokens = new List<SymbolToken>();

            var len = code.Length;
            for(int ind = 0, line = 1, col = 1; ind < len; ind++, col++) {
                if(code[ind] == '\n') {
                    tokens.Add(new SymbolToken {
                        Type = TokenType.NewLine,
                        Source = "<br>",
                        Line = line,
                        Col = col
                    });
                    line++;
                    col = 0;
                } else if(code[ind] == ';') {
                    while(ind < code.Length && code[ind] != '\n') {
                        ind++;
                        col++;
                    }
                    ind--;
                    col--;
                    continue;
                } else if(char.IsWhiteSpace(code[ind])) {
                    continue;
                }

                foreach(var pattern in patternsToTypes.Keys) {
                    var match = pattern.Match(code.Substring(ind));
                    if(match.Success && match.Index == 0) {
                        var type = patternsToTypes[pattern];
                        
                        if(type == TokenType.Ident) {
                            foreach(var mnemonic in mnemonics) {
                                if(match.Value == mnemonic) {
                                    type = TokenType.Mnemonic;
                                }
                            }
                            foreach(var updateWord in updateWords) {
                                if(match.Value == updateWord) {
                                    type = TokenType.UpdateWord;
                                }
                            }
                            foreach(var spritePiece in spritePieces) {
                                if(match.Value == spritePiece) {
                                    type = TokenType.SpritePiece;
                                }
                            }
                        }
                        
                        tokens.Add(new SymbolToken {
                            Type = type,
                            Source = match.Value,
                            Line = line,
                            Col = col
                        });
                        col += match.Length - 1;
                        ind += match.Length - 1;
                        break;
                    }
                }
            }

            return tokens.ToArray();
        }
    }
}
