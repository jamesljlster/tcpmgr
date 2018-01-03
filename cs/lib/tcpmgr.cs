using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace netlib
{
    public partial class tcpmgr
    {
        // Private data types
        private delegate void tcpmgr_client_task(object usrData);

        private class tcpmgr_list
        {
            Thread tHandle;

            int sockStatus;
            Socket clientSock;

            int occupied;
            int closeJoin;

            tcpmgr_client_task client_task;
            object usrData;
        }

        // Constructor
        public tcpmgr(String hostIP, int hostPort, int maxClient)
        {
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

        int cleanTaskStatus;
        Thread cleanTask;

        int acceptTaskStatus;
        Thread acceptTask;

        Socket listenSock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        tcpmgr_client_task client_task;
        object usrData;
    }
}
