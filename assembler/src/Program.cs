using System;
using System.IO;

namespace assembler {
    class Program {
        static void Main(string[] args) {
            var tokens = Lexer.Tokens(File.ReadAllText("docs/example.gca"));
            foreach(var token in tokens) {
                Console.WriteLine(token);
            }
        }
    }
}
