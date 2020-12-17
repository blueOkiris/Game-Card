using System;
using System.IO.Ports;
using System.IO;

namespace RomWriter {
    class Program {
        private static string receivedData = "";
        private static bool connected = false, received = true;
        private static int index = 0;
        
        static void Main(string[] args) {
            if(args.Length < 1) {
                Console.WriteLine("Expected file name as first argument!");
                Environment.Exit(1);
            } else if(args.Length > 2) {
                Console.WriteLine("Too many arguments provided");
            }
            
            if(!File.Exists(args[0])) {
                Console.WriteLine("Cannot find file " + args[0]);
                Environment.Exit(1);
            }
            
            string port = "";
            if(args.Length > 1) {
                port = args[1];
                goto checkValidPortName;
            }
            getValidSerialPortName:
                Console.WriteLine("Serial ports available:");
                Console.WriteLine("-----------------------");
                foreach(var portName in SerialPort.GetPortNames()) {
                    Console.WriteLine(portName);
                }
                
                Console.Write("Enter port name: ");
                port = Console.ReadLine();
            checkValidPortName:
                foreach(var portName in SerialPort.GetPortNames()) {
                    if(port == portName) {
                        goto hasValidPortName;
                    }
                }
                Console.WriteLine("Invalid Port Name given!");
                goto getValidSerialPortName;
            hasValidPortName:
                Console.WriteLine("Using serial port " + port);
            
            var serialPort = new SerialPort(port) {
                BaudRate = 9600, Parity = Parity.None,
                StopBits = StopBits.One, DataBits = 8,
                Handshake = Handshake.None
            };
            serialPort.DataReceived += SerialPortDataReceived;
            serialPort.Open();
            
            while(!receivedData.EndsWith("READY!"));
            Console.WriteLine("\nSending write byte...");
            serialPort.Write(new byte[1] { 0xA5 }, 0, 1);
            while(!receivedData.EndsWith("Received!\r\n"));
            connected = true;
            
            
            var data = File.ReadAllBytes(args[0]);
            //serialPort.Write(data, 0, data.Length);
            for(int i = 0; i < data.Length; i++) {
                while(!received);
                serialPort.Write(data, i, 1);
                received = false;
            }
            while(!received);
            Console.WriteLine();
        }
        
        private static void SerialPortDataReceived(
                object sender, SerialDataReceivedEventArgs eventArgs) {
            var serialPort = (SerialPort) sender;
            var data = serialPort.ReadExisting();
            
            if(!connected) {
                Console.Write(data);
                receivedData += data;
            } else {
                foreach(var c in data) {
                    Console.Write(
                        BitConverter.ToString(new byte[] { (byte) c })
                    );
                    Console.Write(' ');
                    index++;
                    if(index >= 10) {
                        Console.WriteLine();
                        index = 0;
                    }
                }
                received = true;
            }
        }
    }
}
