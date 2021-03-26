using System;
using System.IO;
using System.Text;

namespace GameCard {
    class Program {
        public static void Main(string[] args) {
            checkArgs(args);
            var fileName = args[0];
            var gameFile = File.ReadAllBytes(fileName);
            var gameFiles = createCCodeFromByteArr(gameFile);
            //Console.WriteLine(gameFileHeader);
            buildCmakeProject(gameFiles);
        }
        
        private static void buildCmakeProject(
                (string, string) gameFiles) {
            // Create project dir if needed
            if(!Directory.Exists("writer-proj")) {
                Directory.CreateDirectory("writer-proj");
            }
            
            // Delete old file
            if(File.Exists("writer-proj/CMakeLists.txt")) {
                File.Delete("writer-proj/CMakeLists.txt");
            }
            if(File.Exists("writer-proj/pico_sdk_import.cmake")) {
                File.Delete("writer-proj/pico_sdk_import.cmake");
            }
            if(File.Exists("writer-proj/main.cpp")) {
                File.Delete("writer-proj/main.cpp");
            }
            if(File.Exists("writer-proj/GameFile.hpp")) {
                File.Delete("writer-proj/GameFile.hpp");
            }
            if(File.Exists("writer-proj/GameFile.cpp")) {
                File.Delete("writer-proj/GameFile.cpp");
            }
            
            // Create all the files
            File.WriteAllText(
                "writer-proj/CMakeLists.txt", CProjectFiles.CMakeLists
            );
            File.WriteAllText(
                "writer-proj/pico_sdk_import.cmake", CProjectFiles.PicoSdkImport
            );
            File.WriteAllText(
                "writer-proj/main.cpp", CProjectFiles.MainFile
            );
            File.WriteAllText(
                "writer-proj/GameFile.hpp", gameFiles.Item1
            );
            File.WriteAllText(
                "writer-proj/GameFile.cpp", gameFiles.Item2
            );
        }
        
        private static (string, string) createCCodeFromByteArr(
                byte[] gameFile) {
            var header = new StringBuilder();
            header.Append("#pragma once\n");
            header.Append("\n#include <pico/stdlib.h>\n");
            header.Append("\n#define GAME_FILE_LEN ");
            header.Append(gameFile.Length);
            header.Append("\nextern uint8_t gameFile_g[");
            header.Append(gameFile.Length);
            header.Append("];\n");
            
            var cfile = new StringBuilder();
            cfile.Append("#include \"GameFile.hpp\"\n");
            cfile.Append("\nuint8_t gameFile_g[");
            cfile.Append(gameFile.Length);
            cfile.Append("] = {\n");
            for(int i = 0; i < gameFile.Length; i += 16) {
                cfile.Append("    ");
                for(int j = 0; j < 16; j++) {
                    var index = i + j;
                    if(index >= gameFile.Length) {
                        break;
                    }
                    
                    cfile.Append(((int) gameFile[index]) + ", ");
                }
                cfile.Append("\n");
            }
            cfile.Append("};");
            
            return (header.ToString(), cfile.ToString());
        }
        
        private static void checkArgs(string[] args) {
            if(args.Length < 1) {
                Console.WriteLine("Expected file name as first argument!");
                Environment.Exit(1);
            } else if(args.Length > 1) {
                Console.WriteLine("Too many arguments provided");
                Environment.Exit(1);
            }
            
            if(!File.Exists(args[0])) {
                Console.WriteLine("Cannot find file " + args[0]);
                Environment.Exit(1);
            }
        }
    }
}
