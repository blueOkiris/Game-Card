using System;
using System.IO;

namespace assembler {
    class Program {
        static void Main(string[] args) {
            if(args.Length < 1) {
                Console.WriteLine("Please provide an input file!");
                Environment.Exit(1);
            } else if(!args[0].EndsWith(".gca")) {
                Console.WriteLine("File does not have .gca extension!");
                Environment.Exit(1);
            }
            
            string inputFileName = args[0];
            string outputFileName =
                args[0].Substring(0, args[0].Length - 3) + "hex";
                
            var code = File.ReadAllText(inputFileName);
            var tokens = Lexer.Tokens(code);
            var ast = Parser.BuildProgram(tokens);
            var hex = Assembler.Assemble(ast);
            File.WriteAllBytes(outputFileName, hex);
        }
    }
}
