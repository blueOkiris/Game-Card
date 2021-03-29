using System;
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
            var code = File.ReadAllText(inputFileName).ToLower();
            var tokens = lex(inputFileName, code);
            
            // Debug print
            foreach(var token in tokens) {
                Console.WriteLine(token);
            }
            
            var children = new List<Token>();
            
            // { <inst> | <label> | <include> }
            for(int i = 0; i < tokens.Length; i++) {
                switch(tokens[i].Type) {
                    case SymbolTokenType.Keyword:
                        switch(tokens[i].Source) {
                            // parse include stmt
                            case "include": {
                                var include = parseInclude(
                                    inputFileName, tokens, ref i
                                );
                                var fileStr = (SymbolToken) include.Children[1];
                                var fileName =
                                    Program.projectFolder
                                    + fileStr.Source.Substring(
                                        1, fileStr.Source.Length - 2
                                    );
                                if(!File.Exists(fileName)) {
                                    throw new InvalidIncludeException(
                                        inputFileName, fileName
                                    );
                                }
                                var subAst = Parse(fileName);
                                foreach(var token in subAst.Children) {
                                    children.Add(token);
                                }
                            } break;
                            
                            default:
                                throw new UnexpectedSymbolTokenException(
                                    inputFileName, tokens[i]
                                );
                        }
                        break;
                    
                    // parse label
                    case SymbolTokenType.Identifier:
                        break;
                    
                    // parse instruction
                    case SymbolTokenType.Command:
                        children.Add(
                            parseInstruction(inputFileName, tokens, ref i)
                        );
                        break;
                    
                    default:
                        throw new UnexpectedSymbolTokenException(
                            inputFileName, tokens[i]
                        );
                }
            }
            
            return new CompoundToken() {
                Type = CompoundTokenType.Program,
                Children = children.ToArray(),
                StartingLine = 1
            };
        }
        
        private static SymbolToken[] lex(string inputFileName, string code) {
            var tokens = new List<SymbolToken>();
            
            for(int i = 0, line = 1, pos = 1; i < code.Length; i++, pos++) {
                switch(code[i]) {
                    // Skip whitespace
                    case ' ':
                    case '\t':
                    case '\r':
                        continue;
                    
                    // Comments
                    case ';':
                        while(i < code.Length && code[i] != '\n') {
                            i++;
                        }
                        pos = 0;
                        line++;
                        continue;
                    
                    // Keep track of new lines though
                    case '\n':
                        pos = 0;
                        line++;
                        continue;
                    
                    // Lex a string
                    case '\'':
                        tokens.Add(lexString(
                            inputFileName, code, ref i, ref line, ref pos
                        ));
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
                            Type = SymbolTokenType.Colon,
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
                    tokens.Add(lexInteger(code, ref i, line, ref pos));
                    continue;
                }
                
                // If we got here, then we messed up!
                throw new UnexpectedCharacterException(
                    inputFileName, code[i], line, pos
                );
            }
            
            return tokens.ToArray();
        }
        
        // <cmd> <arg-list>
        private static CompoundToken parseInstruction(
                string inputFileName, SymbolToken[] tokens, ref int i) {
            var children = new List<Token>();
            children.Add(tokens[i++]);
            if(i >= tokens.Length) {
                throw new UnexpectedEofException(inputFileName);
            }
            children.Add(parseArgList(inputFileName, tokens, ref i));
            return new CompoundToken() {
                Type = CompoundTokenType.Instruction,
                StartingLine = ((SymbolToken) children[0]).Line,
                Children = children.ToArray()
            };
        }
        
        // <arg> [ ',' <arg-list> ]
        private static CompoundToken parseArgList(
                string inputFileName, SymbolToken[] tokens, ref int i) {
            var children = new List<Token>();
            children.Add(parseArgument(inputFileName, tokens, ref i));
            if(i + 1 < tokens.Length
                    && tokens[i + 1].Type == SymbolTokenType.Comma) {
                i++;
                children.Add(tokens[i++]);
                children.Add(parseArgList(inputFileName, tokens, ref i));
            }
            return new CompoundToken() {
                Type = CompoundTokenType.ArgumentList,
                StartingLine = ((CompoundToken) children[0]).StartingLine,
                Children = children.ToArray()
            };
        }
        
        /*
         *   'r' <arg>   | 'bg' <arg>  | 'spr' <arg>
         * | 'spx' <arg> | 'spy' <arg> | 'spi' <arg>
         * | 'sprs'      | 'map'       | 'gfx' 
         * | 'inp'       | <integer>
         */
        private static CompoundToken parseArgument(
                string inputFileName, SymbolToken[] tokens, ref int i) {
            if(tokens[i].Type != SymbolTokenType.Keyword
                    && tokens[i].Type != SymbolTokenType.Integer) {
                throw new UnexpectedSymbolTokenException(
                    inputFileName, tokens[i]
                );
            }
            
            if(tokens[i].Type == SymbolTokenType.Integer) {
                return new CompoundToken() {
                    Type = CompoundTokenType.Argument,
                    StartingLine = tokens[i].Line,
                    Children = new Token[] { tokens[i] }
                };
            }
            
            switch(tokens[i].Source) {
                // This is the one keyword that isn't part of args
                case "include":
                    throw new UnexpectedSymbolTokenException(
                        inputFileName, tokens[i]
                    );
                
                // These are tokens that don't take another argument
                case "sprs":
                case "map":
                case "gfx":
                case "inp":
                    return new CompoundToken() {
                        Type = CompoundTokenType.Argument,
                        StartingLine = tokens[i].Line,
                        Children = new Token[] { tokens[i] }
                    };
                
                // The rest take args afterwards
                default: {
                    var children = new List<Token>();
                    
                    children.Add(tokens[i++]);
                    if(i >= tokens.Length) {
                        throw new UnexpectedEofException(inputFileName);
                    }
                    children.Add(parseArgument(
                        inputFileName, tokens, ref i
                    ));
                    
                    return new CompoundToken() {
                        Type = CompoundTokenType.Argument,
                        StartingLine = ((SymbolToken) children[0]).Line,
                        Children = children.ToArray()
                    };
                }
            }
        }
        
        // 'include' <string>
        private static CompoundToken parseInclude(
                string inputFileName, SymbolToken[] tokens, ref int i) {
            var children = new List<Token>();
            children.Add(tokens[i++]);
            if(i >= tokens.Length) {
                throw new UnexpectedEofException(inputFileName);
            } else if(tokens[i].Type != SymbolTokenType.String) {
                throw new UnexpectedSymbolTokenException(
                    inputFileName, tokens[i]
                );
            }
            children.Add(tokens[i]);
            return new CompoundToken() {
                Type = CompoundTokenType.Include,
                StartingLine = ((SymbolToken) children[0]).Line,
                Children = children.ToArray()
            };
        }
        
        // /[0-9]+/ | '0b' /[01]+/ | '0x' /[0-9a-f]+/
        private static SymbolToken lexInteger(
                string code, ref int i, int line, ref int pos) {
            var startPos = pos;
            if(code[i] == '0'
                    && i + 1 < code.Length && i + 2 < code.Length
                    && !char.IsDigit(code[i + 1])) {
                switch(code[i + 1]) {
                    // '0b' /[01]+/
                    case 'b':
                        if(code[i + 2] != '0' && code[i + 2] != '1') {
                            return new SymbolToken() {
                                Type = SymbolTokenType.Integer,
                                Line = line,
                                Pos = pos,
                                Source = "0"
                            };
                        } else {
                            var intSrc = new StringBuilder();
                            intSrc.Append("0b");
                            i += 2;
                            pos += 2;
                            
                            do {
                                intSrc.Append(code[i]);
                                i++;
                                pos++;
                            } while(
                                i < code.Length
                                && (code[i] == '0' || code[i] == '1')
                            );
            
                            // Backtrack 1 bc for loop will auto increment
                            i--;
                            pos--;
                            
                            return new SymbolToken() {
                                Type = SymbolTokenType.Integer,
                                Line = line,
                                Pos = startPos,
                                Source = intSrc.ToString()
                            };
                        }
                    
                    // '0x' /[0-9a-f]/
                    case 'x':
                        if(!char.IsDigit(code[i + 2])
                                && (code[i + 2] < 'a' || code[i + 2] > 'f')) {
                            return new SymbolToken() {
                                Type = SymbolTokenType.Integer,
                                Line = line,
                                Pos = pos,
                                Source = "0"
                            };
                        } else {
                            var intSrc = new StringBuilder();
                            intSrc.Append("0b");
                            i += 2;
                            pos += 2;
                            
                            do {
                                intSrc.Append(code[i]);
                                i++;
                                pos++;
                            } while(
                                i < code.Length
                                && (
                                    char.IsDigit(code[i])
                                    && (code[i] >= 'a' && code[i] <= 'f')
                                )
                            );
            
                            // Backtrack 1 bc for loop will auto increment
                            i--;
                            pos--;
                            
                            return new SymbolToken() {
                                Type = SymbolTokenType.Integer,
                                Line = line,
                                Pos = startPos,
                                Source = intSrc.ToString()
                            };
                        }
                    
                    default:
                        return new SymbolToken() {
                            Type = SymbolTokenType.Integer,
                            Line = line,
                            Pos = pos,
                            Source = "0"
                        };
                }
            } else {
                var intSrc = new StringBuilder();
                do {
                    intSrc.Append(code[i]);
                    i++;
                    pos++;
                } while(i < code.Length && char.IsDigit(code[i]));
            
                // Backtrack 1 bc for loop after exit will auto increment
                i--;
                pos--;
                
                return new SymbolToken() {
                    Type = SymbolTokenType.Integer,
                    Line = line,
                    Pos = startPos,
                    Source = intSrc.ToString()
                };
            }
        }
        
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
                string inputFileName, string code,
                ref int i, ref int line, ref int pos) {
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
                            throw new UnexpectedEofException(inputFileName);
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
                throw new UnexpectedEofException(inputFileName);
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