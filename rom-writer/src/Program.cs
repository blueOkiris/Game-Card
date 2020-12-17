using System;
using System.IO.Ports;

namespace RomWriter {
    class Program {
        static void Main(string[] args) {
            if(args.Length < 1) {
                Console.WriteLine("Expected file name as first argument!");
                Environment.Exit(1);
            } else if(args.Length > 2) {
                Console.WriteLine("Too many arguments provided");
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
            
            while(true);
        }
        
        private static void SerialPortDataReceived(
                object sender, SerialDataReceivedEventArgs eventArgs) {
            var serialPort = (SerialPort) sender;
            var data = serialPort.ReadExisting();
            Console.Write(data);
        }
    }
}
