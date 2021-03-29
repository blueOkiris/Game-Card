using System;
using System.IO;

namespace Assembler {
    public static class Program {
        public static string projectFolder = "";
        
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
            } catch(UnexpectedCompoundTokenException ucte) {
                Console.WriteLine(ucte.Message);
                Environment.Exit(12);
            } catch(UnexpectedEofException uee) {
                Console.WriteLine(uee.Message);
                Environment.Exit(8);
            } catch(InvalidIncludeException iie) {
                Console.WriteLine(iie.Message);
                Environment.Exit(9);
            } catch(ArgumentOutOfRangeException aoore) {
                Console.WriteLine(aoore.Message);
                Environment.Exit(10);
            } catch(WrongNumberArgsException wae) {
                Console.WriteLine(wae.Message);
                Environment.Exit(11);
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
                        args[0].Substring(0, args[0].Length - 4) + ".hex"
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
                
                case 3:
                    if(args[0] == "-p") {
                        if(!Directory.Exists(args[1])) {
                            Console.WriteLine(
                                "Project folder '{0}' does not exist.", args[1]
                            );
                            Environment.Exit(10);
                        }
                        if(args[1].EndsWith('/')) {
                            projectFolder = args[1];
                        } else {
                            projectFolder = args[1] + '/';
                        }
                        
                        args[2] = projectFolder + args[2];
                        if(!args[2].EndsWith(".gca")) {
                            Console.WriteLine("File does not end in .gca");
                            Environment.Exit(2);
                        }
                        if(!File.Exists(args[2])) {
                            Console.WriteLine(
                                "File '{0}' does not exist", args[2]
                            );
                            Environment.Exit(3);
                        }
                        
                        return (
                            args[2],
                            args[2].Substring(0, args[2].Length - 4) + ".hex"
                        );
                    } else if(args[1] == "-p") {
                        if(!Directory.Exists(args[2])) {
                            Console.WriteLine(
                                "Project folder '{0}' does not exist.", args[2]
                            );
                            Environment.Exit(10);
                        }
                        if(args[2].EndsWith('/')) {
                            projectFolder = args[2];
                        } else {
                            projectFolder = args[2] + '/';
                        }
                        
                        args[0] = projectFolder + args[0];
                        if(!args[0].EndsWith(".gca")) {
                            Console.WriteLine("File does not end in .gca");
                            Environment.Exit(2);
                        }
                        if(!File.Exists(args[0])) {
                            Console.WriteLine(
                                "File '{0}' does not exist", args[0]
                            );
                            Environment.Exit(3);
                        }
                        
                        return (
                            args[0],
                            args[0].Substring(0, args[0].Length - 4) + ".hex"
                        );
                    }
                    
                    Console.WriteLine("Too many arguments provided!");
                    Environment.Exit(4);
                    break;
                
                case 4:
                    if(args[0] == "-p") {
                        if(!Directory.Exists(args[1])) {
                            Console.WriteLine(
                                "Project folder '{0}' does not exist.", args[1]
                            );
                            Environment.Exit(10);
                        }
                        if(args[1].EndsWith('/')) {
                            projectFolder = args[1];
                        } else {
                            projectFolder = args[1] + '/';
                        }
                        
                        args[2] = projectFolder + args[2];
                        if(!args[2].EndsWith(".gca")) {
                            Console.WriteLine("File does not end in .gca");
                            Environment.Exit(2);
                        }
                        if(!File.Exists(args[2])) {
                            Console.WriteLine(
                                "File '{0}' does not exist", args[0]
                            );
                            Environment.Exit(3);
                        }
                        
                        return (args[2], projectFolder + args[3]);
                    } else if(args[1] == "-p") {
                        if(!Directory.Exists(args[2])) {
                            Console.WriteLine(
                                "Project folder '{0}' does not exist.", args[2]
                            );
                            Environment.Exit(10);
                        }
                        if(args[2].EndsWith('/')) {
                            projectFolder = args[2];
                        } else {
                            projectFolder = args[2] + '/';
                        }
                        
                        args[0] = projectFolder + args[0];
                        if(!args[0].EndsWith(".gca")) {
                            Console.WriteLine("File does not end in .gca");
                            Environment.Exit(0);
                        }
                        if(!File.Exists(args[0])) {
                            Console.WriteLine(
                                "File '{0}' does not exist", args[0]
                            );
                            Environment.Exit(3);
                        }
                        
                        return (args[0], projectFolder + args[3]);
                    } else if(args[2] == "-p") {
                        if(!Directory.Exists(args[3])) {
                            Console.WriteLine(
                                "Project folder '{0}' does not exist.", args[3]
                            );
                            Environment.Exit(10);
                        }
                        if(args[2].EndsWith('/')) {
                            projectFolder = args[3];
                        } else {
                            projectFolder = args[3] + '/';
                        }
                        
                        args[0] = projectFolder + args[0];
                        if(!args[0].EndsWith(".gca")) {
                            Console.WriteLine("File does not end in .gca");
                            Environment.Exit(2);
                        }
                        if(!File.Exists(args[0])) {
                            Console.WriteLine(
                                "File '{0}' does not exist", args[0]
                            );
                            Environment.Exit(3);
                        }
                        
                        return (args[0], projectFolder + args[1]);
                    }
                    
                    Console.WriteLine("Too many arguments provided!");
                    Environment.Exit(4);
                    break;
                
                default:
                    Console.WriteLine("Too many arguments provided!");
                    Environment.Exit(4);
                    break;
            }
            
            return ("", "");
        }
    }
}
