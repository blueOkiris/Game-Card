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
    }
}