using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace netlib
{
    public partial class tcpmgr
    {
        // Private data types
        public delegate void tcpmgr_client_task(ref object usrData, ref Socket sock);

        private class tcpmgr_list
        {
            public Thread tHandle;

            public int sockStatus;
            public Socket clientSock;

            public int occupied;
            public int closeJoin;

            public tcpmgr_client_task client_task;
            public object usrData;
        }

        // Constructor
        public tcpmgr(String hostIP, int hostPort, int maxClient)
        {
            // Check argument
            if(maxClient <= 0)
            {
                throw new ArgumentException("Invalid maxClient argument");
            }

            // Create client manage list
            this.mgrList = new tcpmgr_list[maxClient];
            this.mgrListLen = maxClient;

            // Bind and listen
            IPEndPoint hostAddrInfo = new IPEndPoint(IPAddress.Parse(hostIP), hostPort);
            this.listenSock.Bind(hostAddrInfo);
            this.listenSock.Listen(maxClient);
        }

        // Destructor
        ~tcpmgr()
        {

        }

        // Private objects
        int stop;
        int mgrListLen;
        tcpmgr_list[] mgrList;

        int serverFlag;

        object cond;

        int cleanTaskStatus;
        Thread cleanTask;

        int acceptTaskStatus;
        Thread acceptTask;

        Socket listenSock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        tcpmgr_client_task client_task;
        object usrData;
    }
}
