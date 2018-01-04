using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

using netlib;

namespace Stack_HW
{
    class Program
    {
        static void Main(string[] args)
        {
            // Initial tcpmgr
            tcpmgr tmgr = new tcpmgr("0.0.0.0", 27015, 5);

            // Start tcpmgr
            tmgr.start(client_task, null);

            while (true)
            {
                String cmd = Console.ReadLine();
                if (cmd == "stop")
                {
                    break;
                }
                else if (cmd == "restart")
                {
                    tmgr.stop();
                    tmgr.start(client_task, null);
                }
            }

            // Stop tcpmgr
            tmgr.stop();
        }

        static void client_task(object usrData, Socket sock)
        {
            ASCIIEncoding enc = new ASCIIEncoding();

            while (true)
            {
                try
                {
                    sock.Send(enc.GetBytes("Hello World\r\n"));
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    break;
                }

                Thread.Sleep(1000);
            }
        }
    }
}
