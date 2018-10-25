using System;
using System.IO.Ports;
using System.Threading.Tasks;

namespace ArduinoLauncher
{
    class Program
    {
        static void Main(string[] args)
        {
            SERIAL_ARDUINO arduino = new SERIAL_ARDUINO();

            Task.Run(() => arduino.loopAsync()).Wait();

        }
    }

    class SERIAL_ARDUINO
    {
        public enum SERIAL_STATUS
        {
            OPEN,
            LOAD,
            WAIT,
            CLOSE,
        }

        SERIAL_STATUS sSTATUS = SERIAL_STATUS.CLOSE;
        SerialPort serialPort1 = new SerialPort();
        string[] exeName = { "Vivaldi", "TEST01", "TEST02", "TEST03", "TEST04", };
        string[] exePath = { "\"C:\\Program Files\\Vivaldi\\Application\\vivaldi.exe\"", "TEST01_PATH", "TEST02_PATH", "TEST03_PATH", "TEST04_PATH", "", "", "", "", "" };

        public async Task loopAsync()
        {
            string ret = "";

            while (true)
            {
                if (serialPort1.IsOpen)
                {
                    ret = serialPort1.ReadExisting().Trim();
                    if (ret.Length > 0)
                    {
                        Console.WriteLine(ret);
                    }
                    sSTATUS = Receives(ret, sSTATUS);
                }

                switch (sSTATUS)
                {
                    case SERIAL_STATUS.CLOSE:
                        if (serialPort1.IsOpen)
                        {
                        }
                        else
                        {
                            serialPort1.PortName = "COM6";
                            serialPort1.BaudRate = 9600;
                            serialPort1.DtrEnable = true;

                            serialPort1.Open();
                            sSTATUS = SERIAL_STATUS.OPEN;
                        }
                        break;
                    case SERIAL_STATUS.OPEN:
                        if (serialPort1.IsOpen)
                        {
                            serialPort1.WriteLine(string.Join(",", exeName));
                            await Task.Delay(2000);
                        }
                        break;
                    case SERIAL_STATUS.WAIT:
                        if(serialPort1.IsOpen)
                        {
                            if (ret.Length >= 4 && ret.Substring(0,4) == "0002")
                            {
                                ret = ret.Substring(4,1);
                                int i = Convert.ToInt32(ret);
                                //Process.Start(exePath[i]);
                                Console.WriteLine("PUSH: " + exeName[i]);
                                Console.WriteLine("EXEC: " + exePath[i]);
                            }
                        }
                        else
                        {
                            serialPort1.Close();
                            Console.WriteLine("SERIAL PORT CLOSED");
                            sSTATUS = SERIAL_STATUS.CLOSE;
                        }
                        break;
                }
                await Task.Delay(200);
            }
        }

        public SERIAL_STATUS Receives(string ret, SERIAL_STATUS s)
        {
            if (ret.Length >= 4)
            {
                switch (s)
                {
                    case SERIAL_STATUS.OPEN:
                        if (ret.Substring(0, 4) == "0000")
                        {
                            Console.WriteLine("SERIAL PORT OPENED");
                            Console.WriteLine("WAIT INPUT");
                            return SERIAL_STATUS.WAIT;
                        }
                        break;

                }

            }
            return s;
        }
    }
}
