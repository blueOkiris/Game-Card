using System.IO;
using System.Collections.Generic;
using System.Text;

namespace Assembler {
    public interface Token {}
    
    public enum SymbolTokenType {
        Keyword, Identifier, Command, Integer, String,
        Colon, Comma
    }
    
    public enum CompoundTokenType {
        Program, Label, Instruction, Include,
        ArgumentList, Argument
    }
    
    public struct SymbolToken : Token {
        public SymbolTokenType Type;
        public int Line, Pos;
        public string Source;

        public override string ToString() {
            var representation = new StringBuilder();
            representation.Append(Type);
            representation.Append(" { \"");
            representation.Append(Source);
            representation.Append("\", ");
            representation.Append(Line);
            representation.Append(" : ");
            representation.Append(Pos);
            representation.Append(" }");
            return representation.ToString();
        }
    }
    
    public struct CompoundToken : Token {
        public CompoundTokenType Type;
        public Token[] Children;
        public int StartingLine;
    }
    
    public static class Parser {
        private static List<string> keywordList = new List<string>() {
            "include", "r", "bg", "spr", "spx", "spy", "spi",
            "sprs", "map", "gfx", "inp"
        };
        
        private static List<string> cmdList = new List<string>() {
            "mov", "add", "sub", "mul", "div", "shr", "shl",
            "til", "upd", "cmp", "del",
            "jmp", "je", "jne", "jgt", "jlt", "jge", "jle"
        };
        
        public static CompoundToken Parse(string inputFileName) {
            var code = File.ReadAllText(inputFileName);
            var tokens = lex(code);
            
            var children = new List<Token>();
            
            // { <inst> | <label> | <include> }
            for(int i = 0; i < tokens.Length; i++) {
                switch(tokens[i].Type) {
                    case SymbolTokenType.Keyword:
                        switch(tokens[i].Source) {
                            case "include":
                                // parse include stmt
                                break;
                            
                            default:
                                throw new UnexpectedSymbolTokenException(
                                    tokens[i]
                                );
                        }
                        break;
                    
                    case SymbolTokenType.Identifier:
                        // parse label
                        break;
                    
                    case SymbolTokenType.Command:
                        // parse command
                        break;
                    
                    default:
                        throw new UnexpectedSymbolTokenException(tokens[i]);
                }
            }
            
            return new CompoundToken() {
                Type = CompoundTokenType.Program,
                Children = children.ToArray(),
                StartingLine = 1
            };
        }
        
        private static SymbolToken[] lex(string code) {
            var tokens = new List<SymbolToken>();
            
            for(int i = 0, line = 1, pos = 1; i < code.Length; i++, pos++) {
                switch(code[i]) {
                    // Skip whitespace
                    case ' ':
                    case '\t':
                    case '\r':
                        continue;
                    
                    // Keep track of new lines though
                    case '\n':
                        pos = 0;
                        line++;
                        continue;
                    
                    // Lex a string
                    case '\'':
                        tokens.Add(lexString(code, ref i, ref line, ref pos));
                        continue;
                    
                    // Lex comma
                    case ',':
                        tokens.Add(new SymbolToken() {
                            Type = SymbolTokenType.Comma,
                            Line = line, Pos = pos, Source = ","
                        });
                        continue;
                        
                    case ':':
                        tokens.Add(new SymbolToken() {
                            Type = SymbolTokenType.Comma,
                            Line = line, Pos = pos, Source = ":"
                        });
                        continue;
                }
                
                // Lex a keyword, identifier, or command
                if(char.IsLetter(code[i]) || code[i] == '_') {
                    var ident = lexIdentifier(code, ref i, line, ref pos);
                    if(keywordList.Contains(ident.Source)) {
                        ident.Type = SymbolTokenType.Keyword;
                    } else if(cmdList.Contains(ident.Source)) {
                        ident.Type = SymbolTokenType.Command;
                    }
                    tokens.Add(ident);
                    continue;
                }
                
                // Lex an integer
                if(char.IsDigit(code[i])) {
                    
                    continue;
                }
                
                // If we got here, then we messed up!
                throw new UnexpectedCharacterException(code[i], line, pos);
            }
            
            return tokens.ToArray();
        }
        
        // /[0-9]+/ | '0b' /[01]+/ | '0x' /[0-9a-f]+/
        
        
        // /[a-z_][a-z0-9_]*/
        private static SymbolToken lexIdentifier(
                string code, ref int i, int line, ref int pos) {
            var startPos = pos;
            var identSrc = new StringBuilder();
                        
            do {
                identSrc.Append(code[i]);
                i++;
                pos++;
            } while(
                i < code.Length
                && (char.IsLetterOrDigit(code[i]) || code[i] == '_')
            );
            
            // Backtrack 1 bc for loop after exit will auto increment
            i--;
            pos--;
            
            return new SymbolToken() {
                Type = SymbolTokenType.Identifier,
                Line = line,
                Pos = startPos,
                Source = identSrc.ToString()
            };
        }
        
        // /'(\\.|[^\\\'])*'/
        private static SymbolToken lexString(
                string code, ref int i, ref int line, ref int pos) {
            var startLine = line;
            var startPos = pos;
            var strSrc = new StringBuilder();
            strSrc.Append(code[i]);
            i++;
            pos++;
            while(i < code.Length && code[i] != '\'') {
                switch(code[i]) {
                    case '\\':
                        if(i + 1 >= code.Length) {
                            throw new UnexpectedEofException();
                        }
                        strSrc.Append('\\');
                        strSrc.Append(code[i + 1]);
                        if(code[i + 1] == '\n') {
                            line++;
                            pos = 1;
                        } else {
                            pos += 2;
                        }
                        i += 2;
                        break;
                    
                    case '\n':
                        strSrc.Append('\n');
                        line++;
                        pos = 1;
                        i++;
                        break;
                    
                    default:
                        strSrc.Append(code[i]);
                        i++;
                        pos++;
                        break;
                }
            }
            if(i >= code.Length) {
                throw new UnexpectedEofException();
            }
            strSrc.Append('\'');
            return new SymbolToken() {
                Type = SymbolTokenType.String,
                Line = startLine,
                Pos = startPos,
                Source = strSrc.ToString()
            };
        }
    }
}