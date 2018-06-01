using System;
using System.Net;
using System.Net.Sockets;

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

            while(true)
            {
                String cmd = Console.ReadLine();
                if(cmd == "stop")
                {
                    break;
                }
            }

            // Stop tcpmgr
            tmgr.stop();
        }

        static void client_task(object usrData, Socket sock)
        {
            sock.Close();
        }
    }
}
