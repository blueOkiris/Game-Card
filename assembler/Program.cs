using System;
using System.IO;

namespace Assembler {
    public static class Program {
        public static void Main(string[] args) {
            var fileNames = getValidFileNames(args);
            
            try {
                var ast = Parser.Parse(fileNames.Item1);
                var code = CodeGenerator.Generate(ast);
                File.WriteAllBytes(fileNames.Item2, code);
            } catch(IOException ioe) {
                Console.WriteLine("Failed to write bytes to file!");
                Console.WriteLine(ioe.Message);
                Environment.Exit(5);
            } catch(UnexpectedCharacterException uce) {
                Console.WriteLine(uce.Message);
                Environment.Exit(6);
            } catch(UnexpectedSymbolTokenException uste) {
                Console.WriteLine(uste.Message);
                Environment.Exit(7);
            }
        }
        
        private static (string, string) getValidFileNames(string[] args) {
            switch(args.Length) {
                case 0:
                    Console.WriteLine("No input filename provided.");
                    Environment.Exit(1);
                    break;
                
                case 1:
                    if(!args[0].EndsWith(".gca")) {
                        Console.WriteLine("File does not end in .gca");
                        Environment.Exit(2);
                    }
                    if(!File.Exists(args[0])) {
                        Console.WriteLine("File '{0}' does not exist", args[0]);
                        Environment.Exit(3);
                    }
                    return (
                        args[0],
                        args[0].Substring(0, args[0].Length - 4)
                    );
                
                case 2:
                    if(!args[0].EndsWith(".gca")) {
                        Console.WriteLine("File does not end in .gca");
                        Environment.Exit(2);
                    }
                    if(!File.Exists(args[0])) {
                        Console.WriteLine("File '{0}' does not exist", args[0]);
                        Environment.Exit(3);
                    }
                    return (args[0], args[1]);
                
                default:
                    Console.WriteLine("Too many arguments provided!");
                    Environment.Exit(4);
                    break;
            }
            
            return ("", "");
        }
    }
}
