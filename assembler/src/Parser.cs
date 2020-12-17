using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace assembler {
    enum TokenType {
        Program, Label, Instruction, DataList, Data,
        Comma, Colon, Mnemonic, Ident, Int, Register, UpdateWord, NewLine
    }
    
    class Token {
        public TokenType Type;
    }
    
    class SymbolToken : Token {
        public string Source;
        public int Line, Col;
        public override string ToString() {
            return
                "Symbol Token: { '" + Source + "', (" + Line + ":" + Col + "), "
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
    
    class Lexer {
        private static Dictionary<Regex, TokenType> patternsToTypes =
                new Dictionary<Regex, TokenType>() {
            {
                new Regex(@",", RegexOptions.Compiled),
                TokenType.Comma
            }, {
                new Regex(@":", RegexOptions.Compiled),
                TokenType.Colon
            }, {
                new Regex(@"[A-Za-z_][A-Za-z0-9_]+", RegexOptions.Compiled),
                TokenType.Ident
            }, {
                new Regex(@"0x[0-9]+|-?[0-9]+", RegexOptions.Compiled),
                TokenType.Int
            }, {
                new Regex(
                    @"[rR][eE][gG][0-9]+|sprx|spry", RegexOptions.Compiled
                ),
                TokenType.Register
            }, {
                new Regex(@"both|map|sprites", RegexOptions.Compiled),
                TokenType.UpdateWord
            }
        };
        private static string[] mnemonics = {
            "mov", "add", "sub", "mul", "div", "rs", "ls", "upd", "tile", "spr",
            "spri", "bgi", "cmp", "je", "jne", "jl", "jg", "jle", "jge", "jmp"
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
