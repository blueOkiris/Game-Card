using System;
using System.IO;
using System.IO.Ports;

namespace GameCard {
    class Program {
        private static string receivedData = "";
        private static bool connected = false, received = true;
        private static int index = 0;
        
        public static void Main(string[] args) {
            checkArgs(args);
            
            var fileName = args[0];
            var port = getValidPort(args.Length > 1 ? args[1] : "");
            
            port.DataReceived += serialPortDataReceived;
            port.Open();
            
            performHandshake(port);
            writeBytesFromFile(fileName, port);
        }
        
        private static void writeBytesFromFile(
                string fileName, SerialPort port) {
            var data = File.ReadAllBytes(fileName);
            for(int i = 0; i < data.Length; i++) {
                while(!received);
                port.Write(data, i, 1);
                received = false;
            }
            while(!received);
            
            // Pad until we write (have to write 128 byte chunks)
            int j = 128 - (data.Length % 128);
            Console.WriteLine(
                "\nProgram % 128 is {0}. Printing {1} extras!",
                data.Length % 128, j
            );
            while(j >= 0) {
                while(!received);
                port.Write(data, 0, 1);
                received = false;
                j--;
            }
            
            while(!received);
            Console.WriteLine();
        }
        
        private static void performHandshake(SerialPort port) {
            connected = false;
            
            while(!receivedData.EndsWith("READY!"));
            Console.WriteLine("Sending write byte...");
            port.Write(new byte[1] { 0xA5 }, 0, 1);
            while(!receivedData.EndsWith("Received!\r\n"));
            
            connected = true;
        }
        
        private static void serialPortDataReceived(
                object sender, SerialDataReceivedEventArgs eventArgs) {
            var serialPort = (SerialPort) sender;
            var data = serialPort.ReadExisting();
            
            if(!connected) {
                Console.Write(data);
                receivedData += data;
            } else {
                Console.Write(data);
                if((index + 1) % 10 == 0 && index != 0) {
                    Console.Write("; ");
                } else {
                    Console.Write(' ');
                }
                if((index + 1) % 30 == 0 && index != 0) {
                    Console.WriteLine();
                }
                index++;
                received = true;
            }
        }
        
        private static SerialPort getValidPort(string inputPort) {
            var validPortName = false;
            var port = inputPort;
            while(!validPortName) {
                foreach(var portName in SerialPort.GetPortNames()) {
                    if(port == portName) {
                        validPortName = true;
                    }
                }
                
                if(!validPortName) {
                    Console.WriteLine("Serial ports available:");
                    Console.WriteLine("-----------------------");
                    foreach(var portName in SerialPort.GetPortNames()) {
                        Console.WriteLine(portName);
                    }
                    
                    Console.Write("Enter port name: ");
                    port = Console.ReadLine();
                }
            }
            return new SerialPort(port) {
                BaudRate = 9600, Parity = Parity.None,
                StopBits = StopBits.One, DataBits = 8,
                Handshake = Handshake.None
            };
        }
        
        private static void checkArgs(string[] args) {
            if(args.Length < 1) {
                Console.WriteLine("Expected file name as first argument!");
                Environment.Exit(1);
            } else if(args.Length > 2) {
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
